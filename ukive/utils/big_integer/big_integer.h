#ifndef UKIVE_UTILS_BIG_INTEGER_BIG_INTEGER_H_
#define UKIVE_UTILS_BIG_INTEGER_BIG_INTEGER_H_

// 按照图书：BigNum Math: Implementing Cryptographic Multiple Precision Arithmetic
// 中的代码编写而成，书中代码来自 LibTomMath 库

#include "ukive/utils/big_integer/int_array.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class BigInteger {
    public:
        using Digit = IntArray::Digit;
        using Word = IntArray::Word;

        // Digit 的可用二进制位数
        const static Digit kDigitBitCount = 32;

        // Word 的可用二进制位数
        const static Digit kWordBitCount = 64;

        // kBase 的以2为底的指数
        const static Digit kBaseBitCount = 28;

        // 采用的基（即进制数），必须是2的幂
        const static Digit kBase = 1 << kBaseBitCount;

        // 用于将 (n % kBase) 转换为 (n & kBaseMask)
        const static Digit kBaseMask = kBase - 1;

        const static int kDelta = 1 << (kWordBitCount - 2 * kBaseBitCount);

        static const BigInteger ZERO;
        static const BigInteger ONE;
        static const BigInteger TWO;

        static BigInteger from32(int32_t i);
        static BigInteger from64(int64_t i);
        static BigInteger fromU32(uint32_t i);
        static BigInteger fromU64(uint64_t i);
        static BigInteger fromRandom(uint32_t bit_count);
        static BigInteger fromRandom(const BigInteger& min, const BigInteger& max);

        BigInteger();

        void destroy();

        void setInt32(int32_t i);
        void setInt64(int64_t i);
        void setUInt32(uint32_t i);
        void setUInt64(uint64_t i);

        BigInteger& add(const BigInteger& rhs);
        BigInteger& sub(const BigInteger& rhs);
        BigInteger& mul(const BigInteger& rhs);
        BigInteger& div(const BigInteger& rhs);
        BigInteger& mod(const BigInteger& rhs);
        BigInteger& pow(Digit exp);
        void pow(const BigInteger& exp);
        BigInteger& powMod(const BigInteger& exp, const BigInteger& m);
        BigInteger& abs();
        BigInteger& inv();
        BigInteger& shl(int offset);
        BigInteger& shr(int offset);

        BigInteger& mul2();
        BigInteger& mul2exp(int exp);
        BigInteger& div2();
        BigInteger& div2exp(int exp);
        BigInteger& mod2exp(int exp);
        BigInteger& exp2();
        BigInteger& zweiExp(Digit exp);

        BigInteger gcd(const BigInteger& rhs) const;
        BigInteger lcm(const BigInteger& rhs) const;
        // {out} = 1/{this} mod {rhs}
        // {out}*{this} = 1 (mod{rhs})
        BigInteger invmod(const BigInteger& rhs) const;

        int compare(const BigInteger& rhs) const;

        BigInteger operator+(const BigInteger& rhs) const;
        BigInteger operator-(const BigInteger& rhs) const;
        BigInteger operator*(const BigInteger& rhs) const;
        BigInteger operator/(const BigInteger& rhs) const;
        BigInteger operator%(const BigInteger& rhs) const;

        bool operator>(const BigInteger& rhs) const;
        bool operator>=(const BigInteger& rhs) const;
        bool operator<(const BigInteger& rhs) const;
        bool operator<=(const BigInteger& rhs) const;
        bool operator==(const BigInteger& rhs) const;

        int64_t toInt64() const;
        uint64_t toUInt64() const;
        bool toString(int radix, string8* str) const;

        bool isOdd() const;
        bool isZero() const;
        bool isBeyondInt64() const;
        bool isBeyondUInt64() const;

        // b > 1
        bool isPrime(const BigInteger& b) const;
        // b > 1
        bool isPrime2(const BigInteger& b) const;

    private:
        static void setDigit(IntArray* a, Digit d);
        static int getBitCount(const IntArray& a);
        static int getLSBZeroCount(const IntArray& a);

        static void lowAdd(const IntArray& l, const IntArray& r, IntArray* result);
        static void lowSub(const IntArray& l, const IntArray& r, IntArray* result);
        static void lowMulDigs(const IntArray& l, const IntArray& r, int digs, IntArray* result);
        static void lowMulHighDigs(const IntArray& l, const IntArray& r, int digs, IntArray* result);
        static void lowFastMulDigs(const IntArray& l, const IntArray& r, int digs, IntArray* result);
        static void lowSqr(const IntArray& a, IntArray* result);
        static void lowFastSqr(const IntArray& a, IntArray* result);
        static void lowExptmod(const IntArray& g, const IntArray& x, const IntArray& p, IntArray* y);
        static void montgomeryCalNorm(IntArray* a, const IntArray& b);
        static void lowFastExptmod(const IntArray& g, const IntArray& x, const IntArray& p, IntArray* y);

        static void karatsubaMul(const IntArray& l, const IntArray& r, IntArray* result);
        static void toomMul(const IntArray& l, const IntArray& r, IntArray* result);

        static void karatsubaSqr(const IntArray& a, IntArray* result);
        static void toomSqr(const IntArray& a, IntArray* result);

        static int cmpUnsItl(const IntArray& l, const IntArray& r);
        static int cmpItl(const IntArray& l, const IntArray& r);

        static void addItl(const IntArray& l, const IntArray& r, IntArray* result);
        static void subItl(const IntArray& l, const IntArray& r, IntArray* result);
        static void mul2Itl(const IntArray& l, IntArray* result);
        static void div2Itl(const IntArray& l, IntArray* result);
        static void shlItl(int offset, IntArray* result);
        static void shrItl(int offset, IntArray* result);
        static void mul2dItl(const IntArray& l, int exp, IntArray* result);
        static void div2dItl(const IntArray& l, int exp, IntArray* result, IntArray* rem);
        static void mod2dItl(const IntArray& l, int exp, IntArray* result);
        static void mulItl(const IntArray& l, const IntArray& r, IntArray* result);
        static void sqrItl(const IntArray& a, IntArray* result);
        static void exptdItl(const IntArray& a, Digit b, IntArray* result);
        static void zweiExptItl(Digit b, IntArray* result);
        static void exptmodItl(const IntArray& g, const IntArray& x, const IntArray& p, IntArray* y);
        static void divItl(const IntArray& a, const IntArray& b, IntArray* c, IntArray* d);
        static void modItl(const IntArray& a, const IntArray& b, IntArray* c);

        static int cmpdItl(const IntArray& l, Digit r);
        static void adddItl(const IntArray& a, Digit b, IntArray* c);
        static void subdItl(const IntArray& a, Digit b, IntArray* c);
        static void muldItl(const IntArray& a, Digit b, IntArray* c);
        static void div3Itl(const IntArray& a, IntArray* b, Digit* c);
        static void divdItl(const IntArray& a, Digit b, IntArray* c, Digit* d);
        static bool rootdItl(IntArray& a, Digit b, IntArray* c);

        // 0 <= x <= m^2, m > 1
        static void reduce(IntArray* x, const IntArray& m, const IntArray& mu);
        static void reduceSetup(const IntArray& b, IntArray* mu);

        // 0 <= x <= n^2, n > 1
        static void montgomeryReduce(IntArray* x, const IntArray& n, Digit rho);
        inline static void fastMontgomeryReduce(IntArray* x, const IntArray& n, Digit rho);
        static void montgomerySetup(const IntArray& n, Digit* rho);

        static bool invmodFast(const IntArray& a, const IntArray& b, IntArray* c);
        static bool invmodSlow(const IntArray& a, const IntArray& b, IntArray* c);

        static bool readFromStringItl(const string8& str, int radix, IntArray* a);
        static bool toStringItl(const IntArray& a, int radix, string8* str);

        static void gcdItl(const IntArray& a, const IntArray& b, IntArray* c);
        static void lcmItl(const IntArray& a, const IntArray& b, IntArray* c);

        // c = 1/a mod b
        static bool invmodItl(const IntArray& a, const IntArray& b, IntArray* c);

        IntArray int_;
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER_BIG_INTEGER_H_