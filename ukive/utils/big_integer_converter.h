#ifndef UKIVE_UTILS_BIG_INTEGER_CONVERTER_H_
#define UKIVE_UTILS_BIG_INTEGER_CONVERTER_H_

#include <cstdint>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {
    class BigInteger2;
    class BigInteger8;
    class BigInteger16;
    class BigInteger32;

    class BigIntegerConverter {
    public:
        BigIntegerConverter();

        int64_t convertToInt64(const BigInteger8& bi);
        string8 convertToString(const BigInteger8& bi);
        string8 convertToStringHex(const BigInteger8& bi);

        int64_t convertToInt64(const BigInteger16& bi);
        string8 convertToString(const BigInteger16& bi);
        string8 convertToStringHex(const BigInteger16& bi);

        int64_t convertToInt64(const BigInteger32& bi);
        string8 convertToString(const BigInteger32& bi);
        string8 convertToStringHex(const BigInteger32& bi);

    private:
        void add(std::vector<uint8_t>& lhs, const std::vector<uint8_t>& rhs);

        template <typename T>
        void mul(std::vector<uint8_t>& lhs, T val);

        template <typename T>
        void mul(std::vector<uint8_t>& result, std::vector<uint8_t>& lhs, T val);

        void shrink(std::vector<uint8_t>& vec);

        uint8_t getWithExt(const std::vector<uint8_t>& vec, size_t idx) const;

        uint8_t toUInt8AddOver(uint8_t val, uint8_t* over) const;
        uint8_t toUInt8MulOver(uint8_t val, uint8_t* over) const;
    };

}

#endif  // UKIVE_UTILS_BIG_INTEGER_CONVERTER_H_