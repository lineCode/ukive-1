#ifndef UTILS_UNICODE_H_
#define UTILS_UNICODE_H_

#include "utils/string_utils.h"


namespace utl {

    class Unicode {
    public:
        static bool UTF8ToUTF16(const string8& src, string16* dst);
        static bool UTF8ToUTF32(const string8& src, std::u32string* dst);
        static bool UTF16ToUTF8(const string16& src, string8* dst);
        static bool UTF16ToUTF32(const string16& src, std::u32string* dst);
        static void UTF32ToUTF8(const std::u32string& src, string8* dst);
        static void UTF32ToUTF16(const std::u32string& src, string16* dst);

    private:
        static bool UTF8ToOthers(const string8& src, string16* dst16, std::u32string* dst32);
        static bool UTF16ToOthers(const string16& src, string8* dst8, std::u32string* dst32);

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

#endif  // UTILS_UNICODE_H_