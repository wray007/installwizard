// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/zlib/google/zip.h"

#include <algorithm>

#if defined(USE_SYSTEM_MINIZIP)
#include <minizip/ioapi.h>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#else
#include "third_party/zlib/contrib/minizip/unzip.h"
#include "third_party/zlib/contrib/minizip/zip.h"
#if defined(OS_WIN)
#include "third_party/zlib/contrib/minizip/iowin32.h"
#elif defined(OS_POSIX)
#include "third_party/zlib/contrib/minizip/ioapi.h"
#endif  // defined(OS_POSIX)
#endif  // defined(USE_SYSTEM_MINIZIP)

#include "third_party/boost/boost/filesystem.hpp"

namespace bfs = boost::filesystem;

namespace {

#if defined(OS_WIN)
typedef struct {
  HANDLE hf;
  int error;
} WIN32FILE_IOWIN;

// This function is derived from third_party/minizip/iowin32.c.
// Its only difference is that it treats the char* as UTF8 and
// uses the Unicode version of CreateFile.
void* ZipOpenFunc(void *opaque, const char* filename, int mode) {
  DWORD desired_access, creation_disposition;
  DWORD share_mode, flags_and_attributes;
  HANDLE file = 0;
  void* ret = NULL;

  desired_access = share_mode = flags_and_attributes = 0;

  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ) {
    desired_access = GENERIC_READ;
    creation_disposition = OPEN_EXISTING;
    share_mode = FILE_SHARE_READ;
  } else if (mode & ZLIB_FILEFUNC_MODE_EXISTING) {
    desired_access = GENERIC_WRITE | GENERIC_READ;
    creation_disposition = OPEN_EXISTING;
  } else if (mode & ZLIB_FILEFUNC_MODE_CREATE) {
    desired_access = GENERIC_WRITE | GENERIC_READ;
    creation_disposition = CREATE_ALWAYS;
  }

  srdtk::tstring file_name;
  u2t(file_name, std::string(filename));
  if ((filename != NULL) && (desired_access != 0)) {
    file = CreateFile(file_name.c_str(), desired_access, share_mode,
        NULL, creation_disposition, flags_and_attributes, NULL);
  }

  if (file == INVALID_HANDLE_VALUE)
    file = NULL;

  if (file != NULL) {
    WIN32FILE_IOWIN file_ret;
    file_ret.hf = file;
    file_ret.error = 0;
    ret = malloc(sizeof(WIN32FILE_IOWIN));
    if (ret == NULL)
      CloseHandle(file);
    else
      *(static_cast<WIN32FILE_IOWIN*>(ret)) = file_ret;
  }
  return ret;
}
#endif

#if defined(OS_POSIX)
// Callback function for zlib that opens a file stream from a file descriptor.
void* FdOpenFileFunc(void* opaque, const char* filename, int mode) {
  FILE* file = NULL;
  const char* mode_fopen = NULL;

  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
    mode_fopen = "rb";
  else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
    mode_fopen = "r+b";
  else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
    mode_fopen = "wb";

  if ((filename != NULL) && (mode_fopen != NULL))
    file = fdopen(*static_cast<int*>(opaque), mode_fopen);

  return file;
}

// We don't actually close the file stream since that would close
// the underlying file descriptor, and we don't own it. However we do need to
// flush buffers and free |opaque| since we malloc'ed it in FillFdOpenFileFunc.
int CloseFileFunc(void* opaque, void* stream) {
  fflush(static_cast<FILE*>(stream));
  free(opaque);
  return 0;
}

// Fills |pzlib_filecunc_def| appropriately to handle the zip file
// referred to by |fd|.
void FillFdOpenFileFunc(zlib_filefunc_def* pzlib_filefunc_def, int fd) {
  fill_fopen_filefunc(pzlib_filefunc_def);
  pzlib_filefunc_def->zopen_file = FdOpenFileFunc;
  pzlib_filefunc_def->zclose_file = CloseFileFunc;
  int* ptr_fd = static_cast<int*>(malloc(sizeof(fd)));
  *ptr_fd = fd;
  pzlib_filefunc_def->opaque = ptr_fd;
}
#endif  // defined(OS_POSIX)

#if defined(OS_WIN)
// Callback function for zlib that opens a file stream from a Windows handle.
void* HandleOpenFileFunc(void* opaque, const char* filename, int mode) {
  WIN32FILE_IOWIN file_ret;
  file_ret.hf = static_cast<HANDLE>(opaque);
  file_ret.error = 0;
  if (file_ret.hf == INVALID_HANDLE_VALUE)
    return NULL;

  void* ret = malloc(sizeof(WIN32FILE_IOWIN));
  if (ret != NULL)
    *(static_cast<WIN32FILE_IOWIN*>(ret)) = file_ret;
  return ret;
}
#endif

// A struct that contains data required for zlib functions to extract files from
// a zip archive stored in memory directly. The following I/O API functions
// expect their opaque parameters refer to this struct.
struct ZipBuffer {
  const char* data;  // weak
  size_t length;
  size_t offset;
};

// Opens the specified file. When this function returns a non-NULL pointer, zlib
// uses this pointer as a stream parameter while compressing or uncompressing
// data. (Returning NULL represents an error.) This function initializes the
// given opaque parameter and returns it because this parameter stores all
// information needed for uncompressing data. (This function does not support
// writing compressed data and it returns NULL for this case.)
void* OpenZipBuffer(void* opaque, const char* /*filename*/, int mode) {
  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) != ZLIB_FILEFUNC_MODE_READ) {
    return NULL;
  }
  ZipBuffer* buffer = static_cast<ZipBuffer*>(opaque);
  if (!buffer || !buffer->data || !buffer->length)
    return NULL;
  buffer->offset = 0;
  return opaque;
}

// Reads compressed data from the specified stream. This function copies data
// refered by the opaque parameter and returns the size actually copied.
uLong ReadZipBuffer(void* opaque, void* /*stream*/, void* buf, uLong size) {
  ZipBuffer* buffer = static_cast<ZipBuffer*>(opaque);
  size_t remaining_bytes = buffer->length - buffer->offset;
  if (!buffer || !buffer->data || !remaining_bytes)
    return 0;
  size = std::min(size, static_cast<uLong>(remaining_bytes));
  memcpy(buf, &buffer->data[buffer->offset], size);
  buffer->offset += size;
  return size;
}

// Writes compressed data to the stream. This function always returns zero
// because this implementation is only for reading compressed data.
uLong WriteZipBuffer(void* /*opaque*/,
                     void* /*stream*/,
                     const void* /*buf*/,
                     uLong /*size*/) {
  return 0;
}

// Returns the offset from the beginning of the data.
long GetOffsetOfZipBuffer(void* opaque, void* /*stream*/) {
  ZipBuffer* buffer = static_cast<ZipBuffer*>(opaque);
  if (!buffer)
    return -1;
  return static_cast<long>(buffer->offset);
}

// Moves the current offset to the specified position.
long SeekZipBuffer(void* opaque, void* /*stream*/, uLong offset, int origin) {
  ZipBuffer* buffer = static_cast<ZipBuffer*>(opaque);
  if (!buffer)
    return -1;
  if (origin == ZLIB_FILEFUNC_SEEK_CUR) {
    buffer->offset = std::min(buffer->offset + static_cast<size_t>(offset),
                              buffer->length);
    return 0;
  }
  if (origin == ZLIB_FILEFUNC_SEEK_END) {
    buffer->offset = (buffer->length > offset) ? buffer->length - offset : 0;
    return 0;
  }
  if (origin == ZLIB_FILEFUNC_SEEK_SET) {
    buffer->offset = std::min(buffer->length, static_cast<size_t>(offset));
    return 0;
  }
  return -1;
}

// Closes the input offset and deletes all resources used for compressing or
// uncompressing data. This function deletes the ZipBuffer object referred by
// the opaque parameter since zlib deletes the unzFile object and it does not
// use this object any longer.
int CloseZipBuffer(void* opaque, void* /*stream*/) {
  if (opaque)
    free(opaque);
  return 0;
}

// Returns the last error happened when reading or writing data. This function
// always returns zero, which means there are not any errors.
int GetErrorOfZipBuffer(void* /*opaque*/, void* /*stream*/) {
  return 0;
}

}  // namespace

namespace zip {
namespace internal {

uLong FileTime(const char* file, tm_zip* tmzip, uLong* dt) {
  int ret = 0;
#if defined(OS_WIN)
  FILETIME filetime;
  HANDLE find_handle;
  WIN32_FIND_DATAA wdf;

  find_handle = FindFirstFileA(file, &wdf);
  if (find_handle != INVALID_HANDLE_VALUE) {
    FileTimeToLocalFileTime(&(wdf.ftLastWriteTime), &filetime);
    FileTimeToDosDateTime(&filetime,((LPWORD)dt)+1,((LPWORD)dt)+0);
    FindClose(find_handle);
    ret = 1;
  }
#elif defined(OS_POSIX)
  struct stat s;        /* results of stat() */
  struct tm* filedate;
  time_t tm_t=0;

  if (strcmp(f, "-")!=0) {
    char name[MAXFILENAME+1];
    int len = strlen(f);
    if (len > MAXFILENAME)
      len = MAXFILENAME;

    strncpy(name, f,MAXFILENAME-1);
    /* strncpy doesnt append the trailing NULL, of the string is too long. */
    name[MAXFILENAME] = '\0';

    if (name[len - 1] == '/')
      name[len - 1] = '\0';
    /* not all systems allow stat'ing a file with / appended */
    if (stat(name, &s) == 0) {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedate = localtime(&tm_t);

  tmzip->tm_sec  = filedate->tm_sec;
  tmzip->tm_min  = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon  = filedate->tm_mon ;
  tmzip->tm_year = filedate->tm_year;
#endif
  return ret;
}

void ChangeFileTime(const char* file, uLong dosdate, const tm_unz& tmu_date) {
#if defined(OS_WIN)
  HANDLE file_handle;
  FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

  file_handle = CreateFileA(file, GENERIC_READ | GENERIC_WRITE,
                            0, NULL, OPEN_EXISTING, 0, NULL);
  if (file_handle == INVALID_HANDLE_VALUE)
    return;
  GetFileTime(file_handle, &ftCreate, &ftLastAcc, &ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16), (WORD)dosdate, &ftLocal);
  LocalFileTimeToFileTime(&ftLocal, &ftm);
  SetFileTime(file_handle, &ftm, &ftLastAcc, &ftm);
  CloseHandle(file_handle);
#elif defined(OS_POSIX)
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min = tmu_date.tm_min;
  newdate.tm_hour = tmu_date.tm_hour;
  newdate.tm_mday = tmu_date.tm_mday;
  newdate.tm_mon = tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
    newdate.tm_year = tmu_date.tm_year - 1900;
  else
    newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;
  ut.actime = ut.modtime =mktime(&newdate);
  utime(filename,&ut);
#endif
}

void CreateDirectory(const srdtk::tstring& dir) {
  bfs::path path(dir);
  if (bfs::exists(path) && !bfs::is_directory(path)) {
    bfs::remove_all(path);
  }

  if (!bfs::exists(path)) {
    bfs::create_directories(path);
  }
}

unzFile OpenForUnzipping(const std::string& file_name_utf8) {
  zlib_filefunc_def* zip_func_ptrs = NULL;
#if defined(OS_WIN)
  zlib_filefunc_def zip_funcs;
  fill_win32_filefunc(&zip_funcs);
  zip_funcs.zopen_file = ZipOpenFunc;
  zip_func_ptrs = &zip_funcs;
#endif
  return unzOpen2(file_name_utf8.c_str(), zip_func_ptrs);
}

#if defined(OS_POSIX)
unzFile OpenFdForUnzipping(int zip_fd) {
  zlib_filefunc_def zip_funcs;
  FillFdOpenFileFunc(&zip_funcs, zip_fd);
  // Passing dummy "fd" filename to zlib.
  return unzOpen2("fd", &zip_funcs);
}
#endif

#if defined(OS_WIN)
unzFile OpenHandleForUnzipping(HANDLE zip_handle) {
  zlib_filefunc_def zip_funcs;
  fill_win32_filefunc(&zip_funcs);
  zip_funcs.zopen_file = HandleOpenFileFunc;
  zip_funcs.opaque = zip_handle;
  return unzOpen2("fd", &zip_funcs);
}
#endif

// static
unzFile PreprareMemoryForUnzipping(const std::string& data) {
  if (data.empty())
    return NULL;

  ZipBuffer* buffer = static_cast<ZipBuffer*>(malloc(sizeof(ZipBuffer)));
  if (!buffer)
    return NULL;
  buffer->data = data.data();
  buffer->length = data.length();
  buffer->offset = 0;

  zlib_filefunc_def zip_functions;
  zip_functions.zopen_file = OpenZipBuffer;
  zip_functions.zread_file = ReadZipBuffer;
  zip_functions.zwrite_file = WriteZipBuffer;
  zip_functions.ztell_file = GetOffsetOfZipBuffer;
  zip_functions.zseek_file = SeekZipBuffer;
  zip_functions.zclose_file = CloseZipBuffer;
  zip_functions.zerror_file = GetErrorOfZipBuffer;
  zip_functions.opaque = static_cast<void*>(buffer);
  return unzOpen2(NULL, &zip_functions);
}

zipFile OpenForZipping(const std::string& file_name_utf8, int append_flag) {
  zlib_filefunc_def* zip_func_ptrs = NULL;
#if defined(OS_WIN)
  zlib_filefunc_def zip_funcs;
  fill_win32_filefunc(&zip_funcs);
  zip_funcs.zopen_file = ZipOpenFunc;
  zip_func_ptrs = &zip_funcs;
#endif
  return zipOpen2(file_name_utf8.c_str(),
                  append_flag,
                  NULL,  // global comment
                  zip_func_ptrs);
}

#if defined(OS_POSIX)
zipFile OpenFdForZipping(int zip_fd, int append_flag) {
  zlib_filefunc_def zip_funcs;
  FillFdOpenFileFunc(&zip_funcs, zip_fd);
  // Passing dummy "fd" filename to zlib.
  return zipOpen2("fd", append_flag, NULL, &zip_funcs);
}
#endif

}  // namespace internal
}  // namespace zip
