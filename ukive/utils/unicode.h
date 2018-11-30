#ifndef UKIVE_UTILS_UNICODE_H_
#define UKIVE_UTILS_UNICODE_H_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Unicode {
    public:
        static bool UTF8ToUTF16(const string8& src, string16* dst);
        static bool UTF16ToUTF8(const string16& src, string8* dst);

    private:
        static string8 SVToUTF8(uint32_t sv);
        static string16 SVToUTF16(uint32_t sv);

        inline static uint32_t getScalarValue(uint8_t byte);
        inline static uint32_t getScalarValue(uint8_t byte1, uint8_t byte2);
        inline static uint32_t getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3);
        inline static uint32_t getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);

        inline static uint32_t getScalarValue(uint16_t word);
        inline static uint32_t getScalarValue(uint16_t word1, uint16_t word2);
    };

}

#endif  // UKIVE_UTILS_UNICODE_H_