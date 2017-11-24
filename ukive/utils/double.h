#ifndef UKIVE_UTILS_DOUBLE_H_
#define UKIVE_UTILS_DOUBLE_H_

#include <limits>
#include <cstdint>

#include "ukive/utils/string_utils.h"


namespace ukive {

    ///<summary>
    ///����IEEE 754��׼ʵ�ֵ��й�double����ط�����
    ///</summary>
    class Double {
    public:
        explicit Double(double value);
        ~Double();

        bool isNaN();
        bool isInfinity();
        bool isNegative();

        uint64_t exponent();
        uint64_t mantissa();

        int compareTo(const Double& f);

        string16 toString();
        string16 toString(int decimalCount);

        static int compare(double lhs, double rhs);
        static float parseDouble(string16 s);
        static string16 toString(double value);

    private:
        union DoubleUnion {
            double raw_value;
            uint64_t bits;
        };

        static const uint64_t kULPs = 4Ui64;
        static const uint64_t kSignMask = 1Ui64 << 63Ui64;
        static const uint64_t kBaseMask = 0xFFFFFFFFFFFFFUi64;
        static const uint64_t kExpoMask = ~(kSignMask | kBaseMask);

    private:
        DoubleUnion du_;
    };

}

#endif  // UKIVE_UTILS_DOUBLE_H_