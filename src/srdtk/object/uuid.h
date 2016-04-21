#ifndef SRDTK_OBJECT_GUID_H_
#define SRDTK_OBJECT_GUID_H_

#pragma once

#include <ObjBase.h>

#include "srdtk/string/string_util.h"

#ifndef _MSVC_VER
// C++ Exception Specification ignored
// A function was declared using exception specification.
// At this time the implementation details of exception
// specification have not been standardized,
// and are accepted but not implemented in Microsoft Visual C++.
#pragma warning(disable: 4290)
#endif

namespace srdtk {

class uuid {
 public:
  uuid() throw(std::runtime_error) {
    if (::CoCreateGuid(&uuid_) != S_OK) {
      throw std::runtime_error("Call win32api [CoCreateGuid] was failed!");
    }
  }

  uuid(const uuid& u) {
    uuid_ = u.uuid_;
  }

  explicit uuid(UUID uuid) {
    uuid_ = uuid;
  }

  tstring ToString(const srdtk::tstring& format) {
    tstring result = srdtk::Format(
                       _T("%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
                       uuid_.Data1,
                       uuid_.Data2,
                       uuid_.Data3,
                       uuid_.Data4[0],
                       uuid_.Data4[1],
                       uuid_.Data4[2],
                       uuid_.Data4[3],
                       uuid_.Data4[5],
                       uuid_.Data4[6],
                       uuid_.Data4[7],
                       uuid_.Data4[8]);

    return result;
  }

 private:
  UUID uuid_;
};


} // namespace srdtk

#endif /* SRDTK_OBJECT_GUID_H_ */

#ifndef _MSVC_VER
#pragma warning(default: 4290)
#endif