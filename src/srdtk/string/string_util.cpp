#include <algorithm>
#include <cctype>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits>
#include <locale>

#include "srdtk/string/string_util.h"
#include "srdtk/object/auto_array.h"

namespace srdtk {

tstring Format(tstring::const_pointer format, ...) {
  va_list args;
  va_start(args, format);

  int nBuffSize = _vsntprintf(NULL, 0, format, args) + 1;

  srdtk::auto_array<TCHAR> s(nBuffSize);
  int nLength = _vsntprintf(s.get(), s.size(), format, args);
  va_end(args);

  if (nLength <= 0) return _T("");
  return tstring(s, nLength);
}

tstring Replace(const tstring& src,
                const tstring& search,
                const tstring& format) {
  tstring result(src);

  tstring::size_type pos;
  tstring::size_type begin = 0;

  while ((pos=result.find(search, begin)) != tstring::npos) {
    result.replace(pos, search.size(), format);
    begin = pos + format.size();
  }

  return result;
}

size_t Split(const tstring& src,
             const tstring& delimiter,
             std::vector<tstring>& splits) {
  tstring input = src;
  splits.clear();

  size_t pos = 0;
  size_t new_pos = -1;
  size_t delimiter_size = delimiter.size();
  size_t input_size = input.size();

  if ((input_size==0) || (delimiter_size==0))
    return 0;
  new_pos = input.find(delimiter, 0);
  if (new_pos == tstring::npos) {
    splits.push_back(src);
    return 1;
  }

  int numFound = 0;
  std::vector<size_t> positions;

  while (new_pos >= pos) {
    numFound++;
    positions.push_back(new_pos);

    pos = new_pos;
    new_pos = input.find(delimiter, pos+delimiter_size);
    if (new_pos == tstring::npos)
      break;
  }

  if (numFound == 0)
    return 0;

  for (int i=0; i<=(int)positions.size(); i++) {
    tstring s(_T(""));
    if (i == 0) {
      s = input.substr(i, positions[i]);
    } else {
      size_t offset = positions[i - 1] + delimiter_size;
      if (offset < input_size) {
        if (i == positions.size())
          s = input.substr(offset);
        else if (i > 0)
          s = input.substr(positions[i-1]+delimiter_size,
                           positions[i]-positions[i-1]-delimiter_size);
      }
    }

    if (!s.empty())
      splits.push_back(s);
  }

  return splits.size();
}

tstring TrimLeft(const tstring& src, const tstring& trim) {
  tstring result = src;

  result.erase(0, result.find_first_not_of(trim));

  return result;
}

tstring TrimRight(const tstring& src, const tstring& trim) {
  tstring result = src;

  result.erase(result.find_last_not_of(trim)+1);

  return result;
}

tstring Trim(const tstring& src, const tstring& trim) {
  tstring result = _T("");

  result = TrimLeft(src, trim);
  result = TrimRight(result, trim);

  return result;
}

tstring Reverse(const tstring& src) {
  tstring result(src);
  std::reverse(result.begin(), result.end());

  return result;
}

tstring ToUpper(const tstring& src) {
  tstring result(src);

  std::transform(result.begin(), result.end(), result.begin(), ::toupper);

  return result;
}

tstring ToLower(const tstring& src) {
  tstring result(src);

  std::transform(result.begin(), result.end(), result.begin(), ::tolower);

  return result;
}

bool IsAlnum(const tstring& src) {
  if (src.empty()) 
    return false;

  for (tstring::const_iterator it=src.begin(); it!=src.end(); ++it) {
    if (*it<'A' || *it>'Z')
      if (*it<'0' || *it>'9')
        if (*it<'a' || *it>'z')
          return false;
  }

  return true;
}

bool IsDigit(const tstring& src) {
  if (src.empty()) 
    return false;

  for (tstring::const_iterator it=src.begin(); it!=src.end(); ++it) {
    if (*it<'0' || *it>'9')
      return false;
  }

  return true;
}

bool IsAlpha(const tstring& src) {
  if (src.empty()) 
    return false;

  for (tstring::const_iterator it=src.begin(); it!=src.end(); ++it) {
    if (*it<'A' || (*it>'Z' && (*it<'a' || *it>'z')))
      return false;
  }

  return true;
}

std::wstring Ansi2Wide(const char* source, const char* locales) {
  std::wstring result = L"";
  if (NULL == source)
    return result;

  setlocale(LC_ALL, locales);

  size_t length;
  size_t destlen = mbstowcs(0, source, 0);
  if (destlen == (size_t)(-1))
    return result;
  length = destlen + 1;
  wchar_t* buff  = new wchar_t[length];
  mbstowcs(buff, source, length);
  result = buff;
  delete buff;
  return result;
}

std::string Wide2Ansi(const wchar_t* source, const char* locales) {
  std::string result = "";
  if (NULL == source)
    return result;

  setlocale(LC_ALL, locales);

  size_t size = wcslen(source) * sizeof(wchar_t);
  char* buff = new char[size + 1];
  memset(buff, 0, size + 1);
  if (NULL == buff)
    return result;
  size_t destlen = wcstombs(buff, source, size);
  if (destlen == (size_t)(0))
    return result;
  result = buff;
  delete buff;
  return result;
}

} // namespace srdtk
