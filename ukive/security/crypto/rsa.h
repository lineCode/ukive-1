#ifndef UKIVE_SECURITY_CRYPTO_RSA_H_
#define UKIVE_SECURITY_CRYPTO_RSA_H_

#include "ukive/utils/big_integer8.h"
#include "ukive/utils/big_integer16.h"
#include "ukive/utils/big_integer32.h"

#define BigIntegerClass BigInteger16


// ���� RFC 8017 ʵ�ֵ� RSA �㷨��
// https://tools.ietf.org/html/rfc8017

namespace ukive {
namespace crypto {

    class RSA {
    public:
        RSA() = default;

        static BigIntegerClass getPrime();
        static bool isPrime(const BigIntegerClass& bi);
    };

}
}

#endif  // UKIVE_SECURITY_CRYPTO_RSA_H_