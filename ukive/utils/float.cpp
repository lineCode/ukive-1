#include "float.h"

#include "ukive/log.h"


namespace ukive {

    Float::Float(float value) {
        fu_.raw_value = value;
    }

    Float::~Float() {
    }


    bool Float::isNaN() {
        return ((fu_.bits & kBaseMask) != 0Ui32
            && (fu_.bits & kExpoMask) == kExpoMask);
    }

    bool Float::isInfinity() {
        return ((fu_.bits & kBaseMask) == 0Ui32
            && (fu_.bits & kExpoMask) == kExpoMask);
    }

    bool Float::isNegative() {
        return (fu_.bits & kSignMask) != 0Ui32;
    }

    uint32_t Float::exponent() {
        return fu_.bits & kExpoMask;
    }

    uint32_t Float::mantissa() {
        return fu_.bits & kBaseMask;
    }


    int Float::compareTo(const Float& f) {
        std::uint32_t lhs = fu_.bits;
        std::uint32_t rhs = f.fu_.bits;

        if (lhs & kSignMask) {
            lhs = ~lhs + 1Ui32;
        }
        else {
            lhs |= kSignMask;
        }

        if (rhs & kSignMask) {
            rhs = ~rhs + 1Ui32;
        }
        else {
            rhs |= kSignMask;
        }

        if (lhs >= rhs && (lhs - rhs) > kULPs) {
            return 1;
        }
        else if (lhs < rhs && (rhs - lhs) > kULPs) {
            return -1;
        }
        else {
            return 0;
        }
    }


    string16 Float::toString() {
        return std::to_wstring(fu_.raw_value);
    }

    string16 Float::toString(int decimalCount) {
        std::wstring text = std::to_wstring(fu_.raw_value);
        size_t index = text.find('.');
        if (index == std::wstring::npos) {
            return text.append(L".00");
        }
        else {
            if (index + decimalCount + 1 >= text.length()) {
                return text;
            }
            else {
                return text.substr(0, index + decimalCount + 1);
            }
        }
    }


    int Float::compare(float lhs, float rhs) {
        return Float(lhs).compareTo(Float(rhs));
    }

    float Float::parseFloat(string16 s) {
        if (s.empty()) {
            Log::e(L"invalid params");
            return 0;
        }

        return 0;
    }

    string16 Float::toString(float value) {
        return Float(value).toString();
    }

    string16 Float::toString(float value, int decimalCount) {
        return Float(value).toString(decimalCount);
    }

}