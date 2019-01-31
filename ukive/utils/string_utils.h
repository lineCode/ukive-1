#ifndef UKIVE_UTILS_STRING_UTILS_H_
#define UKIVE_UTILS_STRING_UTILS_H_

#include <string>
#include <vector>


namespace ukive {

    typedef std::string string8;
    typedef std::wstring string16;

    string8 UTF16ToUTF8(const string16& str);
    string16 UTF8ToUTF16(const string8& str);

    string16 ANSIToUTF16(const string8& str);

    string16 toASCIILowerCase(const string16& str);
    string16 toASCIIUpperCase(const string16& str);

    string8 trimString(const string8& str, bool all);
    string16 trimString(const string16& str, bool all);

    std::vector<string8> splitString(const string8& str, const string8& token);
    std::vector<string16> splitString(const string16& str, const string16& token);

}

using string8 = ukive::string8;
using string16 = ukive::string16;


#endif  // UKIVE_UTILS_STRING_UTILS_H_