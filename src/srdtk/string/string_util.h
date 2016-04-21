#ifndef SRDTK_STRING_STRING_UTIL_H_
#define SRDTK_STRING_STRING_UTIL_H_

#pragma once

#include <tchar.h>
#include <vector>
#include <string>
#include <sstream>

namespace srdtk {

#ifndef UNICODE
typedef std::string         tstring;
typedef std::stringstream   tstringstream;
#else
typedef std::wstring        tstring;
typedef std::wstringstream  tstringstream;
#endif

#ifndef UNICODE
#define tcout std::cout
#else
#define tcout std::wcout
#endif


tstring Format(tstring::const_pointer format, ...);

tstring Replace(const tstring& src,
                const tstring& search,
                const tstring& format);

size_t Split(const tstring& src,
             const tstring& delimiter,
             std::vector<tstring>& splits);

tstring TrimLeft(const tstring& src, const tstring& trim = _T(" "));

tstring TrimRight(const tstring& src, const tstring& trim = _T(" "));

tstring Trim(const tstring& src, const tstring& trim = _T(" "));

tstring Reverse(const tstring& src);

tstring ToUpper(const tstring& src);

tstring ToLower(const tstring& src);

bool IsAlnum(const tstring& src);

bool IsDigit(const tstring& src);

bool IsAlpha(const tstring& src);


std::wstring Ansi2Wide(const char* source, const char* locales = "chs");
std::string Wide2Ansi(const wchar_t* source, const char* locales = "chs");

} // namespace srdtk


#endif /* SRDTK_STRING_STRING_UTIL_H_ */
