#include "ukive/utils/big_integer_converter.h"

#include <algorithm>

#include "ukive/utils/big_integer8.h"
#include "ukive/utils/big_integer16.h"
#include "ukive/utils/big_integer32.h"


namespace {

    const uint32_t kUInt32Max = std::numeric_limits<uint32_t>::max();
    const uint64_t kUInt32Rec = uint64_t(std::numeric_limits<uint32_t>::max()) + 1;

}

namespace ukive {

    BigIntegerConverter::BigIntegerConverter() {}

    int64_t BigIntegerConverter::convertToInt64(const BigInteger8& bi) {
        auto size = sizeof(int64_t);
        bool is_minus = bi.isMinus();

        int64_t result = 0;
        for (size_t i = 0; i < size; ++i) {
            if (i >= bi.raw_.size()) {
                result |= uint64_t(is_minus ? 0xFF : 0) << i * 8;
            } else {
                result |= uint64_t(bi.raw_[i]) << i * 8;
            }
        }

        if (bi.raw_.size() > size) {
            if (is_minus && result >= 0) {
                result |= uint64_t(1) << 63;
            } else if (!is_minus && result <= 0) {
                result &= ~(uint64_t(1) << 63);
            }
        }

        return result;
    }

    string8 BigIntegerConverter::convertToString(const BigInteger8& bi) {
        std::vector<uint8_t> tmp;
        std::vector<uint8_t> result;
        result.push_back(0);
        std::vector<uint8_t> init;
        init.push_back(1);

        uint8_t over = 0;
        auto size = bi.raw_.size();
        bool is_minus = bi.isMinus();

        for (size_t i = 0; i < size; ++i) {
            uint8_t ret = is_minus ?
                bi.toUInt8AddOver(255 - bi.raw_[i] + over + (i == 0 ? 1 : 0), &over) : bi.raw_[i];
            if (ret > 0) {
                mul(tmp, init, ret);
                add(result, tmp);
            }
            mul(init, 256);
        }

        string8 str(result.size(), 0);
        for (size_t i = 0; i < result.size(); ++i) {
            str[result.size() - i - 1] = result[i] + '0';
        }

        if (is_minus) {
            str.insert(str.begin(), '-');
        }

        return str;
    }

    string8 BigIntegerConverter::convertToStringHex(const BigInteger8& bi) {
        bool is_minus = bi.isMinus();

        string8 result;
        uint8_t over = 0;
        for (size_t i = 0; i < bi.raw_.size(); ++i) {
            uint8_t ret = is_minus ? bi.toUInt8AddOver(255 - bi.raw_[i] + over + (i == 0 ? 1 : 0), &over) : bi.raw_[i];
            for (int j = 0; j < 2; ++j) {
                uint8_t hex = (ret >> (4 * j)) & 0xF;
                if (hex >= 0 && hex <= 9) {
                    result.insert(result.begin(), hex + '0');
                } else {
                    result.insert(result.begin(), hex - 10 + 'A');
                }
            }
        }

        result.insert(result.begin(), { '0', 'x' });
        if (is_minus) {
            result.insert(result.begin(), '-');
        }
        return result;
    }

    int64_t BigIntegerConverter::convertToInt64(const BigInteger16& bi) {
        auto size = sizeof(int64_t);
        bool is_minus = bi.isMinus();

        uint8_t over = 0;
        int64_t result = 0;
        for (size_t i = 0; i < size; ++i) {
            if (i >= bi.raw_.size()) {
                result |= uint64_t(is_minus ? 0xFFFF : 0) << i * 16;
            } else {
                if (is_minus) {
                    result |= uint64_t(bi.toUInt16AddOver(65535 - bi.raw_[i] + over + (i == 0 ? 1 : 0), &over)) << i * 16;
                } else {
                    result |= uint64_t(bi.raw_[i]) << i * 16;
                }
            }
        }

        if (bi.raw_.size() > size) {
            if (is_minus && result >= 0) {
                result |= uint64_t(1) << 63;
            } else if (!is_minus && result <= 0) {
                result &= ~(uint64_t(1) << 63);
            }
        }

        return result;
    }

    string8 BigIntegerConverter::convertToString(const BigInteger16& bi) {
        std::vector<uint8_t> tmp;
        std::vector<uint8_t> result;
        result.push_back(0);
        std::vector<uint8_t> init;
        init.push_back(1);

        auto size = bi.raw_.size();
        bool is_minus = bi.isMinus();

        for (size_t i = 0; i < size; ++i) {
            auto ret = bi.raw_[i];
            if (ret > 0) {
                mul(tmp, init, ret);
                add(result, tmp);
            }
            mul(init, 65536U);
        }

        string8 str(result.size(), 0);
        for (size_t i = 0; i < result.size(); ++i) {
            str[result.size() - i - 1] = result[i] + '0';
        }

        if (is_minus) {
            str.insert(str.begin(), '-');
        }

        return str;
    }

    string8 BigIntegerConverter::convertToStringHex(const BigInteger16& bi) {
        bool is_minus = bi.isMinus();

        string8 result;
        for (size_t i = 0; i < bi.raw_.size(); ++i) {
            auto ret = bi.raw_[i];
            for (int j = 0; j < 4; ++j) {
                uint8_t hex = (ret >> (4 * j)) & 0xF;
                if (hex >= 0 && hex <= 9) {
                    result.insert(result.begin(), hex + '0');
                } else {
                    result.insert(result.begin(), hex - 10 + 'A');
                }
            }
        }

        result.insert(result.begin(), { '0', 'x' });
        if (is_minus) {
            result.insert(result.begin(), '-');
        }
        return result;
    }

    int64_t BigIntegerConverter::convertToInt64(const BigInteger32& bi) {
        auto size = sizeof(int64_t);
        bool is_minus = bi.isMinus();

        uint8_t over = 0;
        int64_t result = 0;
        for (size_t i = 0; i < size; ++i) {
            if (i >= bi.raw_.size()) {
                result |= uint64_t(is_minus ? 0xFFFFFFFF : 0) << i * 32;
            } else {
                if (is_minus) {
                    result |= uint64_t(bi.toUInt32AddOver(kUInt32Max - bi.raw_[i] + over + (i == 0 ? 1 : 0), &over)) << i * 32;
                } else {
                    result |= uint64_t(bi.raw_[i]) << i * 32;
                }
            }
        }

        if (bi.raw_.size() > size) {
            if (is_minus && result >= 0) {
                result |= uint64_t(1) << 63;
            } else if (!is_minus && result <= 0) {
                result &= ~(uint64_t(1) << 63);
            }
        }

        return result;
    }

    string8 BigIntegerConverter::convertToString(const BigInteger32& bi) {
        std::vector<uint8_t> tmp;
        std::vector<uint8_t> result;
        result.push_back(0);
        std::vector<uint8_t> init;
        init.push_back(1);

        auto size = bi.raw_.size();
        bool is_minus = bi.isMinus();

        for (size_t i = 0; i < size; ++i) {
            auto ret = bi.raw_[i];
            if (ret > 0) {
                mul(tmp, init, ret);
                add(result, tmp);
            }
            mul(init, kUInt32Rec);
        }

        string8 str(result.size(), 0);
        for (size_t i = 0; i < result.size(); ++i) {
            str[result.size() - i - 1] = result[i] + '0';
        }

        if (is_minus) {
            str.insert(str.begin(), '-');
        }

        return str;
    }

    string8 BigIntegerConverter::convertToStringHex(const BigInteger32& bi) {
        bool is_minus = bi.isMinus();

        string8 result;
        for (size_t i = 0; i < bi.raw_.size(); ++i) {
            auto ret = bi.raw_[i];
            for (int j = 0; j < 8; ++j) {
                uint8_t hex = (ret >> (4 * j)) & 0xF;
                if (hex >= 0 && hex <= 9) {
                    result.insert(result.begin(), hex + '0');
                } else {
                    result.insert(result.begin(), hex - 10 + 'A');
                }
            }
        }

        result.insert(result.begin(), { '0', 'x' });
        if (is_minus) {
            result.insert(result.begin(), '-');
        }
        return result;
    }

    void BigIntegerConverter::add(std::vector<uint8_t>& lhs, const std::vector<uint8_t>& rhs) {
        uint8_t over = 0;
        auto size = std::max(lhs.size(), rhs.size());
        for (size_t i = 0; i < size; ++i) {
            auto ret = toUInt8AddOver(getWithExt(lhs, i) + getWithExt(rhs, i) + over, &over);
            if (lhs.size() < i + 1) {
                lhs.push_back(ret);
            } else {
                lhs[i] = ret;
            }
        }

        if (over > 0) {
            lhs.push_back(over);
        }

        shrink(lhs);
    }

    template <typename T>
    void BigIntegerConverter::mul(std::vector<uint8_t>& lhs, T val) {
        std::vector<uint8_t> rhs;
        while (val > 0) {
            rhs.push_back(val % 10);
            val /= 10;
        }

        std::vector<uint8_t> mid(lhs.size() + rhs.size(), 0);

        for (size_t i = 0; i < rhs.size(); ++i) {
            uint8_t over = 0;
            uint8_t mid_over = 0;
            for (size_t j = 0; j < lhs.size(); ++j) {
                uint8_t l_ret = getWithExt(lhs, j);
                uint8_t r_ret = getWithExt(rhs, i);

                auto ret = toUInt8MulOver(l_ret * r_ret + over, &over);
                mid[j + i] = toUInt8AddOver(mid[j + i] + ret + mid_over, &mid_over);
            }

            if (over > 0 || mid_over > 0) {
                mid[lhs.size() + i] += over + mid_over;
            }
        }

        lhs = std::move(mid);
        shrink(lhs);
    }

    template <typename T>
    void BigIntegerConverter::mul(std::vector<uint8_t>& result, std::vector<uint8_t>& lhs, T val) {
        std::vector<uint8_t> rhs;
        while (val > 0) {
            rhs.push_back(val % 10);
            val /= 10;
        }

        result.assign(lhs.size() + rhs.size(), 0);

        for (size_t i = 0; i < rhs.size(); ++i) {
            uint8_t over = 0;
            uint8_t mid_over = 0;
            for (size_t j = 0; j < lhs.size(); ++j) {
                uint8_t l_ret = getWithExt(lhs, j);
                uint8_t r_ret = getWithExt(rhs, i);

                auto ret = toUInt8MulOver(l_ret * r_ret + over, &over);
                result[j + i] = toUInt8AddOver(result[j + i] + ret + mid_over, &mid_over);
            }

            if (over > 0 || mid_over > 0) {
                result[lhs.size() + i] += over + mid_over;
            }
        }

        shrink(result);
    }

    void BigIntegerConverter::shrink(std::vector<uint8_t>& vec) {
        size_t idx = vec.size();
        for (size_t i = vec.size(); i > 1; --i) {
            if (vec[i - 1] == 0) {
                idx = i - 1;
            } else {
                break;
            }
        }

        if (idx < vec.size()) {
            vec.erase(vec.begin() + idx, vec.end());
        }
    }

    uint8_t BigIntegerConverter::getWithExt(const std::vector<uint8_t>& vec, size_t idx) const {
        if (idx >= vec.size()) {
            return 0;
        }
        return vec[idx];
    }

    uint8_t BigIntegerConverter::toUInt8AddOver(uint8_t val, uint8_t* over) const {
        if (val > 9) {
            *over = 1;
            val -= 10;
        } else {
            *over = 0;
        }
        return val;
    }

    uint8_t BigIntegerConverter::toUInt8MulOver(uint8_t val, uint8_t* over) const {
        *over = val / 10;
        return val % 10;
    }

}