#ifndef UTILS_CONVERT_H_
#define UTILS_CONVERT_H_

#include <cstdint>


namespace utl {

    int16_t swapBits(int16_t val);
    uint16_t swapBits(uint16_t val);
    int32_t swapBits(int32_t val);
    uint32_t swapBits(uint32_t val);
    int64_t swapBits(int64_t val);
    uint64_t swapBits(uint64_t val);

}

#endif  // UTILS_CONVERT_H_