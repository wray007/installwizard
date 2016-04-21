#ifndef SRDTK_STRING_STRING_MACROS_H_
#define SRDTK_STRING_STRING_MACROS_H_

#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "srdtk/string/string_util.h"
#include "third_party/utf8/utf8.h"


#define a2w(w, a) do { \
  w = srdtk::Ansi2Wide(a.c_str()); \
} while(0)

#define w2a(a, w) do { \
  a = srdtk::Wide2Ansi(w.c_str()); \
} while(0)

#define w2u(u, w) do { \
  u.clear(); \
  const std::wstring& source = w; \
  utf8::utf16to8(source.begin(), source.end(), back_inserter(u)); \
} while(0)

#define u2w(w, u) do { \
  w.clear(); \
  const std::string& source = u; \
  utf8::utf8to16(source.begin(), source.end(), back_inserter(w)); \
} while(0)

#define a2u(u, a) do { \
  u.clear(); \
  std::wstring w = srdtk::Ansi2Wide(a.c_str()); \
  utf8::utf16to8(w.begin(), w.end(), back_inserter(u)); \
} while(0)

#define u2a(a, u) do { \
  a.clear(); \
  std::wstring w; \
  utf8::utf8to16(u.begin(), u.end(), back_inserter(w)); \
  a = srdtk::Wide2Ansi(w.c_str()); \
} while(0)

#ifndef UNICODE
#define a2t(t, a) t = a
#define t2a(a, t) a = t
#define w2t(t, w) w2a(t, w)
#define t2w(w, t) a2w(w, t)
#define t2u(u, t) a2u(u, t)
#define u2t(t, u) u2a(t, u)
#else
#define a2t(t, a) a2w(t, a)
#define t2a(a, t) w2a(a, t)
#define w2t(t, w) t = w
#define t2w(w, t) w = t
#define t2u(u, t) w2u(u, t)
#define u2t(t, u) u2w(t, u)
#endif

#endif /* SRDTK_STRING_STRING_MACROS_H_ */