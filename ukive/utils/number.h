#ifndef UKIVE_UTILS_NUMBER_H_
#define UKIVE_UTILS_NUMBER_H_

#include <limits>
#include <cstdint>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Number {
    public:
        static const int MIN_RADIX = 2;
        static const int MAX_RADIX = 36;

        static int digit(wchar_t code, int radix);
        static wchar_t character(int number);

        static int32_t parseInt(string16 s, int radix);
        static uint32_t parseUInt(string16 s, int radix);
        static int64_t parseInt64(string16 s, int radix);
        static uint64_t parseUInt64(string16 s, int radix);

        static string16 toString(int32_t value);
        static string16 toString(uint32_t value);
        static string16 toString(int64_t value);
        static string16 toString(uint64_t value);

    private:
        Number();

        static wchar_t mDigitc[];
    };
}

#endif  // UKIVE_UTILS_NUMBER_H_