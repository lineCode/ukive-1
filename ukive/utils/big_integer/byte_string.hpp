#ifndef UKIVE_UTILS_BIG_INTEGER_BYTE_STRING_HPP_
#define UKIVE_UTILS_BIG_INTEGER_BYTE_STRING_HPP_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class ByteString {
    public:
        static void xor(const stringu8& a, const stringu8& b, stringu8* r);
        static void div2(const stringu8& a, stringu8* r);
        static uint8_t getBit(const stringu8& a, int idx);
        static void inc(const stringu8& a, int s, stringu8* r);
        static void len64(const stringu8& a, stringu8* r);
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER_BYTE_STRING_HPP_