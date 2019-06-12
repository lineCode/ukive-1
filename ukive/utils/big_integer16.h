#ifndef UKIVE_UTILS_BIG_INTEGER16_H_
#define UKIVE_UTILS_BIG_INTEGER16_H_

#include <cstdint>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class BigInteger16 {
    public:
        static const BigInteger16 ZERO;
        static const BigInteger16 ONE;
        static const BigInteger16 TWO;

        static BigInteger16 from16(int16_t val);
        static BigInteger16 fromU16(uint16_t val);
        static BigInteger16 from32(int32_t val);
        static BigInteger16 fromU32(uint32_t val);
        static BigInteger16 from64(int64_t val);
        static BigInteger16 fromU64(uint64_t val);

        BigInteger16();

        BigInteger16& add(const BigInteger16& rhs);
        BigInteger16& sub(const BigInteger16& rhs);
        BigInteger16& mul(const BigInteger16& rhs);
        BigInteger16& div(const BigInteger16& rhs, BigInteger16* rem = nullptr);
        BigInteger16& mod(const BigInteger16& rhs);
        BigInteger16& pow(const BigInteger16& exp);
        BigInteger16& powMod(const BigInteger16& exp, const BigInteger16& rem);
        BigInteger16& shl(uint32_t off);
        BigInteger16& shr(uint32_t off);
        BigInteger16& abs();
        BigInteger16& inv();

        BigInteger16 operator+(const BigInteger16& rhs) const;
        BigInteger16 operator-(const BigInteger16& rhs) const;
        BigInteger16 operator*(const BigInteger16& rhs) const;
        BigInteger16 operator/(const BigInteger16& rhs) const;
        BigInteger16 operator%(const BigInteger16& rhs) const;

        bool operator>(const BigInteger16& rhs) const;
        bool operator>=(const BigInteger16& rhs) const;
        bool operator<(const BigInteger16& rhs) const;
        bool operator<=(const BigInteger16& rhs) const;
        bool operator==(const BigInteger16& rhs) const;

        int compare(const BigInteger16& rhs) const;

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
        void addUs(BigInteger16& left, const BigInteger16& right) const;
        void subUsTL(BigInteger16& left, const BigInteger16& right) const;
        void subUsTR(const BigInteger16& left, BigInteger16& right) const;
        void mulUs(BigInteger16& left, const BigInteger16& right) const;
        BigInteger16 mulUs(const BigInteger16& left, const BigInteger16& right) const;
        void divModUs(BigInteger16& left, const BigInteger16& rhs, bool is_mod, BigInteger16* rem = nullptr) const;
        int compareUs(const BigInteger16& left, const BigInteger16& right) const;

        void shrink(bool all = false);

        BigInteger16 powRecur(const BigInteger16& exp);
        BigInteger16 powModRecur(const BigInteger16& exp, const BigInteger16& rem);

        uint16_t get(size_t idx) const;
        uint16_t getWithExt(size_t idx) const;
        uint32_t getBack2() const;
        uint64_t getBack3() const;

        static uint16_t toUInt16AddOver(uint32_t val, uint8_t* over);
        static uint16_t toUInt16MulOver(uint32_t val, uint16_t* over);

        bool is_nan_;
        bool is_minus_;
        std::vector<uint16_t> raw_;
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER16_H_