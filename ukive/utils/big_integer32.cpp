#include "ukive/utils/big_integer32.h"

#include <algorithm>
#include <stack>

#include "ukive/utils/big_integer_converter.h"
#include "ukive/log.h"


namespace {

    const uint32_t kUInt32Max = std::numeric_limits<uint32_t>::max();
    const uint64_t kUInt32Rec = uint64_t(std::numeric_limits<uint32_t>::max()) + 1;

}

namespace ukive {

    BigInteger32::BigInteger32()
        : is_nan_(false),
          is_minus_(false) {}

    // static
    BigInteger32 BigInteger32::fromVal(int64_t val) {
        BigInteger32 big_integer;
        big_integer.raw_.assign(2, 0);

        if (val >= 0) {
            for (int i = 0; i < 2; ++i) {
                big_integer.raw_[i] = static_cast<uint32_t>(val >> (i * 32));
            }
        } else {
            uint8_t over = 0;
            for (int i = 0; i < 2; ++i) {
                big_integer.raw_[i] =
                    toUInt32AddOver(uint64_t(kUInt32Max) - static_cast<uint32_t>(val >> (i * 32)) + over + (i == 0 ? 1 : 0), &over);
            }

            if (over) {
                big_integer.raw_.push_back(over);
            }
            big_integer.is_minus_ = true;
        }

        big_integer.shrink();
        return big_integer;
    }

    BigInteger32& BigInteger32::add(const BigInteger32& rhs) {
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

    BigInteger32& BigInteger32::sub(const BigInteger32& rhs) {
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

    BigInteger32& BigInteger32::mul(const BigInteger32& rhs) {
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

    BigInteger32& BigInteger32::div(const BigInteger32& rhs, BigInteger32* rem) {
        if (is_nan_) return *this;

        if (rhs.isZero()) {
            is_nan_ = true;
            raw_ = std::move(fromVal(0).raw_);
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

    BigInteger32& BigInteger32::mod(const BigInteger32& rhs) {
        if (is_nan_) return *this;

        if (rhs.isZero()) {
            is_nan_ = true;
            raw_ = std::move(fromVal(0).raw_);
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

    BigInteger32& BigInteger32::pow(const BigInteger32& exp) {
        if (is_nan_) return *this;

        *this = powRecur(exp);
        return *this;
    }

    BigInteger32& BigInteger32::powMod(const BigInteger32& exp, const BigInteger32& rem) {
        if (is_nan_) return *this;

        *this = powModRecur(exp, rem);
        return *this;
    }

    BigInteger32& BigInteger32::abs() {
        if (is_nan_) return *this;
        if (!isMinus()) return *this;
        is_minus_ = false;
        return *this;
    }

    BigInteger32& BigInteger32::inv() {
        if (is_nan_) return *this;
        if (isZero()) return *this;
        is_minus_ = !is_minus_;
        return *this;
    }

    BigInteger32 BigInteger32::operator+(const BigInteger32& rhs) const {
        BigInteger32 tmp(*this);
        tmp.add(rhs);
        return tmp;
    }

    BigInteger32 BigInteger32::operator-(const BigInteger32& rhs) const {
        BigInteger32 tmp(*this);
        tmp.sub(rhs);
        return tmp;
    }

    BigInteger32 BigInteger32::operator*(const BigInteger32& rhs) const {
        BigInteger32 tmp(*this);
        tmp.mul(rhs);
        return tmp;
    }

    BigInteger32 BigInteger32::operator/(const BigInteger32& rhs) const {
        BigInteger32 tmp(*this);
        tmp.div(rhs);
        return tmp;
    }

    BigInteger32 BigInteger32::operator%(const BigInteger32& rhs) const {
        BigInteger32 tmp(*this);
        tmp.mod(rhs);
        return tmp;
    }

    bool BigInteger32::operator>(const BigInteger32& rhs) const {
        return compare(rhs) == 1;
    }

    bool BigInteger32::operator>=(const BigInteger32& rhs) const {
        return compare(rhs) >= 0;
    }

    bool BigInteger32::operator<(const BigInteger32& rhs) const {
        return compare(rhs) == -1;
    }

    bool BigInteger32::operator<=(const BigInteger32& rhs) const {
        return compare(rhs) <= 0;
    }

    bool BigInteger32::operator==(const BigInteger32& rhs) const {
        return compare(rhs) == 0;
    }

    int BigInteger32::compare(const BigInteger32& rhs) const {
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

    bool BigInteger32::isNaN() const {
        return is_nan_;
    }

    bool BigInteger32::isEven() const {
        return !(raw_.front() & 0x1);
    }

    bool BigInteger32::isZero() const {
        for (const auto& unit : raw_) {
            if (unit != 0) {
                return false;
            }
        }
        return true;
    }

    bool BigInteger32::isMinus() const {
        return is_minus_;
    }

    bool BigInteger32::isBeyondInt64() const {
        if (raw_.size() < sizeof(int64_t) / 4) {
            return false;
        }
        if (raw_.size() > sizeof(int64_t) / 4) {
            return true;
        }

        if (!is_minus_ && (raw_[1] & 0x80000000)) {
            return true;
        }
        if (is_minus_ && (raw_[1] & 0x80000000) && (raw_[0] || (raw_[1] &~0x80000000))) {
            return true;
        }
        return false;
    }

    int64_t BigInteger32::toInt64() const {
        return BigIntegerConverter().convertToInt64(*this);
    }

    string8 BigInteger32::toString() const {
        return BigIntegerConverter().convertToString(*this);
    }

    string8 BigInteger32::toStringHex() const {
        return BigIntegerConverter().convertToStringHex(*this);
    }

    void BigInteger32::addUs(BigInteger32& left, const BigInteger32& right) const {
        uint8_t over = 0;
        auto size = std::max(left.raw_.size(), right.raw_.size());
        for (size_t i = 0; i < size; ++i) {
            auto ret = toUInt32AddOver(uint64_t(left.getWithExt(i)) + right.getWithExt(i) + over, &over);
            if (left.raw_.size() < i + 1) {
                left.raw_.push_back(ret);
            } else {
                left.raw_[i] = ret;
            }
        }
    }

    void BigInteger32::subUsTL(BigInteger32& left, const BigInteger32& right) const {
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

    void BigInteger32::subUsTR(const BigInteger32& left, BigInteger32& right) const {
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

    void BigInteger32::mulUs(BigInteger32& left, const BigInteger32& right) const {
        std::vector<uint32_t> mid(left.raw_.size() + right.raw_.size(), 0);

        for (size_t i = 0; i < right.raw_.size(); ++i) {
            uint32_t over = 0;
            uint8_t mid_over = 0;
            auto r_ret = right.raw_[i];

            for (size_t j = 0; j < left.raw_.size(); ++j) {
                auto l_ret = left.raw_[j];
                auto ret = toUInt32MulOver(uint64_t(l_ret) * r_ret + over, &over);
                mid[j + i] = toUInt32AddOver(uint64_t(mid[j + i]) + ret + mid_over, &mid_over);
            }

            if (over > 0 || mid_over > 0) {
                mid[left.raw_.size() + i] += over + mid_over;
            }
        }

        left.raw_ = std::move(mid);
        left.shrink();
    }

    BigInteger32 BigInteger32::mulUs(const BigInteger32& left, const BigInteger32& right) const {
        auto tmp = left;
        mulUs(tmp, right);
        return tmp;
    }

    void BigInteger32::divModUs(BigInteger32& left, const BigInteger32& rhs, bool is_mod, BigInteger32* rem) const {
        std::vector<uint32_t> mid;

        BigInteger32 divid;
        size_t idx = raw_.size();
        if (raw_.back() == 0) {
            --idx;
        }

        bool first = true;
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

            uint64_t top = 0;
            if (beyond) {
                if (!is_next) {
                    if (divid.raw_.size() > 1 && rhs.raw_.size() > 1) {
                        top = divid.getBack2() / rhs.getBack2();
                    } else {
                        top = divid.raw_.back() / rhs.raw_.back();
                    }
                } else {
                    if (divid.raw_.size() > 2 && rhs.raw_.size() > 1) {
                        top = (divid.getBack3() / BigInteger16::fromU64(rhs.getBack2())).toInt64();
                    } else {
                        if (divid.raw_.size() > 1) {
                            top = std::min(divid.getBack2() / rhs.raw_.back(), uint64_t(kUInt32Max));
                        } else {
                            top = divid.raw_.back() / rhs.raw_.back();
                        }
                    }
                }
            }

            int count = 0;

            for (uint64_t i = top; i > 0; --i) {
                ++count;
                auto tmp = fromVal(i);

                BigInteger32 test = mulUs(rhs, tmp);
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

            if (top > 0 && count > 2) {
                LOG(Log::INFO) << "Div retry: " << count;
            }

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

    int BigInteger32::compareUs(const BigInteger32& left, const BigInteger32& right) const {
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

    void BigInteger32::shrink(bool all) {
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

    BigInteger32 BigInteger32::powRecur(const BigInteger32& exp) {
        BigInteger32 expl = exp;
        BigInteger32 result = fromVal(1);

        std::stack<int> stack;

        for (;;) {
            if (expl.isZero()) {
                break;
            }
            if (expl.isEven()) {
                expl.div(2);
                stack.push(1);
            } else {
                expl.sub(1).div(2);
                stack.push(2);
            }
        }

        while (!stack.empty()) {
            switch (stack.top()) {
            case 1:
                result = result * result;
                break;
            case 2:
                result = (result * result).mul(*this);
                break;
            default:
                break;
            }
            stack.pop();
        }
        return result;
    }

    BigInteger32 BigInteger32::powModRecur(const BigInteger32& exp, const BigInteger32& rem) {
        BigInteger32 expl = exp;
        BigInteger32 result = fromVal(1);

        std::stack<int> stack;

        while (true) {
            if (expl.isZero()) {
                break;
            }
            if (expl.isEven()) {
                expl.div(2);
                stack.push(1);
            } else {
                expl.sub(1).div(2);
                stack.push(2);
            }
        }

        while (!stack.empty()) {
            switch (stack.top()) {
            case 1:
                result = (result * result).mod(rem);
                break;
            case 2:
                result = (result * result).mul(*this).mod(rem);
                break;
            default:
                break;
            }
            stack.pop();
        }
        return result;
    }

    uint32_t BigInteger32::get(size_t idx) const {
        return raw_[idx];
    }

    uint32_t BigInteger32::getWithExt(size_t idx) const {
        if (idx >= raw_.size()) {
            return 0x00;
        }
        return get(idx);
    }

    uint64_t BigInteger32::getBack2() const {
        return uint64_t(raw_.back()) * kUInt32Rec + raw_[raw_.size() - 2];
    }

    BigInteger16 BigInteger32::getBack3() const {
        BigInteger16 result = BigInteger16::fromU64(uint64_t(raw_.back()));
        result.mul(BigInteger16::fromU64(kUInt32Rec));
        result.mul(BigInteger16::fromU64(kUInt32Rec));
        result.add(BigInteger16::fromU64(raw_[raw_.size() - 2] * kUInt32Rec));
        result.add(BigInteger16::fromU32(raw_[raw_.size() - 3]));
        return result;
    }

    uint32_t BigInteger32::toUInt32AddOver(uint64_t val, uint8_t* over) {
        if (val > kUInt32Max) {
            *over = 1;
        } else {
            *over = 0;
        }
        return static_cast<uint32_t>(val);
    }

    uint32_t BigInteger32::toUInt32MulOver(uint64_t val, uint32_t* over) {
        *over = val / kUInt32Rec;
        return static_cast<uint32_t>(val % kUInt32Rec);
    }

}
