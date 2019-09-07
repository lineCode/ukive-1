#include "shell/test/utils/big_integer_unit_test.h"

#include "ukive/log.h"
#include "ukive/utils/big_integer/big_integer.h"
#include "ukive/utils/string_utils.h"

#define BigInteger ukive::BigInteger


namespace {

    bool testToInt64(int64_t left) {
        auto test = BigInteger::from64(left);
        return test.toInt64() == left;
    }

    bool testAdd(int64_t left, int64_t right) {
        auto test = BigInteger::from64(left);
        test.add(BigInteger::from64(right));
        return test.toInt64() == left + right;
    }

    bool testSub(int64_t left, int64_t right) {
        auto test = BigInteger::from64(left);
        test.sub(BigInteger::from64(right));
        return test.toInt64() == left - right;
    }

    bool testMul(int32_t left, int32_t right) {
        auto test = BigInteger::from32(left);
        test.mul(BigInteger::from32(right));
        return test.toInt64() == int64_t(left) * int64_t(right);
    }

    bool testDiv(int64_t left, int64_t right) {
        auto test = BigInteger::from64(left);
        test.div(BigInteger::from64(right));
        return test.toInt64() == left / right;
    }

    bool testDivOverflow() {
        auto test = BigInteger::from64(INT64_MIN);
        test.div(BigInteger::from32(-1));
        return test.isBeyondInt64();
    }

    bool testMod(int64_t left, int64_t right) {
        auto test = BigInteger::from64(left);
        test.mod(BigInteger::from64(right));
        return test.toInt64() == left % right;
    }

    bool testPow(int64_t left, int64_t exp) {
        auto test = BigInteger::from64(left);
        test.pow(BigInteger::from64(exp));
        return test.toInt64() == int64_t(std::pow(left, exp));
    }

    bool testPowMod(int64_t left, int64_t exp, int64_t rem) {
        auto test = BigInteger::from64(left);
        test.powMod(BigInteger::from64(exp), BigInteger::from64(rem));
        return test.toInt64() == int64_t(std::pow(left, exp)) % rem;
    }

    bool testAbs(int64_t left) {
        auto test = BigInteger::from64(left);
        test.abs();
        return test.toInt64() == std::abs(left);
    }

    bool testInv(int64_t left) {
        auto test = BigInteger::from64(left);
        test.inv();
        return test.toInt64() == -left;
    }

    bool testShl(int64_t left, uint32_t off) {
        auto test = BigInteger::from64(left);
        test.shl(off);
        return test.toInt64() == left << off;
    }

    bool testShr(int64_t left, uint32_t off) {
        auto test = BigInteger::from64(left);
        test.shr(off);
        return test.toInt64() == left >> off;
    }

    bool testBeyondInt64() {
        auto test = BigInteger::from64(INT64_MAX);
        if (test.isBeyondInt64()) return false;

        test.add(BigInteger::ONE);
        if (!test.isBeyondInt64()) return false;

        test = BigInteger::from64(INT64_MIN);
        if (test.isBeyondInt64()) return false;

        test.add(BigInteger::from32(-1));
        return test.isBeyondInt64();
    }

    bool testToString(int64_t left) {
        std::string result;
        auto test = BigInteger::from64(left);
        return test.toString(10, &result) && result == std::to_string(left);
    }

    bool testToStringHex(int64_t left) {
        std::string result;
        auto test = BigInteger::from64(left);
        return test.toString(16, &result) && result == ukive::toString8Hex(left);
    }

}

namespace shell {
namespace test {

    void TEST_BIG_INTEGER() {

        // To int64
        DCHECK(testToInt64(-1));
        DCHECK(testToInt64(0));
        DCHECK(testToInt64(1));
        DCHECK(testToInt64(2));
        DCHECK(testToInt64(10));
        DCHECK(testToInt64(128));
        DCHECK(testToInt64(-128));
        DCHECK(testToInt64(65536));
        DCHECK(testToInt64(-65536));
        DCHECK(testToInt64(654984684858));
        DCHECK(testToInt64(-654984684858));

        // Add
        DCHECK(testAdd(0, 0));
        DCHECK(testAdd(0, 1));
        DCHECK(testAdd(1, 1));
        DCHECK(testAdd(-1, 0));
        DCHECK(testAdd(-1, 1));
        DCHECK(testAdd(-1, -1));
        DCHECK(testAdd(127, 1));
        DCHECK(testAdd(-1, 128));
        DCHECK(testAdd(1, -128));
        DCHECK(testAdd(128, 0));
        DCHECK(testAdd(128, 128));
        DCHECK(testAdd(128, -128));
        DCHECK(testAdd(-128, 128));
        DCHECK(testAdd(-128, -128));
        DCHECK(testAdd(5649856, 345346536));
        DCHECK(testAdd(-5649856, 345346536));

        // Sub
        DCHECK(testSub(0, 0));
        DCHECK(testSub(0, 1));
        DCHECK(testSub(1, 0));
        DCHECK(testSub(1, 1));
        DCHECK(testSub(-1, 1));
        DCHECK(testSub(1, -1));
        DCHECK(testSub(-1, -1));
        DCHECK(testSub(-128, -1));
        DCHECK(testSub(-128, 1));
        DCHECK(testSub(128, 127));
        DCHECK(testSub(128, -127));
        DCHECK(testSub(128, -129));
        DCHECK(testSub(3458386, 345609497));
        DCHECK(testSub(11134583866, 345609497));

        // Mul
        DCHECK(testMul(0, 0));
        DCHECK(testMul(1, 0));
        DCHECK(testMul(-1, 0));
        DCHECK(testMul(0, 1));
        DCHECK(testMul(1, 1));
        DCHECK(testMul(-1, 1));
        DCHECK(testMul(1, -1));
        DCHECK(testMul(-1, -1));
        DCHECK(testMul(16, 16));
        DCHECK(testMul(16, 16));
        DCHECK(testMul(64, 2));
        DCHECK(testMul(64, -2));
        DCHECK(testMul(32768, 2));
        DCHECK(testMul(32768, -2));
        DCHECK(testMul(65536, 65535));
        DCHECK(testMul(4568456, 234234));
        DCHECK(testMul(-4568456, 234234));
        DCHECK(testMul(4568456, -234234));
        DCHECK(testMul(-4568456, -234234));

        // Div
        DCHECK(testDivOverflow());
        DCHECK(testDiv(0, 1));
        DCHECK(testDiv(1, 1));
        DCHECK(testDiv(1, -1));
        DCHECK(testDiv(0, 2));
        DCHECK(testDiv(1, 2));
        DCHECK(testDiv(2, 2));
        DCHECK(testDiv(-2, 2));
        DCHECK(testDiv(-2, -2));
        DCHECK(testDiv(128, 2));
        DCHECK(testDiv(256, 2));
        DCHECK(testDiv(256, 3));
        DCHECK(testDiv(1024, 2));
        DCHECK(testDiv(65536, 2));
        DCHECK(testDiv(65537, 2));
        DCHECK(testDiv(65539, 2));
        DCHECK(testDiv(65535, 65536));
        DCHECK(testDiv(65536, 65535));
        DCHECK(testDiv(65536, 65537));
        DCHECK(testDiv(65537, 65536));
        DCHECK(testDiv(1048612, 2));
        DCHECK(testDiv(int64_t(INT32_MAX) + 1, 2));
        DCHECK(testDiv(int64_t(INT32_MAX) + 1, 255));
        DCHECK(testDiv(int64_t(INT32_MAX) + 347, 2));
        DCHECK(testDiv(int64_t(INT32_MAX) + 346459, 2));
        DCHECK(testDiv(99000000, 9900));
        DCHECK(testDiv(-99000000, 9900));
        DCHECK(testDiv(1, 345885486));
        DCHECK(testDiv(5484984645, 345885486));
        DCHECK(testDiv(-5484984645, 345885486));
        DCHECK(testDiv(5484984645, -345885486));
        DCHECK(testDiv(200000000000, 20000000001));
        DCHECK(testDiv(0, -20000000001));
        DCHECK(testDiv(200000000000, -20000000001));
        DCHECK(testDiv(4534444444, 453));
        DCHECK(testDiv(INT32_MIN, -1));
        DCHECK(testDiv(INT32_MIN, 2));
        DCHECK(testDiv(INT32_MAX, 2));
        DCHECK(testDiv(int64_t(INT32_MAX) + 1, -1));
        DCHECK(testDiv(int64_t(INT32_MAX) + 1, 3));
        DCHECK(testDiv(int64_t(INT32_MAX) + 1, INT32_MAX - 1));
        DCHECK(testDiv(int64_t(INT32_MAX), 2));
        DCHECK(testDiv(94856794756945, 1));
        DCHECK(testDiv(94856794756945, 94856794756945));
        DCHECK(testDiv(94856794756944, 94856794756945));
        DCHECK(testDiv(94856794756945 / 2, 94856794756945));
        DCHECK(testDiv(119025, 346));

        // Abs
        DCHECK(testAbs(0));
        DCHECK(testAbs(1));
        DCHECK(testAbs(-1));
        DCHECK(testAbs(128));
        DCHECK(testAbs(-128));
        DCHECK(testAbs(548745845888));
        DCHECK(testAbs(-548745845888));
        DCHECK(testAbs(-65536));

        // Inv
        DCHECK(testInv(0));
        DCHECK(testInv(1));
        DCHECK(testInv(-1));
        DCHECK(testInv(128));
        DCHECK(testInv(-128));
        DCHECK(testInv(548745845888));
        DCHECK(testInv(-548745845888));
        DCHECK(testInv(-65536));

        // Mod
        DCHECK(testMod(0, 1));
        DCHECK(testMod(1, 1));
        DCHECK(testMod(1, 2));
        DCHECK(testMod(2, 2));
        DCHECK(testMod(-2, 2));
        DCHECK(testMod(2, -2));
        DCHECK(testMod(-2, -2));
        DCHECK(testMod(4, 2));
        DCHECK(testMod(8, 2));
        DCHECK(testMod(16, 2));
        DCHECK(testMod(32, 2));
        DCHECK(testMod(64, 2));
        DCHECK(testMod(128, 2));
        DCHECK(testMod(128, 128));
        DCHECK(testMod(1024, 2));
        DCHECK(testMod(65536, 128));
        DCHECK(testMod(65536, 65535));
        DCHECK(testMod(65536, 65537));
        DCHECK(testMod(99000000, 9900));
        DCHECK(testMod(200000000000, -20000000001));
        DCHECK(testMod(356987398567394, 65537));
        DCHECK(testMod(356987398567394, 2303840586945));
        DCHECK(testMod(94856794756945, 1));
        DCHECK(testMod(94856794756945, 94856794756945));
        DCHECK(testMod(94856794756944, 94856794756945));
        DCHECK(testMod(94856794756945 / 2, 94856794756945));
        DCHECK(testMod(INT32_MIN, -1));
        DCHECK(testMod(INT32_MIN, 2));
        DCHECK(testMod(INT32_MAX, 2));
        DCHECK(testMod(int64_t(INT32_MAX) + 1, -1));
        DCHECK(testMod(int64_t(INT32_MAX) + 1, 3));
        DCHECK(testMod(int64_t(INT32_MAX) + 1, INT32_MAX - 1));
        DCHECK(testMod(int64_t(INT32_MAX), 2));

        // Pow
        /*DCHECK(testPow(0, 1));
        DCHECK(testPow(1, 1));
        DCHECK(testPow(0, 10));
        DCHECK(testPow(1, 10));
        DCHECK(testPow(0, 5648576));
        DCHECK(testPow(1, 5648576));
        DCHECK(testPow(2, 1));
        DCHECK(testPow(2, 2));
        DCHECK(testPow(2, 10));
        DCHECK(testPow(2, 11));
        DCHECK(testPow(16, 2));
        DCHECK(testPow(16, 10));
        DCHECK(testPow(348937, 2));
        DCHECK(testPow(-348937, 1));
        DCHECK(testPow(-348937, 2));
        DCHECK(testPow(-34893, 3));
        DCHECK(testPow(-348, 6));
        DCHECK(testPow(2, 8));
        DCHECK(testPow(2, 10));
        DCHECK(testPow(2, 15));
        DCHECK(testPow(2, 16));
        DCHECK(testPow(2, 20));
        DCHECK(testPow(2, 31));
        DCHECK(testPow(2, 32));
        DCHECK(testPow(2, 62));*/

        // Pow and Mod
        DCHECK(testPowMod(0, 1, 1));
        DCHECK(testPowMod(0, 1, 2));
        DCHECK(testPowMod(1, 1, 2));
        DCHECK(testPowMod(-1, 1, 2));
        DCHECK(testPowMod(-1, 1, 1));
        DCHECK(testPowMod(2, 1, 3));
        DCHECK(testPowMod(2, 2, 5));
        DCHECK(testPowMod(2, 3, 7));
        DCHECK(testPowMod(2, 4, 9));
        DCHECK(testPowMod(2, 5, 11));
        DCHECK(testPowMod(2, 6, 13));
        DCHECK(testPowMod(2, 7, 15));
        DCHECK(testPowMod(2, 8, 17));
        DCHECK(testPowMod(2, 9, 19));
        DCHECK(testPowMod(2, 10, 21));
        DCHECK(testPowMod(2, 11, 23));
        DCHECK(testPowMod(2, 12, 25));
        DCHECK(testPowMod(2, 13, 27));
        DCHECK(testPowMod(2, 14, 29));
        DCHECK(testPowMod(2, 15, 31));
        DCHECK(testPowMod(2, 16, 33));
        DCHECK(testPowMod(2, 17, 35));
        DCHECK(testPowMod(2, 31, 63));
        DCHECK(testPowMod(2, 63, 64));
        DCHECK(testPowMod(345, 5, 346));
        DCHECK(testPowMod(345, 5, 344));
        DCHECK(testPowMod(3450, 2, 3451));
        DCHECK(testPowMod(3451, 2, 3452));

        // Shl
        /*DCHECK(testShl(0, 0));
        DCHECK(testShl(0, 1));
        DCHECK(testShl(0, 10));
        DCHECK(testShl(0, 100));
        DCHECK(testShl(1, 0));
        DCHECK(testShl(1, 1));
        DCHECK(testShl(1, 2));
        DCHECK(testShl(1, 10));
        DCHECK(testShl(1, 16));
        DCHECK(testShl(3, 4));
        DCHECK(testShl(3, 7));
        DCHECK(testShl(65536, 5));
        DCHECK(testShl(34537485, 3));*/

        // Shr
        /*DCHECK(testShr(0, 0));
        DCHECK(testShr(0, 1));
        DCHECK(testShr(0, 10));
        DCHECK(testShr(0, 100));
        DCHECK(testShr(1, 0));
        DCHECK(testShr(1, 1));
        DCHECK(testShr(1, 2));
        DCHECK(testShr(1, 10));
        DCHECK(testShr(1, 16));
        DCHECK(testShr(3, 4));
        DCHECK(testShr(3, 7));
        DCHECK(testShr(65536, 5));
        DCHECK(testShr(34537485, 3));*/

        // Beyond int64
        DCHECK(testBeyondInt64());

        // To string
        DCHECK(testToString(0));
        DCHECK(testToString(1));
        DCHECK(testToString(-1));
        DCHECK(testToString(2));
        DCHECK(testToString(10));
        DCHECK(testToString(65535));
        DCHECK(testToString(65536));
        DCHECK(testToString(-65535));
        DCHECK(testToString(-65536));
        DCHECK(testToString(100000));
        DCHECK(testToString(-100000));
        DCHECK(testToString(INT64_MIN));
        DCHECK(testToString(INT64_MAX));

        // To string hex
        DCHECK(testToStringHex(0));
        DCHECK(testToStringHex(1));
        //DCHECK(testToStringHex(-1));
        DCHECK(testToStringHex(2));
        DCHECK(testToStringHex(10));
        DCHECK(testToStringHex(65535));
        DCHECK(testToStringHex(65536));
        //DCHECK(testToStringHex(-65535));
        //DCHECK(testToStringHex(-65536));
        DCHECK(testToStringHex(100000));
        //DCHECK(testToStringHex(-100000));
        //DCHECK(testToStringHex(INT64_MIN));
        DCHECK(testToStringHex(INT64_MAX));

        /*auto i = BigInteger::ONE;
        for (auto z = BigInteger::TWO; z.compare(BigInteger::fromU64(99999)) != 0;) {
            i.mul(z);
            z.add(BigInteger::ONE);
        }*/

        /*string8 out;
        if (i.toString(10, &out)) {
            LOG(Log::INFO) << ukive::UTF8ToUTF16(out);
        }*/

        {
            auto init = BigInteger::TWO;
            init.pow(1023);
            if (!init.isOdd()) {
                init.add(BigInteger::ONE);
            }

            int i = 0;
            while (!init.isPrime2(BigInteger::TWO)) {
                init.add(BigInteger::TWO);
                ++i;
            }

            //      Debug   Release
            // 28:  12.7s   1.6s
            // 30:  5.4s    2.0s
            LOG(Log::INFO) << "Prime retry: " << i;
        }
    }

}
}
