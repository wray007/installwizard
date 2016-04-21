// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/zlib/google/zip.h"

#include <fstream>
#include "third_party/zlib/google/zip_internal.h"
#include "third_party/zlib/google/zip_reader.h"

#if defined(USE_SYSTEM_MINIZIP)
#include <minizip/unzip.h>
#include <minizip/zip.h>
#else
#include "third_party/zlib/contrib/minizip/unzip.h"
#include "third_party/zlib/contrib/minizip/zip.h"
#endif

#include "srdtk/string/string_util.h"
#include "third_party/boost/boost/filesystem.hpp"
#include "third_party/boost/boost/algorithm/string.hpp"
#include "third_party/boost/boost/algorithm/string/replace.hpp"

namespace bfs = boost::filesystem;

namespace {

bool AddFileToZip(zipFile zip_file, const std::string& src_dir) {
  std::ifstream stream(src_dir.c_str(), std::ios::in | std::ios::binary);
  if (!stream.is_open()) {
    // output log here
    return false;
  }

  int num_bytes;
  char buf[zip::internal::kZipBufSize] = {0};
  do {
    stream.read(buf, zip::internal::kZipBufSize);
    num_bytes = static_cast<int>(stream.gcount());
    if (ZIP_OK != zipWriteInFileInZip(zip_file, buf, num_bytes)) {
      // output log here
      return false;
    }
    memset(buf, 0, zip::internal::kZipBufSize);
  } while (!stream.eof());

  return true;
}

bool AddEntryToZip(zipFile zip_file, const std::string& path,
                   const std::string& root_path) {
  std::string str_path = path.substr(root_path.length() + 1);
#if defined(OS_WIN)
  boost::algorithm::replace_all(str_path, "\\", "/");
#endif

  bfs::path file_path(path);
  if (bfs::is_directory(path))
    str_path += "/";

  zip_fileinfo file_info = {0};
  zip::internal::FileTime(path.c_str(), &file_info.tmz_date, 
                          &file_info.dosDate);
  if (ZIP_OK != zipOpenNewFileInZip(
      zip_file, str_path.c_str(),
      &file_info, NULL, 0u, NULL, 0u, NULL,  // file info, extrafield local, length,
                                       // extrafield global, length, comment
      Z_DEFLATED, Z_DEFAULT_COMPRESSION)) {
    //DLOG(ERROR) << "Could not open zip file entry " << str_path;
    return false;
  }

  bool success = true;
  if (!bfs::is_directory(path)) {
    success = AddFileToZip(zip_file, path);
  }

  if (ZIP_OK != zipCloseFileInZip(zip_file)) {
    //DLOG(ERROR) << "Could not close zip file entry " << str_path;
    return false;
  }

  return success;
}

bool ExcludeNoFilesFilter(const std::string& file_path) {
  return true;
}

bool ExcludeHiddenFilesFilter(const std::string& file_path) {
  return file_path.at(0) != _T('.');
}

}  // namespace

namespace zip {

bool Unzip(const srdtk::tstring& zip_file, const srdtk::tstring& dest_dir) {
  ZipReader reader;
  if (!reader.Open(zip_file)) {
    //DLOG(WARNING) << "Failed to open " << src_file.value();
    return false;
  }
  while (reader.HasMore()) {
    if (!reader.OpenCurrentEntryInZip()) {
      //DLOG(WARNING) << "Failed to open the current file in zip";
      return false;
    }
    if (reader.current_entry_info()->is_unsafe()) {
      //DLOG(WARNING) << "Found an unsafe file in zip "
      //              << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.ExtractCurrentEntryIntoDirectory(dest_dir)) {
      //DLOG(WARNING) << "Failed to extract "
      //              << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.AdvanceToNextEntry()) {
      //DLOG(WARNING) << "Failed to advance to the next file";
      return false;
    }
  }
  return true;
}

bool ZipWithFilterCallback(const std::string& src_dir,
                           const std::string& dest_file,
                           const pfnFilterCallback filter_cb) {
  zipFile zip_file = internal::OpenForZipping(dest_file, APPEND_STATUS_CREATE);
  if (!zip_file) {
    //DLOG(WARNING) << "couldn't create file " << dest_file.value();
    return false;
  }

  bfs::path path(src_dir);
  std::string root_path = path.parent_path().string();

  bool success = true;
  if (!AddEntryToZip(zip_file, src_dir, root_path)) {
    success = false;
    return false;
  }

  bfs::recursive_directory_iterator iter(path);
  for (; iter != bfs::recursive_directory_iterator(); ++iter) {
    if (!filter_cb(iter->path().string()))
      continue;
    if (!AddEntryToZip(zip_file, iter->path().string(), root_path)) {
      success = false;
      return false;
    }
  }

  if (ZIP_OK != zipClose(zip_file, NULL)) {
    //DLOG(ERROR) << "Error closing zip file " << dest_file.value();
    return false;
  }

  return success;
}

bool Zip(const std::string& src_dir, const std::string& dest_file,
         bool include_hidden_files) {
  if (include_hidden_files) {
    return ZipWithFilterCallback(src_dir, dest_file, ExcludeNoFilesFilter);
  } else {
    return ZipWithFilterCallback(src_dir, dest_file, ExcludeHiddenFilesFilter);
  }
}

#if defined(OS_POSIX)
bool ZipFiles(const std::string& src_dir,
              const std::vector<std::string>& src_relative_paths,
              int dest_fd) {
  zipFile zip_file = internal::OpenFdForZipping(dest_fd, APPEND_STATUS_CREATE);

  if (!zip_file) {
    //DLOG(ERROR) << "couldn't create file for fd " << dest_fd;
    return false;
  }

  bool success = true;
  for (std::vector<std::string>::const_iterator iter =
           src_relative_paths.begin();
      iter != src_relative_paths.end(); ++iter) {
    const std::string& path = src_dir.Append(*iter);
    if (!AddEntryToZip(zip_file, path, src_dir)) {
      // TODO(hshi): clean up the partial zip file when error occurs.
      success = false;
      break;
    }
  }

  if (ZIP_OK != zipClose(zip_file, NULL)) {
    DLOG(ERROR) << "Error closing zip file for fd " << dest_fd;
    success = false;
  }

  return success;
}
#endif  // defined(OS_POSIX)

bool UnzipWithCallback(const srdtk::tstring& zip_file,
                       const srdtk::tstring& dest_dir,
                       const pfnUnzipCallback callback) {
  ZipReader reader;
  if (!reader.Open(zip_file)) {
    //DLOG(WARNING) << "Failed to open " << src_file.value();
    return false;
  }
  while (reader.HasMore()) {
    if (!reader.OpenCurrentEntryInZip()) {
      //DLOG(WARNING) << "Failed to open the current file in zip";
      return false;
    }
    if (reader.current_entry_info()->is_unsafe()) {
      //DLOG(WARNING) << "Found an unsafe file in zip "
      //              << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.ExtractCurrentEntryIntoDirectory(dest_dir)) {
      //DLOG(WARNING) << "Failed to extract "
      //              << reader.current_entry_info()->file_path().value();
      return false;
    }

    // callback
    if (callback)
      callback(reader.num_entries(), reader.current_entry_index());

    if (!reader.AdvanceToNextEntry()) {
      //DLOG(WARNING) << "Failed to advance to the next file";
      return false;
    }
  }
  return true;
}

}  // namespace zip
