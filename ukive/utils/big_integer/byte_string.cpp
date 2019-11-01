#include "ukive/utils/big_integer/byte_string.hpp"


namespace ukive {

    void ByteString::xor(const stringu8& a, const stringu8& b, stringu8* r) {
        const stringu8* min;
        const stringu8* max;
        if (a.size() < b.size()) {
            min = &a;
            max = &b;
            r->resize(b.size());
        } else {
            min = &b;
            max = &a;
            r->resize(a.size());
        }

        size_t i;
        for (i = 0; i < min->size(); ++i) {
            (*r)[i] = (a[i] ^ b[i]);
        }

        for (; i < max->size(); ++i) {
            (*r)[i] = (*max)[i];
        }
    }

    void ByteString::div2(const stringu8& a, stringu8* r) {
        r->resize(a.size());
        uint8_t rem = 0;
        for (size_t i = 0; i < a.size(); ++i) {
            uint8_t tmp = a[i];
            (*r)[i] = (tmp >> 1) | (rem << 7);
            rem = tmp & 0x01;
        }
    }

    uint8_t ByteString::getBit(const stringu8& a, int idx) {
        int pos = idx / 8;
        int off = idx % 8;
        return (a[pos] >> (7 - off)) & 1;
    }

    void ByteString::inc(const stringu8& a, int s, stringu8* r) {
        r->resize(a.size());

        int ls = a.size() - s;
        uint8_t over = 1;
        for (int i = int(a.size()) - 1; i >= ls; --i) {
            uint16_t tmp = a[i] + over;
            (*r)[i] = tmp & 0xFF;
            over = tmp >> 8;
        }

        for (int i = 0; i < ls; ++i) {
            (*r)[i] = a[i];
        }
    }

    void ByteString::len64(const stringu8& a, stringu8* r) {
        r->resize(8);

        uint64_t iv_len = a.size() * 8;
        for (int i = 0; i < 8; ++i) {
            (*r)[i] = iv_len >> ((7 - i) * 8);
        }
    }

}