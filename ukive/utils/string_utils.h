#ifndef UKIVE_UTILS_STRING_UTILS_H_
#define UKIVE_UTILS_STRING_UTILS_H_

#include <string>

namespace ukive {

    typedef std::string string8;
    typedef std::wstring string16;

    string8 UTF16ToUTF8(const string16& str);
    string16 UTF8ToUTF16(const string8& str);

}

using string8 = ukive::string8;
using string16 = ukive::string16;


#endif  // UKIVE_UTILS_STRING_UTILS_H_