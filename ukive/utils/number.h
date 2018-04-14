#ifndef UKIVE_UTILS_NUMBER_H_
#define UKIVE_UTILS_NUMBER_H_

#include <limits>
#include <cstdint>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Number {
    public:
        static int digit(wchar_t code, int radix);
        static wchar_t character(int number);

        static int parseInt(const string8& s, int radix = 10);
        static int parseInt(const string16& s, int radix = 10);

    private:
        Number();

        static wchar_t mDigitc[];
    };
}

#endif  // UKIVE_UTILS_NUMBER_H_