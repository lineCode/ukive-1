#ifndef UKIVE_SECURITY_CRYPTO_ECDP_H_
#define UKIVE_SECURITY_CRYPTO_ECDP_H_

#include "ukive/utils/big_integer/big_integer.h"


// SEC 1: Elliptic Curve Cryptography
// SEC 2: Recommended Elliptic Curve Domain Parameters
// https://tools.ietf.org/html/rfc7748

namespace ukive {
namespace crypto {

    class ECDP {
    public:
        // Curve: y^2 = x^3 + ax + b
        static void secp192k1(
            BigInteger* p,
            BigInteger* a, BigInteger* b,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp192r1(
            BigInteger* p,
            BigInteger* a, BigInteger* b, BigInteger* S,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp224k1(
            BigInteger* p,
            BigInteger* a, BigInteger* b,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp224r1(
            BigInteger* p,
            BigInteger* a, BigInteger* b, BigInteger* S,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp256k1(
            BigInteger* p,
            BigInteger* a, BigInteger* b,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp256r1(
            BigInteger* p,
            BigInteger* a, BigInteger* b, BigInteger* S,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp384r1(
            BigInteger* p,
            BigInteger* a, BigInteger* b, BigInteger* S,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        static void secp521r1(
            BigInteger* p,
            BigInteger* a, BigInteger* b, BigInteger* S,
            BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h);

        // v^2 = u^3 + A*u^2 + u
        static void curve25519(
            BigInteger* p,
            uint32_t* A, BigInteger* order, uint8_t* cofactor, uint8_t* Up, BigInteger* Vp);

        static void curve448(
            BigInteger* p,
            uint32_t* A, BigInteger* order, uint8_t* cofactor, uint8_t* Up, BigInteger* Vp);

        // -x^2 + y^2 = 1 + d * x^2 * y^2
        static void edwards25519(
            BigInteger* p,
            BigInteger* d, BigInteger* order, uint8_t* cofactor, BigInteger* Xp, BigInteger* Yp);

        // x^2 + y^2 = 1 + d * x^2 * y^2
        static void edwards448_1(
            BigInteger* p,
            BigInteger* d, BigInteger* order, uint8_t* cofactor, BigInteger* Xp, BigInteger* Yp);

        static void edwards448_2(
            BigInteger* p,
            BigInteger* d, BigInteger* order, uint8_t* cofactor, BigInteger* Xp, BigInteger* Yp);


        // 对于 y^2 = x^3 + ax + b

        // (x2, y2) = (x1, y1) + (x2, y2)
        static void addPoint(
            const BigInteger& p, const BigInteger& a,
            const BigInteger& x1, const BigInteger& y1,
            BigInteger* x2, BigInteger* y2);

        // (x, y) = d * (x, y)
        static void mulPoint(
            const BigInteger& p, const BigInteger& a,
            const BigInteger& d, BigInteger* x, BigInteger* y);

        static bool verifyPoint(
            const BigInteger& p,
            const BigInteger& a, const BigInteger& b,
            const BigInteger& x, const BigInteger& y);

        static void X25519(
            const BigInteger& p, const BigInteger& k, const BigInteger& u,
            BigInteger* result);

        static void X448(
            const BigInteger& p, const BigInteger& k, const BigInteger& u,
            BigInteger* result);

    private:
        // 对于 curve25519/448
        static void X25519_448(
            const BigInteger& p, const BigInteger& k, const BigInteger& u,
            uint32_t a24, BigInteger* result);

        static void cswap(uint8_t swap, BigInteger* x2, BigInteger* x3);
    };

}
}

#endif  // UKIVE_SECURITY_CRYPTO_ECDP_H_