#ifndef UKIVE_UTILS_BIG_INTEGER8_H_
#define UKIVE_UTILS_BIG_INTEGER8_H_

#include <cstdint>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class BigInteger8 {
    public:
        BigInteger8();

        static BigInteger8 fromVal(int64_t val);

        BigInteger8& add(const BigInteger8& rhs);
        BigInteger8& sub(const BigInteger8& rhs);
        BigInteger8& mul(const BigInteger8& rhs);
        BigInteger8& div(const BigInteger8& rhs, BigInteger8* rem = nullptr);
        BigInteger8& mod(const BigInteger8& rhs);
        BigInteger8& pow(const BigInteger8& exp);
        BigInteger8& powMod(const BigInteger8& exp, const BigInteger8& rem);

        BigInteger8& add(int64_t val) { return add(fromVal(val)); }
        BigInteger8& sub(int64_t val) { return sub(fromVal(val)); }
        BigInteger8& mul(int64_t val) { return mul(fromVal(val)); }
        BigInteger8& div(int64_t val, int64_t* rem = nullptr) {
            if (rem) {
                BigInteger8 tmp;
                auto& result = div(fromVal(val), &tmp);
                *rem = tmp.toInt64();
                return result;
            }
            return div(fromVal(val));
        }
        BigInteger8& mod(int64_t val) { return mod(fromVal(val)); }
        BigInteger8& pow(uint64_t exp) { return pow(fromVal(exp)); }
        BigInteger8& powMod(int64_t exp, int64_t rem) { return powMod(fromVal(exp), fromVal(rem)); }

        BigInteger8& abs();
        BigInteger8& inv();

        BigInteger8 operator+(const BigInteger8& rhs) const;
        BigInteger8 operator-(const BigInteger8& rhs) const;
        BigInteger8 operator*(const BigInteger8& rhs) const;
        BigInteger8 operator/(const BigInteger8& rhs) const;
        BigInteger8 operator%(const BigInteger8& rhs) const;

        BigInteger8 operator+(int64_t val) const { return operator+(fromVal(val)); }
        BigInteger8 operator-(int64_t val) const { return operator-(fromVal(val)); }
        BigInteger8 operator*(int64_t val) const { return operator*(fromVal(val)); }
        BigInteger8 operator/(int64_t val) const { return operator/(fromVal(val)); }
        BigInteger8 operator%(int64_t val) const { return operator%(fromVal(val)); }

        bool operator>(const BigInteger8& rhs) const;
        bool operator>=(const BigInteger8& rhs) const;
        bool operator<(const BigInteger8& rhs) const;
        bool operator<=(const BigInteger8& rhs) const;
        bool operator==(const BigInteger8& rhs) const;

        int compare(const BigInteger8& rhs) const;

        bool isNaN() const;
        bool isEven() const;
        bool isZero() const;
        bool isMinus() const;
        bool isBeyondInt64() const;

        int64_t toInt64() const;
        string8 toString() const;
        string8 toStringHex() const;

    private:
        friend class BigIntegerConverter;

        // ÎÞ·ûºÅÔËËã
        static int compareUs(const BigInteger8& left, const BigInteger8& right);
        static BigInteger8 subUs(const BigInteger8& left, const BigInteger8& right);
        static BigInteger8 mulUs(const BigInteger8& left, const BigInteger8& right);

        void shrink();
        void elimZero(bool all);
        void divModInternal(const BigInteger8& rhs, bool is_mod, BigInteger8* rem = nullptr);

        BigInteger8 powRecur(const BigInteger8& exp);
        BigInteger8 powModRecur(const BigInteger8& exp, const BigInteger8& rem);

        uint8_t get(size_t idx) const;
        uint8_t getInv(size_t idx) const;
        uint8_t getWithExt(size_t idx) const;
        uint8_t getWithExt(size_t idx, bool minus) const;
        uint8_t getInvWithExt(size_t idx) const;
        uint8_t getInvWithExt(size_t idx, bool minus) const;
        uint8_t getBackNZ() const;
        uint16_t getBackNZ2() const;

        static uint8_t toUInt8AddOver(uint16_t val, uint8_t* over);
        static uint8_t toUInt8MulOver(uint16_t val, uint8_t* over);

        bool is_nan_;
        std::vector<uint8_t> raw_;
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER8_H_