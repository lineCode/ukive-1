#ifndef UKIVE_UTILS_BIG_INTEGER32_H_
#define UKIVE_UTILS_BIG_INTEGER32_H_

#include <cstdint>
#include <vector>

#include "ukive/utils/string_utils.h"
#include "ukive/utils/big_integer16.h"


namespace ukive {

    class BigInteger32 {
    public:
        BigInteger32();

        static BigInteger32 fromVal(int64_t val);

        BigInteger32& add(const BigInteger32& rhs);
        BigInteger32& sub(const BigInteger32& rhs);
        BigInteger32& mul(const BigInteger32& rhs);
        BigInteger32& div(const BigInteger32& rhs, BigInteger32* rem = nullptr);
        BigInteger32& mod(const BigInteger32& rhs);
        BigInteger32& pow(const BigInteger32& exp);
        BigInteger32& powMod(const BigInteger32& exp, const BigInteger32& rem);

        BigInteger32& add(int64_t val) { return add(fromVal(val)); }
        BigInteger32& sub(int64_t val) { return sub(fromVal(val)); }
        BigInteger32& mul(int64_t val) { return mul(fromVal(val)); }
        BigInteger32& div(int64_t val, int64_t* rem = nullptr) {
            if (rem) {
                BigInteger32 tmp;
                auto& result = div(fromVal(val), &tmp);
                *rem = tmp.toInt64();
                return result;
            }
            return div(fromVal(val));
        }
        BigInteger32& mod(int64_t val) { return mod(fromVal(val)); }
        BigInteger32& pow(uint64_t exp) { return pow(fromVal(exp)); }
        BigInteger32& powMod(int64_t exp, int64_t rem) { return powMod(fromVal(exp), fromVal(rem)); }

        BigInteger32& abs();
        BigInteger32& inv();

        BigInteger32 operator+(const BigInteger32& rhs) const;
        BigInteger32 operator-(const BigInteger32& rhs) const;
        BigInteger32 operator*(const BigInteger32& rhs) const;
        BigInteger32 operator/(const BigInteger32& rhs) const;
        BigInteger32 operator%(const BigInteger32& rhs) const;

        BigInteger32 operator+(int64_t val) const { return operator+(fromVal(val)); }
        BigInteger32 operator-(int64_t val) const { return operator-(fromVal(val)); }
        BigInteger32 operator*(int64_t val) const { return operator*(fromVal(val)); }
        BigInteger32 operator/(int64_t val) const { return operator/(fromVal(val)); }
        BigInteger32 operator%(int64_t val) const { return operator%(fromVal(val)); }

        bool operator>(const BigInteger32& rhs) const;
        bool operator>=(const BigInteger32& rhs) const;
        bool operator<(const BigInteger32& rhs) const;
        bool operator<=(const BigInteger32& rhs) const;
        bool operator==(const BigInteger32& rhs) const;

        int compare(const BigInteger32& rhs) const;

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
        void addUs(BigInteger32& left, const BigInteger32& right) const;
        void subUsTL(BigInteger32& left, const BigInteger32& right) const;
        void subUsTR(const BigInteger32& left, BigInteger32& right) const;
        void mulUs(BigInteger32& left, const BigInteger32& right) const;
        BigInteger32 mulUs(const BigInteger32& left, const BigInteger32& right) const;
        void divModUs(BigInteger32& left, const BigInteger32& rhs, bool is_mod, BigInteger32* rem = nullptr) const;
        int compareUs(const BigInteger32& left, const BigInteger32& right) const;

        void shrink(bool all = false);

        BigInteger32 powRecur(const BigInteger32& exp);
        BigInteger32 powModRecur(const BigInteger32& exp, const BigInteger32& rem);

        uint32_t get(size_t idx) const;
        uint32_t getWithExt(size_t idx) const;
        uint64_t getBack2() const;
        BigInteger16 getBack3() const;

        static uint32_t toUInt32AddOver(uint64_t val, uint8_t* over);
        static uint32_t toUInt32MulOver(uint64_t val, uint32_t* over);

        bool is_nan_;
        bool is_minus_;
        std::vector<uint32_t> raw_;
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER32_H_