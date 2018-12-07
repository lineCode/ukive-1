#include "ukive/utils/string_utils.h"

#include "ukive/utils/unicode.h"


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

}