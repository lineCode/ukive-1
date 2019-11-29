#include "ukive/security/crypto/rsa.h"

#include "utils/log.h"


namespace ukive {
namespace crypto {

    void RSA::init() {
        auto p = getPrime();
        auto q = getPrime();
        auto n = p * q;
        auto fn = p.sub(utl::BigInteger::ONE) * q.sub(utl::BigInteger::ONE);

        // 确定公钥指数 e
        auto e = utl::BigInteger::fromRandom(utl::BigInteger::TWO, fn - utl::BigInteger::ONE);
        while (!(e.gcd(fn) == utl::BigInteger::ONE)) {
            e = utl::BigInteger::fromRandom(utl::BigInteger::TWO, fn - utl::BigInteger::ONE);
        }

        // 确定私钥指数 d
        auto d = e.invmod(fn);

        p.destroy();
        q.destroy();

        int M = 2233;
        auto C = utl::BigInteger::fromU32(M).powMod(e, n);
        auto M1 = C.powMod(d, n);

        int i = 0;
    }

    utl::BigInteger RSA::getPrime() {
        auto init = utl::BigInteger::fromRandom(1024);
        if (!init.isOdd()) {
            init.add(1);
        }

        while (!isPrime(init)) {
            init.add(2);
        }
        return init;
    }

    bool RSA::isPrime(const utl::BigInteger& bi) {
        return bi.isPrime2(utl::BigInteger::TWO) &&
            bi.isPrime2(utl::BigInteger::fromU32(3));
    }

}
}