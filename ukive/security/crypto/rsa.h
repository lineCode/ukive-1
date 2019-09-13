#ifndef UKIVE_SECURITY_CRYPTO_RSA_H_
#define UKIVE_SECURITY_CRYPTO_RSA_H_

#include "ukive/utils/big_integer/big_integer.h"


// 根据 RFC 8017 实现的 RSA 算法。
// https://tools.ietf.org/html/rfc8017

namespace ukive {
namespace crypto {

    class RSA {
    public:
        RSA() = default;

        void init();

        static BigInteger getPrime();
        static bool isPrime(const BigInteger& bi);
    };

}
}

#endif  // UKIVE_SECURITY_CRYPTO_RSA_H_