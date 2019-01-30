#include "ukive/utils/unicode.h"

#define IN_RANGE(var, min, max)  \
    (var) >= (min) && (var) <= (max)

#define IF_IN_RANGE(var, min, max)  \
    if (IN_RANGE(var, min, max)) {

#define ELIF_IN_RANGE(var, min, max)  \
    } else if (IN_RANGE(var, min, max)) {

#define EQUAL(var, v)  \
    (var) == (v)

#define IF_EQUAL(var, v)  \
    if (EQUAL(var, v)) {

#define ELIF_EQUAL(var, v)  \
    } else if (EQUAL(var, v)) {

#define ELSE  \
    } else {

#define END_IF  \
    }

#define ELSE_RET_FALSE  \
    ELSE return false; END_IF

#define CHECK_LENGTH(l)  \
    if (src_length - index < (l)) {  \
        return false;  \
    END_IF

#define CHECK_IN_RANGE(var, min, max)  \
    if (!(IN_RANGE(var, min, max))) {  \
        return false;  \
    END_IF

#define GET_BYTE(no)  \
    auto byte##no = static_cast<uint8_t>(src[index + (no) - 1])

#define GET_BYTE_AND_CHECK_RANGE(no, min, max)  \
    GET_BYTE(no);  \
    CHECK_IN_RANGE(byte##no, min, max)

#define GET_WORD(no)  \
    auto word##no = static_cast<uint16_t>(src[index + (no) - 1])

#define GET_WORD_AND_CHECK_RANGE(no, min, max)  \
    GET_WORD(no);  \
    CHECK_IN_RANGE(word##no, min, max)


namespace ukive {

    bool Unicode::UTF8ToUTF16(const string8& src, string16* dst) {
        if (src.empty()) {
            *dst = string16();
            return true;
        }

        string16::size_type index = 0;
        auto src_length = src.length();
        uint32_t scalar_value;
        string16 utf16_string;

        while (index < src_length) {
            GET_BYTE(1);
            IF_IN_RANGE(byte1, 0x00, 0x7F)   // 1 byte
                scalar_value = getScalarValue(byte1);
                ++index;
            ELIF_IN_RANGE(byte1, 0xC2, 0xDF) // 2 byte
                CHECK_LENGTH(2)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2);
                index += 2;
            ELIF_EQUAL(byte1, 0xE0)          // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0xA0, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_IN_RANGE(byte1, 0xE1, 0xEC) // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_EQUAL(byte1, 0xED)          // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0x9F)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_IN_RANGE(byte1, 0xEE, 0xEF) // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_EQUAL(byte1, 0xF0)          // 4 byte
                CHECK_LENGTH(4)
                GET_BYTE_AND_CHECK_RANGE(2, 0x90, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3, byte4);
                index += 4;
            ELIF_IN_RANGE(byte1, 0xF1, 0xF3) // 4 byte
                CHECK_LENGTH(4)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3, byte4);
                index += 4;
            ELIF_EQUAL(byte1, 0xF4)          // 4 byte
                CHECK_LENGTH(4)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0x8F)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3, byte4);
                index += 4;
            ELSE_RET_FALSE

            utf16_string += SVToUTF16(scalar_value);
        }

        *dst = utf16_string;
        return true;
    }

    bool Unicode::UTF16ToUTF8(const string16& src, string8* dst) {
        if (src.empty()) {
            *dst = string8();
            return true;
        }

        string8::size_type index = 0;
        auto src_length = src.length();
        uint32_t scalar_value;
        string8 utf8_string;

        while (index < src_length) {
            GET_WORD(1);
            IF_IN_RANGE(word1, 0x0000, 0xD7FF)   // 1 word
                scalar_value = getScalarValue(word1);
                ++index;
            ELIF_IN_RANGE(word1, 0xE000, 0xFFFF) // 1 word
                scalar_value = getScalarValue(word1);
                ++index;
            ELSE                                 // 2 word
                CHECK_LENGTH(2)
                GET_WORD(2);
                scalar_value = getScalarValue(word1, word2);
                index += 2;
            END_IF

            utf8_string += SVToUTF8(scalar_value);
        }

        *dst = utf8_string;
        return true;
    }


    string8 Unicode::SVToUTF8(uint32_t sv) {
        string8 result;
        if((sv & 0xFFFF80) == 0) {
            // 1 byte
            result.push_back(static_cast<char>(sv & 0x7F));
        } else if((sv & 0xFFF800) == 0) {
            // 2 byte
            result.push_back(static_cast<char>(((sv & 0x7C0) >> 6) + 0xC0));
            result.push_back(static_cast<char>((sv & 0x3F) + 0x80));
        } else if ((sv & 0xFF0000) == 0) {
            // 3 byte
            result.push_back(static_cast<char>(((sv & 0xF000) >> 12) + 0xE0));
            result.push_back(static_cast<char>(((sv & 0xFC0) >> 6) + 0x80));
            result.push_back(static_cast<char>((sv & 0x3F) + 0x80));
        } else {
            // 4 byte
            result.push_back(static_cast<char>(((sv & 0x1C0000) >> 18) + 0xF0));
            result.push_back(static_cast<char>(((sv & 0x3F000) >> 12) + 0x80));
            result.push_back(static_cast<char>(((sv & 0x7C0) >> 6) + 0x80));
            result.push_back(static_cast<char>((sv & 0x3F) + 0x80));
        }

        return result;
    }

    string16 Unicode::SVToUTF16(uint32_t sv) {
        string16 result;
        if ((sv & 0x1F0000) == 0) {
            // 1 word
            result.push_back(static_cast<wchar_t>(sv & 0xFFFF));
        } else {
            // 2 word
            result.push_back(static_cast<wchar_t>(
                ((sv & 0xFC00) >> 10) +
                (((sv & 0x1F0000 - 1) & 0xF0000) >> 16) + 0xD800));
            result.push_back(static_cast<wchar_t>((sv & 0x3FF) + 0xDC00));
        }

        return result;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte) {
        return byte;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte1, uint8_t byte2) {
        uint32_t result = (byte1 & 0x1F) << 6;
        result += (byte2 & 0x3F);
        return result;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
        uint32_t result = (byte1 & 0xF) << 12;
        result += (byte2 & 0x3F) << 6;
        result += (byte3 & 0x3F);
        return result;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) {
        uint32_t result = (byte1 & 0x7) << 18;
        result += (byte2 & 0x3F) << 12;
        result += (byte3 & 0x3F) << 6;
        result += (byte4 & 0x3F);
        return result;
    }

    uint32_t Unicode::getScalarValue(uint16_t word) {
        return word;
    }

    uint32_t Unicode::getScalarValue(uint16_t word1, uint16_t word2) {
        uint32_t result = ((word1 & 0x3C0) + 1) << 16;
        result += (word1 & 0x3F) << 10;
        result += (word2 & 0x3FF);
        return result;
    }

}