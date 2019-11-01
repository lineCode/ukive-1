#include "ukive/utils/convert.h"


namespace ukive {

    int16_t swapBits(int16_t val) {
        return (val << 8) | ((val >> 8) & 0xFF);
    }

    uint16_t swapBits(uint16_t val) {
        return (val << 8) | (val >> 8);
    }

    int32_t swapBits(int32_t val) {
        return (val << 24) | ((val << 8) & 0x00FF0000) |
            ((val >> 24) & 0xFF) | ((val >> 8) & 0x0000FF00);
    }

    uint32_t swapBits(uint32_t val) {
        return (val << 24) | ((val << 8) & 0x00FF0000) |
            (val >> 24) | ((val >> 8) & 0x0000FF00);
    }

    int64_t swapBits(int64_t val) {
        return (val << 56) | ((val << 40) & 0xFF000000000000) | ((val << 24) & 0xFF0000000000) | ((val << 8) & 0xFF00000000) |
            (val >> 56 & 0xFF) | ((val >> 40) & 0xFF00) | ((val >> 24) & 0xFF0000) | ((val >> 8) & 0xFF000000);
    }

    uint64_t swapBits(uint64_t val) {
        return (val << 56) | ((val << 40) & 0xFF000000000000) | ((val << 24) & 0xFF0000000000) | ((val << 8) & 0xFF00000000) |
            (val >> 56) | ((val >> 40) & 0xFF00) | ((val >> 24) & 0xFF0000) | ((val >> 8) & 0xFF000000);
    }

}