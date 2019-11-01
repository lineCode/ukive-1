#include "ukive/security/crypto/aead.hpp"

#include <assert.h>

#include "ukive/security/crypto/aes.h"
#include "ukive/utils/big_integer/byte_string.hpp"


namespace ukive {
namespace crypto {

    void GCM::product(const stringu8& X, const stringu8& Y, stringu8* r) {
        stringu8 Z(16, 0), V(Y);

        for (int i = 0; i < 128; ++i) {
            auto t = ByteString::getBit(X, i);
            if (t) {
                ByteString:: xor (Z, V, &Z);
            }
            if (V.back() & 0x01) {
                ByteString::div2(V, &V);
                V[0] ^= 0xE1;
            } else {
                ByteString::div2(V, &V);
            }
        }

        *r = std::move(Z);
    }

    void GCM::GHASH(const stringu8& H, const stringu8& X, stringu8* r) {
        int m = X.size() / 16;
        stringu8 Y(16, 0);

        for (int i = 0; i < m; ++i) {
            ByteString::xor(Y, X.substr(i * 16, 16), &Y);
            product(Y, H, &Y);
        }

        *r = std::move(Y);
    }

    void GCM::GCTR(const stringu8& K, const stringu8& ICB, const stringu8& X, stringu8* r) {
        if (X.empty()) {
            r->clear();
            return;
        }
        int n = (X.size() + 15) / 16;

        int i;
        stringu8 Y;
        stringu8 CB(ICB);
        for (i = 0; i <= n - 2; ++i) {
            stringu8 crypted;
            CIPH(K, CB, &crypted);

            stringu8 Yi(X.substr(i * 16, 16));
            ByteString::xor(Yi, crypted, &Yi);
            ByteString::inc(CB, 4, &CB);

            Y.append(Yi);
        }

        stringu8 crypted;
        CIPH(K, CB, &crypted);

        stringu8 Yi(X.substr(i * 16, 16));
        ByteString::xor(Yi, crypted.substr(0, Yi.size()), &Yi);

        Y.append(Yi);

        *r = std::move(Y);
    }

    void GCM::CIPH(const stringu8& K, const stringu8& CB, stringu8* r) {
        uint8_t buf[AES::Nb * 4];
        AES::encrypt(CB.data(), buf, K);
        *r = std::move(stringu8(std::begin(buf), std::end(buf)));
    }

    void GCM::GCM_AE(
        const stringu8& K, const stringu8& IV, const stringu8& P, const stringu8& A, int t,
        stringu8* C, stringu8* T)
    {
        assert(IV.size() >= kLenIVMin && IV.size() <= kLenIVMax);
        assert(P.size() >= kLenPMin && P.size() <= kLenPMax);
        assert(A.size() >= kLenAMin && A.size() <= kLenAMax);

        stringu8 H;
        CIPH(K, stringu8(16, 0), &H);

        stringu8 J0;
        if (IV.size() == 12) {
            J0 = IV;
            J0.append(3, 0).push_back(1);
        } else {
            int s = 16 * ((IV.size() + 15) / 16) - IV.size();
            stringu8 X(IV);
            X.append(s + 8, 0);

            stringu8 len;
            ByteString::len64(IV, &len);
            X.append(len);

            GHASH(H, X, &J0);
        }

        stringu8 J0_1(J0);
        ByteString::inc(J0_1, 4, &J0_1);
        GCTR(K, J0_1, P, C);

        int u = 16 * ((C->size() + 15) / 16) - C->size();
        int v = 16 * ((A.size() + 15) / 16) - A.size();

        stringu8 len_A, len_C;
        ByteString::len64(A, &len_A);
        ByteString::len64(*C, &len_C);

        stringu8 X(A);
        X.append(v, 0).append(*C).append(u, 0)
            .append(len_A).append(len_C);

        stringu8 S;
        GHASH(H, X, &S);

        stringu8 tmp;
        GCTR(K, J0, S, &tmp);
        *T = std::move(tmp.substr(0, t));
    }

    bool GCM::GCM_AD(
        const stringu8& K, const stringu8& IV, const stringu8& C, const stringu8& A, int t,
        const stringu8& T, stringu8* P)
    {
        if (IV.size() < kLenIVMin || IV.size() > kLenIVMax) { return false; }
        if (A.size() < kLenAMin || A.size() > kLenAMax)     { return false; }
        if (C.size() < kLenPMin || C.size() > kLenPMax)     { return false; }

        stringu8 H;
        CIPH(K, stringu8(16, 0), &H);

        stringu8 J0;
        if (IV.size() == 12) {
            J0 = IV;
            J0.append(3, 0).push_back(1);
        } else {
            int s = 16 * ((IV.size() + 15) / 16) - IV.size();
            stringu8 X(IV);
            X.append(s + 8, 0);

            stringu8 len;
            ByteString::len64(IV, &len);
            X.append(len);

            GHASH(H, X, &J0);
        }

        stringu8 _P;
        stringu8 J0_1(J0);
        ByteString::inc(J0_1, 4, &J0_1);
        GCTR(K, J0_1, C, &_P);

        int u = 16 * ((C.size() + 15) / 16) - C.size();
        int v = 16 * ((A.size() + 15) / 16) - A.size();

        stringu8 len_A, len_C;
        ByteString::len64(A, &len_A);
        ByteString::len64(C, &len_C);

        stringu8 X(A);
        X.append(v, 0).append(C).append(u, 0)
            .append(len_A).append(len_C);

        stringu8 S;
        GHASH(H, X, &S);

        stringu8 tmp;
        GCTR(K, J0, S, &tmp);
        auto _T = std::move(tmp.substr(0, t));
        if (T != _T) {
            return false;
        }

        *P = std::move(_P);
        return true;
    }

}
}