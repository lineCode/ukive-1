#ifndef UKIVE_UTILS_CONVERT_H_
#define UKIVE_UTILS_CONVERT_H_

#include <cstdint>


namespace ukive {

    int16_t swapBits(int16_t val);
    uint16_t swapBits(uint16_t val);
    int32_t swapBits(int32_t val);
    uint32_t swapBits(uint32_t val);
    int64_t swapBits(int64_t val);
    uint64_t swapBits(uint64_t val);

}

#endif  // UKIVE_UTILS_CONVERT_H_