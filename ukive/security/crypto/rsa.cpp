#include "ukive/security/crypto/rsa.h"

#include "ukive/log.h"


namespace ukive {
namespace crypto {

    void RSA::init() {
        auto p = getPrime();
        auto q = getPrime();
        auto n = p * q;
        auto fn = p.sub(BigInteger::ONE) * q.sub(BigInteger::ONE);

        // 确定公钥指数 e
        auto e = BigInteger::fromRandom(BigInteger::TWO, fn - BigInteger::ONE);
        while (!(e.gcd(fn) == BigInteger::ONE)) {
            e = BigInteger::fromRandom(BigInteger::TWO, fn - BigInteger::ONE);
        }

        // 确定私钥指数 d
        auto d = e.invmod(fn);

        p.destroy();
        q.destroy();

        int M = 2233;
        auto C = BigInteger::fromU32(M).powMod(e, n);
        auto M1 = C.powMod(d, n);

        int i = 0;
    }

    BigInteger RSA::getPrime() {
        auto init = BigInteger::fromRandom(1024);
        if (!init.isOdd()) {
            init.add(BigInteger::ONE);
        }

        while (!isPrime(init)) {
            init.add(BigInteger::TWO);
        }
        return init;
    }

    bool RSA::isPrime(const BigInteger& bi) {
        return bi.isPrime2(BigInteger::TWO) &&
            bi.isPrime2(BigInteger::fromU32(3));
    }

}
}