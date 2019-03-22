#include "number.h"

#include <sstream>

#include "ukive/log.h"


namespace ukive {

    wchar_t Number::digitc_[] = {
        L'0',L'1',L'2',L'3',L'4',
        L'5',L'6',L'7',L'8',L'9',
        L'a',L'b',L'c',L'd',L'e',
        L'f',L'g',L'h',L'i',L'j',
        L'k',L'l',L'm',L'n',L'o',
        L'p',L'q',L'r',L's',L't',
        L'u',L'v',L'w',L'x',L'y',L'z'
    };

    int Number::digit(wchar_t code, int radix) {
        if (code < 128) {
            int result = -1;
            if (L'0' <= code && code <= L'9') {
                result = code - L'0';
            }
            else if (L'a' <= code && code <= L'z') {
                result = 10 + (code - L'a');
            }
            else if (L'A' <= code && code <= L'Z') {
                result = 10 + (code - L'A');
            }

            return result < radix ? result : -1;
        }

        return -1;
    }

    wchar_t Number::character(int number) {
        if (number >= 0 && number <= 9) {
            return number + L'0';
        }

        return -1;
    }


    int Number::parseInt(const string8& s, int radix) {
        long result = std::strtol(s.c_str(), nullptr, radix);
        if (errno == ERANGE) {
            errno = 0;
            return 0;
        }

        if (result > std::numeric_limits<int>::max() ||
            result < std::numeric_limits<int>::min())
        {
            return 0;
        }

        return static_cast<int>(result);
    }

    int Number::parseInt(const string16& s, int radix) {
        long result = std::wcstol(s.c_str(), nullptr, radix);
        if (errno == ERANGE) {
            errno = 0;
            return 0;
        }

        if (result > std::numeric_limits<int>::max() ||
            result < std::numeric_limits<int>::min())
        {
            return 0;
        }

        return static_cast<int>(result);
    }

}