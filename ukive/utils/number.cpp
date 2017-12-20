#include "number.h"

#include "ukive/log.h"


namespace ukive {

    wchar_t Number::mDigitc[] = {
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


    int32_t Number::parseInt(string16 s, int radix) {
        if (s.empty()) {
            Log::e(L"null");
            return 0;
        }

        if (radix < MIN_RADIX || radix > MAX_RADIX) {
            Log::e(L"illegal radix.");
            return 0;
        }

        int32_t result = 0;
        int32_t digit;
        int32_t multmin;
        size_t i = 0, len = s.length();
        int32_t limit = -std::numeric_limits<int32_t>::max();
        bool negative = false;

        wchar_t firstChar = s.at(0);
        if (firstChar < L'0') {
            if (firstChar == L'-') {
                negative = true;
                limit = std::numeric_limits<int32_t>::min();
            }
            else if (firstChar != L'+') {
                Log::e(L"illegal number str");
                return 0;
            }

            i++;
        }

        multmin = limit / radix;

        while (i < len) {
            digit = Number::digit(s.at(i++), radix);
            if (digit < 0) {
                Log::e(L"illegal number str");
                return 0;
            }

            if (result < multmin) {
                Log::e(L"illegal number str");
                return 0;
            }

            result *= radix;
            if (result < limit + digit) {
                Log::e(L"illegal number str");
                return 0;
            }

            result -= digit;
        }

        return negative ? result : -result;
    }

    int64_t Number::parseInt64(string16 s, int radix) {
        if (s.empty()) {
            Log::e(L"null");
            return 0;
        }

        if (radix < MIN_RADIX || radix > MAX_RADIX) {
            Log::e(L"radix is illegal.");
            return 0;
        }

        int32_t digit;
        size_t i = 0, len = s.length();
        int64_t multmin;
        int64_t result = 0;
        int64_t limit = -(std::numeric_limits<int64_t>::max)();
        bool negative = false;

        wchar_t firstChar = s.at(0);
        if (firstChar < L'0') {
            if (firstChar == L'-') {
                negative = true;
                limit = (std::numeric_limits<int64_t>::min)();
            }
            else if (firstChar != L'+') {
                Log::e(L"illegal number str");
                return 0;
            }

            i++;
        }
        multmin = limit / radix;

        while (i < len) {
            digit = Number::digit(s.at(i++), radix);
            if (digit < 0) {
                Log::e(L"illegal number str");
                return 0;
            }

            if (result < multmin) {
                Log::e(L"illegal number str");
                return 0;
            }

            result *= radix;
            if (result < limit + digit) {
                Log::e(L"illegal number str");
                return 0;
            }

            result -= digit;
        }

        return negative ? result : -result;
    }

}