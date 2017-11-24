#include "double.h"

#include <sstream>

#include "ukive/log.h"


namespace ukive {

    Double::Double(double value) {
        du_.raw_value = value;
    }

    Double::~Double() {
    }


    bool Double::isNaN() {
        return ((du_.bits & kBaseMask) != 0Ui64
            && (du_.bits & kExpoMask) == kExpoMask);
    }

    bool Double::isInfinity() {
        return ((du_.bits & kBaseMask) == 0Ui64
            && (du_.bits & kExpoMask) == kExpoMask);
    }

    bool Double::isNegative() {
        return (du_.bits & kSignMask) != 0Ui64;
    }


    uint64_t Double::exponent() {
        return du_.bits & kExpoMask;
    }

    uint64_t Double::mantissa() {
        return du_.bits & kBaseMask;
    }

    int Double::compareTo(const Double& f) {
        std::uint64_t lhs = du_.bits;
        std::uint64_t rhs = f.du_.bits;

        if (lhs & kSignMask) {
            lhs = ~lhs + 1Ui64;
        }
        else {
            lhs |= kSignMask;
        }

        if (rhs & kSignMask) {
            rhs = ~rhs + 1Ui64;
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

    string16 Double::toString() {
        std::wstringstream ss;
        ss << du_.raw_value;

        return ss.str();
    }

    string16 Double::toString(int decimalCount) {
        std::wstringstream ss;
        ss << du_.raw_value;

        return ss.str();
    }

    int Double::compare(double lhs, double rhs) {
        return Double(lhs).compareTo(Double(rhs));
    }

    float Double::parseDouble(string16 s) {
        if (s.empty()) {
            Log::e(L"invalid params");
            return 0;
        }

        return 0;
    }

    string16 Double::toString(double value) {
        return Double(value).toString();
    }

}