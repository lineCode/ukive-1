#include "ukive/utils/big_integer16.h"

#include <algorithm>
#include <stack>

#include "ukive/utils/big_integer_converter.h"
#include "ukive/log.h"


namespace ukive {

    const BigInteger16 BigInteger16::ZERO = fromU16(0);
    const BigInteger16 BigInteger16::ONE = fromU16(1);
    const BigInteger16 BigInteger16::TWO = fromU16(2);

    BigInteger16::BigInteger16()
        : is_nan_(false),
          is_minus_(false) {}

    BigInteger16 BigInteger16::from16(int16_t val) {
        BigInteger16 big_integer;

        if (val >= 0) {
            big_integer.raw_.push_back(val);
        } else {
            uint8_t over;
            big_integer.raw_.push_back(toUInt16AddOver(uint32_t(65535) - static_cast<uint16_t>(val) + 1, &over));
            if (over) {
                big_integer.raw_.push_back(over);
            }
            big_integer.is_minus_ = true;
        }
        return big_integer;
    }

    BigInteger16 BigInteger16::fromU16(uint16_t val) {
        BigInteger16 big_integer;
        big_integer.raw_.push_back(val);
        return big_integer;
    }

    BigInteger16 BigInteger16::from32(int32_t val) {
        BigInteger16 big_integer;
        big_integer.raw_.assign(2, 0);

        if (val >= 0) {
            for (int i = 0; i < 2; ++i) {
                big_integer.raw_[i] = static_cast<uint16_t>(val >> (i * 16));
            }
        } else {
            uint8_t over = 0;
            for (int i = 0; i < 2; ++i) {
                big_integer.raw_[i] =
                    toUInt16AddOver(uint32_t(65535U) - static_cast<uint16_t>(val >> (i * 16)) + over + (i == 0 ? 1 : 0), &over);
            }

            if (over) {
                big_integer.raw_.push_back(over);
            }

            big_integer.is_minus_ = true;
        }

        big_integer.shrink();
        return big_integer;
    }

    BigInteger16 BigInteger16::fromU32(uint32_t val) {
        BigInteger16 big_integer;
        big_integer.raw_.assign(2, 0);

        for (int i = 0; i < 2; ++i) {
            big_integer.raw_[i] = static_cast<uint16_t>(val >> (i * 16));
        }

        big_integer.shrink();
        return big_integer;
    }

    // static
    BigInteger16 BigInteger16::from64(int64_t val) {
        BigInteger16 big_integer;
        big_integer.raw_.assign(4, 0);

        if (val >= 0) {
            for (int i = 0; i < 4; ++i) {
                big_integer.raw_[i] = static_cast<uint16_t>(val >> (i * 16));
            }
        } else {
            uint8_t over = 0;
            for (int i = 0; i < 4; ++i) {
                big_integer.raw_[i] =
                    toUInt16AddOver(uint32_t(65535U) - static_cast<uint16_t>(val >> (i * 16)) + over + (i == 0 ? 1 : 0), &over);
            }

            if (over) {
                big_integer.raw_.push_back(over);
            }

            big_integer.is_minus_ = true;
        }

        big_integer.shrink();
        return big_integer;
    }

    // static
    BigInteger16 BigInteger16::fromU64(uint64_t val) {
        BigInteger16 big_integer;
        big_integer.raw_.assign(4, 0);

        for (int i = 0; i < 4; ++i) {
            big_integer.raw_[i] = static_cast<uint16_t>(val >> (i * 16));
        }

        big_integer.shrink();
        return big_integer;
    }

    BigInteger16& BigInteger16::add(const BigInteger16& rhs) {
        if (is_nan_) return *this;

        bool minus = isMinus();
        bool r_minus = rhs.isMinus();
        if (minus ^ r_minus) {
            if (compareUs(*this, rhs) >= 0) {
                subUsTL(*this, rhs);
                if (isZero()) is_minus_ = false;
            } else {
                subUsTR(rhs, *this);
                is_minus_ = !is_minus_;
            }
        } else {
            addUs(*this, rhs);
        }

        shrink();
        return *this;
    }

    BigInteger16& BigInteger16::sub(const BigInteger16& rhs) {
        if (is_nan_) return *this;

        bool minus = isMinus();
        bool r_minus = rhs.isMinus();
        if (minus ^ r_minus) {
            addUs(*this, rhs);
        } else {
            if (compareUs(*this, rhs) >= 0) {
                subUsTL(*this, rhs);
                if (isZero()) is_minus_ = false;
            } else {
                subUsTR(rhs, *this);
                is_minus_ = !is_minus_;
            }
        }

        shrink();
        return *this;
    }

    BigInteger16& BigInteger16::mul(const BigInteger16& rhs) {
        if (is_nan_) return *this;

        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();

        mulUs(*this, rhs);
        if (isZero()) {
            is_minus_ = false;
        } else {
            is_minus_ = is_l_minus ^ is_r_minus;
        }

        shrink();
        return *this;
    }

    BigInteger16& BigInteger16::div(const BigInteger16& rhs, BigInteger16* rem) {
        if (is_nan_) return *this;

        if (rhs.isZero()) {
            is_nan_ = true;
            raw_ = std::move(fromU16(0).raw_);
            return *this;
        }

        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();

        divModUs(*this, rhs, false, rem);
        if (isZero()) {
            is_minus_ = false;
        } else {
            is_minus_ = is_l_minus ^ is_r_minus;
        }

        shrink();

        if (rem) {
            rem->is_minus_ = is_l_minus;
            rem->shrink();
        }

        return *this;
    }

    BigInteger16& BigInteger16::mod(const BigInteger16& rhs) {
        if (is_nan_) return *this;

        if (rhs.isZero()) {
            is_nan_ = true;
            raw_ = std::move(fromU16(0).raw_);
            return *this;
        }

        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();

        divModUs(*this, rhs, true);
        if (isZero()) {
            is_minus_ = false;
        } else {
            is_minus_ = is_l_minus;
        }
        shrink();

        return *this;
    }

    BigInteger16& BigInteger16::pow(const BigInteger16& exp) {
        if (is_nan_) return *this;

        *this = powRecur(exp);
        return *this;
    }

    BigInteger16& BigInteger16::powMod(const BigInteger16& exp, const BigInteger16& rem) {
        if (is_nan_) return *this;

        *this = powModRecur(exp, rem);
        return *this;
    }

    BigInteger16& BigInteger16::shl(uint32_t off) {
        if (is_nan_) return *this;
        if (off == 0 || isZero()) return *this;

        uint32_t strip = off / 16;
        uint32_t inoff = off % 16;

        raw_.insert(raw_.begin(), strip, 0);

        uint16_t prev = 0;
        for (size_t i = 0; i < raw_.size(); ++i) {
            auto cur = raw_[i] >> (16 - inoff);
            raw_[i] <<= inoff;
            raw_[i] |= prev;
            prev = cur;
        }

        if (prev) {
            raw_.push_back(prev);
        }

        return *this;
    }

    BigInteger16& BigInteger16::shr(uint32_t off) {
        if (is_nan_) return *this;
        if (off == 0 || isZero()) return *this;

        uint32_t strip = off / 16;
        uint32_t inoff = off % 16;

        if (strip >= raw_.size()) {
            *this = ZERO;
            return *this;
        }
        raw_.erase(raw_.begin(), raw_.begin() + strip);

        uint16_t prev = 0;
        for (size_t i = raw_.size(); i > 0; --i) {
            auto cur = raw_[i - 1] << (16 - inoff);
            raw_[i - 1] >>= inoff;
            raw_[i - 1] |= prev;
            prev = cur;
        }

        shrink();

        return *this;
    }

    BigInteger16& BigInteger16::abs() {
        if (is_nan_) return *this;
        if (!isMinus()) return *this;
        is_minus_ = false;
        return *this;
    }

    BigInteger16& BigInteger16::inv() {
        if (is_nan_) return *this;
        if (isZero()) return *this;
        is_minus_ = !is_minus_;
        return *this;
    }

    BigInteger16 BigInteger16::operator+(const BigInteger16& rhs) const {
        BigInteger16 tmp(*this);
        tmp.add(rhs);
        return tmp;
    }

    BigInteger16 BigInteger16::operator-(const BigInteger16& rhs) const {
        BigInteger16 tmp(*this);
        tmp.sub(rhs);
        return tmp;
    }

    BigInteger16 BigInteger16::operator*(const BigInteger16& rhs) const {
        BigInteger16 tmp(*this);
        tmp.mul(rhs);
        return tmp;
    }

    BigInteger16 BigInteger16::operator/(const BigInteger16& rhs) const {
        BigInteger16 tmp(*this);
        tmp.div(rhs);
        return tmp;
    }

    BigInteger16 BigInteger16::operator%(const BigInteger16& rhs) const {
        BigInteger16 tmp(*this);
        tmp.mod(rhs);
        return tmp;
    }

    bool BigInteger16::operator>(const BigInteger16& rhs) const {
        return compare(rhs) == 1;
    }

    bool BigInteger16::operator>=(const BigInteger16& rhs) const {
        return compare(rhs) >= 0;
    }

    bool BigInteger16::operator<(const BigInteger16& rhs) const {
        return compare(rhs) == -1;
    }

    bool BigInteger16::operator<=(const BigInteger16& rhs) const {
        return compare(rhs) <= 0;
    }

    bool BigInteger16::operator==(const BigInteger16& rhs) const {
        return compare(rhs) == 0;
    }

    int BigInteger16::compare(const BigInteger16& rhs) const {
        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();
        if (!is_l_minus && is_r_minus) {
            return 1;
        }
        if (is_l_minus && !is_r_minus) {
            return -1;
        }

        int result = compareUs(*this, rhs);
        if (is_l_minus && is_r_minus) {
            return -result;
        }
        if (!is_l_minus && !is_r_minus) {
            return result;
        }

        return 0;
    }

    bool BigInteger16::isNaN() const {
        return is_nan_;
    }

    bool BigInteger16::isEven() const {
        return !(raw_.front() & 0x1);
    }

    bool BigInteger16::isZero() const {
        for (const auto& unit : raw_) {
            if (unit != 0) {
                return false;
            }
        }
        return true;
    }

    bool BigInteger16::isMinus() const {
        return is_minus_;
    }

    bool BigInteger16::isBeyondInt64() const {
        if (raw_.size() < sizeof(int64_t) / 2) {
            return false;
        }
        if (raw_.size() > sizeof(int64_t) / 2) {
            return true;
        }

        if (!is_minus_ && (raw_[3] & 0x8000)) {
            return true;
        }
        if (is_minus_ && (raw_[3] & 0x8000) && (raw_[0] || raw_[1] || raw_[2] || raw_[3] &~0x8000)) {
            return true;
        }
        return false;
    }

    int64_t BigInteger16::toInt64() const {
        return BigIntegerConverter().convertToInt64(*this);
    }

    string8 BigInteger16::toString() const {
        return BigIntegerConverter().convertToString(*this);
    }

    string8 BigInteger16::toStringHex() const {
        return BigIntegerConverter().convertToStringHex(*this);
    }

    void BigInteger16::addUs(BigInteger16& left, const BigInteger16& right) const {
        uint8_t over = 0;
        auto size = std::max(left.raw_.size(), right.raw_.size());
        for (size_t i = 0; i < size; ++i) {
            auto ret = toUInt16AddOver(uint32_t(left.getWithExt(i)) + right.getWithExt(i) + over, &over);
            if (left.raw_.size() < i + 1) {
                left.raw_.push_back(ret);
            } else {
                left.raw_[i] = ret;
            }
        }
    }

    void BigInteger16::subUsTL(BigInteger16& left, const BigInteger16& right) const {
        uint8_t borrow = 0;
        auto size = std::max(left.raw_.size(), right.raw_.size());
        for (size_t i = 0; i < size; ++i) {
            auto l = left.getWithExt(i);
            auto r = right.getWithExt(i);
            if (l < borrow) {
                borrow = 1;
                l -= borrow;
            } else {
                l -= borrow;
                borrow = (l < r) ? 1 : 0;
            }

            if (left.raw_.size() < i + 1) {
                left.raw_.push_back(l - r);
            } else {
                left.raw_[i] = l - r;
            }
        }
    }

    void BigInteger16::subUsTR(const BigInteger16& left, BigInteger16& right) const {
        uint8_t borrow = 0;
        auto size = std::max(left.raw_.size(), right.raw_.size());
        for (size_t i = 0; i < size; ++i) {
            auto l = left.getWithExt(i);
            auto r = right.getWithExt(i);
            if (l < borrow) {
                borrow = 1;
                l -= borrow;
            } else {
                l -= borrow;
                borrow = (l < r) ? 1 : 0;
            }

            if (right.raw_.size() < i + 1) {
                right.raw_.push_back(l - r);
            } else {
                right.raw_[i] = l - r;
            }
        }
    }

    void BigInteger16::mulUs(BigInteger16& left, const BigInteger16& right) const {
        std::vector<uint16_t> mid(left.raw_.size() + right.raw_.size(), 0);

        for (size_t i = 0; i < right.raw_.size(); ++i) {
            uint16_t over = 0;
            uint8_t mid_over = 0;
            auto r_ret = right.raw_[i];

            for (size_t j = 0; j < left.raw_.size(); ++j) {
                auto l_ret = left.raw_[j];
                auto ret = toUInt16MulOver(uint32_t(l_ret) * r_ret + over, &over);
                mid[j + i] = toUInt16AddOver(uint32_t(mid[j + i]) + ret + mid_over, &mid_over);
            }

            if (over > 0 || mid_over > 0) {
                mid[left.raw_.size() + i] += over + mid_over;
            }
        }

        left.raw_ = std::move(mid);
    }

    BigInteger16 BigInteger16::mulUs(const BigInteger16& left, const BigInteger16& right) const {
        auto tmp = left;
        mulUs(tmp, right);
        return tmp;
    }

    void BigInteger16::divModUs(BigInteger16& left, const BigInteger16& rhs, bool is_mod, BigInteger16* rem) const {
        std::vector<uint16_t> mid;

        BigInteger16 divid;

        bool first = true;
        size_t idx = raw_.size();
        size_t r_size = rhs.raw_.size();

        for (;;) {
            bool insuff = false;
            size_t d_size = divid.raw_.size();

            size_t need;
            size_t zero_size = 0;
            if (r_size < d_size) {
                need = idx;
                insuff = true;
            } else {
                need = r_size - d_size;
                if (idx < need) {
                    zero_size = is_mod ? 0 : idx;
                    need = idx;
                    insuff = true;
                }
            }

            if (!is_mod && !first && !insuff && need > 1) {
                zero_size = need - 1;
            }

            mid.insert(mid.begin(), zero_size, 0);

            idx -= need;
            divid.raw_.insert(divid.raw_.begin(), raw_.begin() + idx, raw_.begin() + (idx + need));
            divid.shrink();
            if (insuff) {
                break;
            }

            bool beyond;
            bool is_next = false;
            for (;;) {
                beyond = compareUs(divid, rhs) >= 0;
                if (beyond || idx == 0) {
                    break;
                }

                --idx;
                is_next = true;
                divid.raw_.insert(divid.raw_.begin(), raw_[idx]);
                divid.shrink();
                if (!is_mod && need) {
                    mid.insert(mid.begin(), 0);
                }
            }

            uint16_t top = 0;
            if (beyond) {
                if (!is_next) {
                    if (divid.raw_.size() > 1 && rhs.raw_.size() > 1) {
                        top = divid.getBack2() / rhs.getBack2();
                    } else {
                        top = divid.raw_.back() / rhs.raw_.back();
                    }
                } else {
                    if (divid.raw_.size() > 2 && rhs.raw_.size() > 1) {
                        top = divid.getBack3() / rhs.getBack2();
                    } else {
                        if (divid.raw_.size() > 1) {
                            top = std::min(divid.getBack2() / rhs.raw_.back(), 65535U);
                        } else {
                            top = divid.raw_.back() / rhs.raw_.back();
                        }
                    }
                }
            }

            //int count = 0;

            for (uint16_t i = top; i > 0; --i) {
                //++count;
                auto tmp = fromU16(i);

                BigInteger16 test = mulUs(rhs, tmp);
                test.shrink();
                if (compareUs(test, divid) <= 0) {
                    if (!is_mod) {
                        mid.insert(mid.begin(), i);
                    }

                    if (beyond) {
                        subUsTL(divid, test);
                        divid.shrink(true);
                    }
                    break;
                }
            }

            /*if (top > 0 && count > 2) {
                LOG(Log::INFO) << "Div retry: " << count;
            }*/

            if (!is_mod) {
                if (top == 0) {
                    mid.insert(mid.begin(), 0);
                }
            }

            if (idx == 0) {
                break;
            }

            first = false;
        }

        if (is_mod) {
            left.raw_ = std::move(divid.raw_);
        } else {
            if (rem) {
                *rem = std::move(divid);
            }
            left.raw_ = std::move(mid);
        }

        if (left.raw_.empty()) {
            left.raw_.push_back(0);
        }
    }

    int BigInteger16::compareUs(const BigInteger16& left, const BigInteger16& right) const {
        if (left.raw_.size() > right.raw_.size()) {
            return 1;
        }
        if (left.raw_.size() < right.raw_.size()) {
            return -1;
        }
        for (size_t i = left.raw_.size(); i > 0; --i) {
            if (left.raw_[i - 1] > right.raw_[i - 1]) {
                return 1;
            }
            if (left.raw_[i - 1] < right.raw_[i - 1]) {
                return -1;
            }
        }
        return 0;
    }

    void BigInteger16::shrink(bool all) {
        size_t limit = all ? 0 : 1;

        size_t idx = raw_.size();
        for (size_t i = raw_.size(); i > limit; --i) {
            if (raw_[i - 1] == 0) {
                idx = i - 1;
            } else {
                break;
            }
        }

        if (idx < raw_.size()) {
            raw_.erase(raw_.begin() + idx, raw_.end());
        }
    }

    BigInteger16 BigInteger16::powRecur(const BigInteger16& exp) {
        BigInteger16 expl = exp;
        BigInteger16 result = ONE;

        std::stack<int> stack;

        for (;;) {
            if (expl.isZero()) {
                break;
            }
            if (expl.isEven()) {
                expl.shr(1);
                stack.push(1);
            } else {
                expl.sub(ONE).shr(1);
                stack.push(2);
            }
        }

        while (!stack.empty()) {
            switch (stack.top()) {
            case 1:
                result.mul(result);
                break;
            case 2:
                result.mul(result).mul(*this);
                break;
            default:
                break;
            }
            stack.pop();
        }
        return result;
    }

    BigInteger16 BigInteger16::powModRecur(const BigInteger16& exp, const BigInteger16& rem) {
        BigInteger16 expl = exp;
        BigInteger16 result = ONE;

        std::stack<int> stack;

        while (true) {
            if (expl.isZero()) {
                break;
            }
            if (expl.isEven()) {
                expl.shr(1);
                stack.push(1);
            } else {
                expl.sub(ONE).shr(1);
                stack.push(2);
            }
        }

        while (!stack.empty()) {
            switch (stack.top()) {
            case 1:
                result.mul(result).mod(rem);
                break;
            case 2:
                result.mul(result).mul(*this).mod(rem);
                break;
            default:
                break;
            }
            stack.pop();
        }
        return result;
    }

    uint16_t BigInteger16::get(size_t idx) const {
        return raw_[idx];
    }

    uint16_t BigInteger16::getWithExt(size_t idx) const {
        if (idx >= raw_.size()) {
            return 0x00;
        }
        return get(idx);
    }

    uint32_t BigInteger16::getBack2() const {
        return uint32_t(raw_.back()) * 65536 + raw_[raw_.size() - 2];
    }

    uint64_t BigInteger16::getBack3() const {
        return uint64_t(raw_.back()) * 65536 * 65536 + uint64_t(raw_[raw_.size() - 2]) * 65536 + raw_[raw_.size() - 3];
    }

    uint16_t BigInteger16::toUInt16AddOver(uint32_t val, uint8_t* over) {
        if (val > 65535) {
            *over = 1;
        } else {
            *over = 0;
        }
        return static_cast<uint16_t>(val);
    }

    uint16_t BigInteger16::toUInt16MulOver(uint32_t val, uint16_t* over) {
        *over = val / 65536;
        return static_cast<uint16_t>(val % 65536);
    }

}
