#ifndef UKIVE_UTILS_FLOAT_H_
#define UKIVE_UTILS_FLOAT_H_

#include <limits>
#include <cstdint>

#include "ukive/utils/string_utils.h"


namespace ukive {

    ///<summary>
    ///根据IEEE 754标准实现的有关float的相关方法。
    ///</summary>
    class Float {
    public:
        explicit Float(float value);
        ~Float();

        bool isNaN();
        bool isInfinity();
        bool isNegative();

        uint32_t exponent();
        uint32_t mantissa();

        int compareTo(const Float& f);

        string16 toString();
        string16 toString(int decimalCount);

        static int compare(float lhs, float rhs);
        static float parseFloat(string16 s);
        static string16 toString(float value);
        static string16 toString(float value, int decimalCount);

    private:
        union FloatUnion {
            float raw_value;
            uint32_t bits;
        };

        static const uint32_t kULPs = 4U;
        static const uint32_t kSignMask = 1U << 31U;
        static const uint32_t kBaseMask = 0x007FFFFFU;
        static const uint32_t kExpoMask = ~(kSignMask | kBaseMask);

    private:
        FloatUnion fu_;
    };

}

#endif  // UKIVE_UTILS_FLOAT_H_