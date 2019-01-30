#include "ukive/utils/string_utils.h"

#include <algorithm>

#include "ukive/utils/unicode.h"
#include <locale>

namespace ukive {

    string8 UTF16ToUTF8(const string16& str) {
        string8 utf8_str;
        if (Unicode::UTF16ToUTF8(str, &utf8_str)) {
            return utf8_str;
        }

        return {};
    }

    string16 UTF8ToUTF16(const string8& str) {
        string16 utf16_str;
        if (Unicode::UTF8ToUTF16(str, &utf16_str)) {
            return utf16_str;
        }

        return {};
    }

    string16 toASCIILowerCase(const string16& str) {
        string16 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x41 && ch <= 0x5A) {
                ch += 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    string16 toASCIIUpperCase(const string16& str) {
        string16 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x61 && ch <= 0x7A) {
                ch -= 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

}
