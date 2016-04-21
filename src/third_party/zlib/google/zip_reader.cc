// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/zlib/google/zip_reader.h"

#include <fstream>
#include "third_party/zlib/google/zip_internal.h"

#if defined(USE_SYSTEM_MINIZIP)
#include <minizip/unzip.h>
#else
#include "third_party/zlib/contrib/minizip/unzip.h"
#if defined(OS_WIN)
#include "third_party/zlib/contrib/minizip/iowin32.h"
#endif  // defined(OS_WIN)
#endif  // defined(USE_SYSTEM_MINIZIP)

#include "third_party/boost/boost/filesystem.hpp"
#include "third_party/boost/boost/algorithm/string.hpp"
#include "third_party/boost/boost/algorithm/string/replace.hpp"

namespace bfs = boost::filesystem;

namespace zip {

// TODO(satorux): The implementation assumes that file names in zip files
// are encoded in UTF-8. This is true for zip files created by Zip()
// function in zip.h, but not true for user-supplied random zip files.
ZipReader::EntryInfo::EntryInfo(const std::string& file_name_in_zip,
                                const unz_file_info& raw_file_info)
    : is_directory_(false) {
  a2t(file_path_, file_name_in_zip);
  original_size_ = raw_file_info.uncompressed_size;

  // Directory entries in zip files end with "/".
  is_directory_ = file_name_in_zip[file_name_in_zip.length() - 1] == '/';

  // Check the file name here for directory traversal issues. In the name of
  // simplicity and security, we might reject a valid file name such as "a..b".
  is_unsafe_ = file_name_in_zip.find("..") != std::string::npos;

  last_modified_ = raw_file_info.dosDate;
  tmu_date_.tm_year = raw_file_info.tmu_date.tm_year;
  tmu_date_.tm_mon = raw_file_info.tmu_date.tm_mon;
  tmu_date_.tm_mday = raw_file_info.tmu_date.tm_mday;
  tmu_date_.tm_hour = raw_file_info.tmu_date.tm_hour;
  tmu_date_.tm_min = raw_file_info.tmu_date.tm_min;
  tmu_date_.tm_sec = raw_file_info.tmu_date.tm_sec;
}

ZipReader::ZipReader() {
  Reset();
}

ZipReader::~ZipReader() {
  Close();
}

bool ZipReader::Open(const srdtk::tstring& zip_file_path) {
  // Use of "Unsafe" function does not look good, but there is no way to do
  // this safely on Linux. See file_util.h for details.
  std::string file_path;
  t2u(file_path, zip_file_path);
  zip_file_ = internal::OpenForUnzipping(file_path);
  if (!zip_file_) {
    return false;
  }

  return OpenInternal();
}

bool ZipReader::OpenFromPlatformFile(base::PlatformFile zip_fd) {
  if (zip_file_ == NULL)
    return false;

#if defined(OS_POSIX)
  zip_file_ = internal::OpenFdForUnzipping(zip_fd);
#elif defined(OS_WIN)
  zip_file_ = internal::OpenHandleForUnzipping(zip_fd);
#endif
  if (!zip_file_) {
    return false;
  }

  return OpenInternal();
}

bool ZipReader::OpenFromString(const std::string& data) {
  zip_file_ = internal::PreprareMemoryForUnzipping(data);
  if (!zip_file_)
    return false;
  return OpenInternal();
}

void ZipReader::Close() {
  if (zip_file_) {
    unzClose(zip_file_);
  }
  Reset();
}

bool ZipReader::HasMore() {
  return !reached_end_;
}

bool ZipReader::AdvanceToNextEntry() {
  // Should not go further if we already reached the end.
  if (reached_end_)
    return false;

  unz_file_pos position = {};
  if (unzGetFilePos(zip_file_, &position) != UNZ_OK)
    return false;
  const int current_entry_index = position.num_of_file;
  // If we are currently at the last entry, then the next position is the
  // end of the zip file, so mark that we reached the end.
  if (current_entry_index + 1 == num_entries_) {
    reached_end_ = true;
    current_entry_index_ = num_entries_;
  } else {
    if (unzGoToNextFile(zip_file_) != UNZ_OK) {
      return false;
    }
    ++current_entry_index_;
  }
  current_entry_info_.reset();
  return true;
}

bool ZipReader::OpenCurrentEntryInZip() {
  unz_file_info raw_file_info = {};
  char raw_file_name_in_zip[internal::kZipMaxPath] = {};
  const int result = unzGetCurrentFileInfo(zip_file_,
                                           &raw_file_info,
                                           raw_file_name_in_zip,
                                           sizeof(raw_file_name_in_zip) - 1,
                                           NULL,  // extraField.
                                           0,  // extraFieldBufferSize.
                                           NULL,  // szComment.
                                           0);  // commentBufferSize.
  if (result != UNZ_OK)
    return false;
  if (raw_file_name_in_zip[0] == '\0')
    return false;
  current_entry_info_.reset(
      new EntryInfo(raw_file_name_in_zip, raw_file_info));
  return true;
}

bool ZipReader::LocateAndOpenEntry(const srdtk::tstring& path_in_zip) {
  std::string str_path;
  t2a(str_path, path_in_zip);

  current_entry_info_.reset();
  reached_end_ = false;
  const int kDefaultCaseSensivityOfOS = 0;
  const int result = unzLocateFile(zip_file_,
                                   str_path.c_str(),
                                   kDefaultCaseSensivityOfOS);
  if (result != UNZ_OK)
    return false;

  // Then Open the entry.
  return OpenCurrentEntryInZip();
}

bool ZipReader::ExtractCurrentEntryToFilePath(
    const srdtk::tstring& output_file_path) {
  if (zip_file_ == NULL)
    return false;

  bfs::path dir(output_file_path);
  // If this is a directory, just create it and return.
  if (current_entry_info()->is_directory()) {
    if (!bfs::exists(dir))
      return bfs::create_directories(dir.parent_path());
    return true;
  }

  const int open_result = unzOpenCurrentFile(zip_file_);
  if (open_result != UNZ_OK)
    return false;

  // We can't rely on parent directory entries being specified in the
  // zip, so we make sure they are created.
  //srdtk::tstring output_dir_path = output_file_path.DirName();
  //if (!file_util::CreateDirectory(output_dir_path))
  //  return false;
  if (bfs::is_regular_file(dir) && !bfs::exists(dir.parent_path())) {
    if (!bfs::create_directories(dir.parent_path()))
      return false;
  }

  std::ofstream stream(output_file_path.c_str(), std::ios::out | std::ios::binary);
  if (!stream.is_open())
    return false;

  bool success = true;  // This becomes false when something bad happens.
  while (true) {
    char buf[internal::kZipBufSize];
    const int num_bytes_read = unzReadCurrentFile(zip_file_, buf,
                                                  internal::kZipBufSize);
    if (num_bytes_read == 0) {
      // Reached the end of the file.
      break;
    } else if (num_bytes_read < 0) {
      // If num_bytes_read < 0, then it's a specific UNZ_* error code.
      success = false;
      break;
    } else if (num_bytes_read > 0) {
      // Some data is read. Write it to the output file.
      stream.write(buf, num_bytes_read);
    }
  }

  unzCloseCurrentFile(zip_file_);
  return success;
}

bool ZipReader::ExtractCurrentEntryIntoDirectory(
    const srdtk::tstring& output_directory_path) {
  srdtk::tstring output_file_path = output_directory_path;
#if defined(OS_WIN)
  boost::algorithm::replace_all(output_file_path, "\\", "/");
#endif
  zip::internal::CreateDirectory(output_file_path);
  output_file_path.append(_T("/")).append(current_entry_info()->file_path());
  if(ExtractCurrentEntryToFilePath(output_file_path)) {
    std::string file;
    t2a(file, output_file_path);
    zip::internal::ChangeFileTime(file.c_str(), 
                                  current_entry_info()->last_modified(),
                                  current_entry_info()->tmu_date());
    return true;
  }
  return false;
}

#if defined(OS_POSIX)
bool ZipReader::ExtractCurrentEntryToFd(const int fd) {
  // If this is a directory, there's nothing to extract to the file descriptor,
  // so return false.
  if (current_entry_info()->is_directory())
    return false;

  const int open_result = unzOpenCurrentFile(zip_file_);
  if (open_result != UNZ_OK)
    return false;

  std::ofstream stream(output_file_path.c_str(), std::ios::out | std::ios::binary);
  if (!stream.is_open())
    return false;

  bool success = true;  // This becomes false when something bad happens.
  while (true) {
    char buf[internal::kZipBufSize];
    const int num_bytes_read = unzReadCurrentFile(zip_file_, buf,
                                                  internal::kZipBufSize);
    if (num_bytes_read == 0) {
      // Reached the end of the file.
      break;
    } else if (num_bytes_read < 0) {
      // If num_bytes_read < 0, then it's a specific UNZ_* error code.
      success = false;
      break;
    } else if (num_bytes_read > 0) {
      // Some data is read. Write it to the output file descriptor.
      stream.write(buff, num_bytes_read);
    }
  }

  unzCloseCurrentFile(zip_file_);
  return success;
}
#endif  // defined(OS_POSIX)

bool ZipReader::OpenInternal() {
  if (zip_file_ == NULL)
    return false;

  unz_global_info zip_info = {};  // Zero-clear.
  if (unzGetGlobalInfo(zip_file_, &zip_info) != UNZ_OK) {
    return false;
  }
  num_entries_ = zip_info.number_entry;
  if (num_entries_ < 0)
    return false;

  // We are already at the end if the zip file is empty.
  reached_end_ = (num_entries_ == 0);
  return true;
}

void ZipReader::Reset() {
  zip_file_ = NULL;
  num_entries_ = 0;
  current_entry_index_ = 1;
  reached_end_ = false;
  current_entry_info_.reset();
}

}  // namespace zip
