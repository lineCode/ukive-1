#ifndef UKIVE_SECURITY_CRYPTO_AEAD_HPP_
#define UKIVE_SECURITY_CRYPTO_AEAD_HPP_

#include "utils/string_utils.h"


namespace ukive {
namespace crypto {

    // NIST Special Publication 800-38D
    // Based on AES.
    class GCM {
    public:
        static const uint64_t kLenPMin = 0;
        static const uint64_t kLenPMax = (uint64_t(1) << 39) - 256;
        static const uint64_t kLenAMin = 0;
        static const uint64_t kLenAMax = -1;
        static const uint64_t kLenIVMin = 1;
        static const uint64_t kLenIVMax = -1;

        static void product(const stringu8& X, const stringu8& Y, stringu8* r);
        static void GHASH(const stringu8& H, const stringu8& X, stringu8* r);
        static void GCTR(const stringu8& K, const stringu8& ICB, const stringu8& X, stringu8* r);
        static void CIPH(const stringu8& K, const stringu8& CB, stringu8* r);

        static void GCM_AE(
            const stringu8& K, const stringu8& IV, const stringu8& P, const stringu8& A, int t,
            stringu8* C, stringu8* T);

        static bool GCM_AD(
            const stringu8& K, const stringu8& IV, const stringu8& C, const stringu8& A, int t,
            const stringu8& T, stringu8* P);
    };

}
}

#endif  // UKIVE_SECURITY_CRYPTO_AEAD_HPP_