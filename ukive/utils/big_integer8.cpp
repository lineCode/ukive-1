#include "ukive/utils/big_integer8.h"

#include <algorithm>
#include <stack>

#include "ukive/utils/big_integer_converter.h"
#include "ukive/log.h"


namespace ukive {

    BigInteger8::BigInteger8()
        : is_nan_(false) {}

    // static
    BigInteger8 BigInteger8::fromVal(int64_t val) {
        BigInteger8 big_integer;
        big_integer.raw_.assign(8, 0);
        for (int i = 0; i < 8; ++i) {
            big_integer.raw_[i] = static_cast<uint8_t>(val >> (i * 8));
        }
        big_integer.shrink();
        return big_integer;
    }

    BigInteger8& BigInteger8::add(const BigInteger8& rhs) {
        if (is_nan_) return *this;

        uint8_t over = 0;
        bool minus = isMinus();
        auto size = std::max(raw_.size(), rhs.raw_.size());
        for (size_t i = 0; i < size; ++i) {
            auto ret = toUInt8AddOver(uint16_t(getWithExt(i, minus)) + rhs.getWithExt(i) + over, &over);
            if (raw_.size() < i + 1) {
                raw_.push_back(ret);
            }  else {
                raw_[i] = ret;
            }
        }

        if (!(minus ^ rhs.isMinus()) && minus != isMinus()) {
            raw_.push_back(minus ? 0xFF : 0);
        }

        shrink();
        return *this;
    }

    BigInteger8& BigInteger8::sub(const BigInteger8& rhs) {
        if (is_nan_) return *this;

        uint8_t over = 0;
        uint8_t r_over = 0;
        bool minus = isMinus();
        auto size = std::max(raw_.size(), rhs.raw_.size());

        for (size_t i = 0; i < size; ++i) {
            auto r_ret = toUInt8AddOver(uint16_t(rhs.getInvWithExt(i)) + r_over + (i == 0 ? 1 : 0), &r_over);
            auto ret = toUInt8AddOver(uint16_t(getWithExt(i, minus)) + r_ret + over, &over);
            if (raw_.size() < i + 1) {
                raw_.push_back(ret);
            } else {
                raw_[i] = ret;
            }
        }

        if (minus ^ rhs.isMinus() && minus != isMinus()) {
            raw_.push_back(minus ? 0xFF : 0);
        }

        shrink();
        return *this;
    }

    BigInteger8& BigInteger8::mul(const BigInteger8& rhs) {
        if (is_nan_) return *this;

        std::vector<uint8_t> mid(raw_.size() + rhs.raw_.size(), 0);

        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();
        uint8_t r_over = 0;

        for (size_t i = 0; i < rhs.raw_.size(); ++i) {
            uint8_t over = 0;
            uint8_t mid_over = 0;
            uint8_t l_over = 0;

            uint8_t r_ret = is_r_minus ?
                toUInt8AddOver(uint16_t(rhs.getInvWithExt(i)) + r_over + (i == 0 ? 1 : 0), &r_over) :
                rhs.getWithExt(i);

            for (size_t j = 0; j < raw_.size(); ++j) {
                uint8_t l_ret = is_l_minus ?
                    toUInt8AddOver(uint16_t(getInvWithExt(j)) + l_over + (j == 0 ? 1 : 0), &l_over) :
                    getWithExt(j);

                auto ret = toUInt8MulOver(uint16_t(l_ret) * r_ret + over, &over);
                mid[j + i] = toUInt8AddOver(uint16_t(mid[j + i]) + ret + mid_over, &mid_over);
            }

            if (over > 0 || mid_over > 0) {
                mid[raw_.size() + i] += over + mid_over;
            }
        }

        raw_ = std::move(mid);

        shrink();
        if (is_l_minus ^ is_r_minus) {
            inv();
        }
        return *this;
    }

    BigInteger8& BigInteger8::div(const BigInteger8& rhs, BigInteger8* rem) {
        if (is_nan_) return *this;

        if (rhs.isZero()) {
            is_nan_ = true;
            raw_ = std::move(fromVal(0).raw_);
            return *this;
        }

        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();

        if (is_l_minus) {
            abs();
        }

        if (is_r_minus) {
            BigInteger8 r_tmp(rhs);
            r_tmp.abs();
            if (r_tmp.raw_.back() == 0) {
                r_tmp.raw_.pop_back();
            }

            divModInternal(r_tmp, false, rem);
        } else if (rhs.raw_.back() == 0) {
            BigInteger8 r_tmp(rhs);
            r_tmp.raw_.pop_back();

            divModInternal(r_tmp, false, rem);
        } else {
            divModInternal(rhs, false, rem);
        }

        raw_.push_back(0);
        shrink();

        if (is_l_minus ^ is_r_minus) {
            inv();
        }

        if (rem) {
            rem->raw_.push_back(0);
            if (is_l_minus) {
                rem->inv();
            }
            rem->shrink();
        }

        return *this;
    }

    BigInteger8& BigInteger8::mod(const BigInteger8& rhs) {
        if (is_nan_) return *this;

        if (rhs.isZero()) {
            is_nan_ = true;
            raw_ = std::move(fromVal(0).raw_);
            return *this;
        }

        bool is_l_minus = isMinus();
        bool is_r_minus = rhs.isMinus();

        if (is_l_minus) {
            abs();
        }

        if (is_r_minus) {
            BigInteger8 r_tmp(rhs);
            r_tmp.abs();
            if (r_tmp.raw_.back() == 0) {
                r_tmp.raw_.pop_back();
            }

            divModInternal(r_tmp, true);
        } else if (rhs.raw_.back() == 0) {
            BigInteger8 r_tmp(rhs);
            r_tmp.raw_.pop_back();

            divModInternal(r_tmp, true);
        } else {
            divModInternal(rhs, true);
        }

        raw_.push_back(0);
        shrink();

        if (is_l_minus) {
            inv();
        }
        return *this;
    }

    BigInteger8& BigInteger8::pow(const BigInteger8& exp) {
        if (is_nan_) return *this;

        *this = powRecur(exp);
        return *this;
    }

    BigInteger8& BigInteger8::powMod(const BigInteger8& exp, const BigInteger8& rem) {
        if (is_nan_) return *this;

        *this = powModRecur(exp, rem);
        return *this;
    }

    BigInteger8& BigInteger8::abs() {
        if (is_nan_) return *this;
        if (!isMinus()) return *this;

        uint8_t r_over = 0;
        for (size_t i = 0; i < raw_.size(); ++i) {
            raw_[i] = toUInt8AddOver(uint16_t(getInv(i)) + r_over + (i == 0 ? 1 : 0), &r_over);
        }

        // 绝对值取完后还是负数，说明溢出
        if (isMinus()) {
            raw_.push_back(0);
        }
        return *this;
    }

    BigInteger8& BigInteger8::inv() {
        if (is_nan_) return *this;

        bool is_minus = isMinus();

        uint8_t r_over = 0;
        for (size_t i = 0; i < raw_.size(); ++i) {
            raw_[i] = toUInt8AddOver(uint16_t(getInv(i)) + r_over + (i == 0 ? 1 : 0), &r_over);
        }

        // 负数取完相反数后还是负数，说明溢出
        if (is_minus && isMinus()) {
            raw_.push_back(0);
        }

        // 正数取完相反数后需要缩一下
        if (!is_minus) {
            shrink();
        }
        return *this;
    }

    BigInteger8 BigInteger8::operator+(const BigInteger8& rhs) const {
        BigInteger8 tmp(*this);
        tmp.add(rhs);
        return tmp;
    }

    BigInteger8 BigInteger8::operator-(const BigInteger8& rhs) const {
        BigInteger8 tmp(*this);
        tmp.sub(rhs);
        return tmp;
    }

    BigInteger8 BigInteger8::operator*(const BigInteger8& rhs) const {
        BigInteger8 tmp(*this);
        tmp.mul(rhs);
        return tmp;
    }

    BigInteger8 BigInteger8::operator/(const BigInteger8& rhs) const {
        BigInteger8 tmp(*this);
        tmp.div(rhs);
        return tmp;
    }

    BigInteger8 BigInteger8::operator%(const BigInteger8& rhs) const {
        BigInteger8 tmp(*this);
        tmp.mod(rhs);
        return tmp;
    }

    bool BigInteger8::operator>(const BigInteger8& rhs) const {
        return compare(rhs) == 1;
    }

    bool BigInteger8::operator>=(const BigInteger8& rhs) const {
        return compare(rhs) >= 0;
    }

    bool BigInteger8::operator<(const BigInteger8& rhs) const {
        return compare(rhs) == -1;
    }

    bool BigInteger8::operator<=(const BigInteger8& rhs) const {
        return compare(rhs) <= 0;
    }

    bool BigInteger8::operator==(const BigInteger8& rhs) const {
        return compare(rhs) == 0;
    }

    int BigInteger8::compare(const BigInteger8& rhs) const {
        BigInteger8 big_integer(*this);
        big_integer.sub(rhs);
        if (big_integer.isZero()) {
            return 0;
        }
        if (big_integer.isMinus()) {
            return -1;
        }
        return 1;
    }

    bool BigInteger8::isNaN() const {
        return is_nan_;
    }

    bool BigInteger8::isEven() const {
        return !(raw_.front() & 0x1);
    }

    bool BigInteger8::isZero() const {
        for (const auto& u8 : raw_) {
            if (u8 != 0) {
                return false;
            }
        }
        return true;
    }

    bool BigInteger8::isMinus() const {
        return raw_.back() & 0x80;
    }

    bool BigInteger8::isBeyondInt64() const {
        return raw_.size() > sizeof(int64_t);
    }

    int64_t BigInteger8::toInt64() const {
        return BigIntegerConverter().convertToInt64(*this);
    }

    string8 BigInteger8::toString() const {
        return BigIntegerConverter().convertToString(*this);
    }

    string8 BigInteger8::toStringHex() const {
        return BigIntegerConverter().convertToStringHex(*this);
    }

    int BigInteger8::compareUs(const BigInteger8& left, const BigInteger8& right) {
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

    BigInteger8 BigInteger8::subUs(const BigInteger8& left, const BigInteger8& right) {
        uint8_t over = 0;
        uint8_t r_over = 0;
        auto size = std::max(left.raw_.size(), right.raw_.size());

        BigInteger8 result;
        result.raw_.resize(size);

        for (size_t i = 0; i < size; ++i) {
            auto r_ret = toUInt8AddOver(uint16_t(right.getInvWithExt(i, false)) + r_over + (i == 0 ? 1 : 0), &r_over);
            result.raw_[i] = toUInt8AddOver(uint16_t(left.getWithExt(i, false)) + r_ret + over, &over);
        }

        result.elimZero(true);
        return result;
    }

    BigInteger8 BigInteger8::mulUs(const BigInteger8& left, const BigInteger8& right) {
        std::vector<uint8_t> mid(left.raw_.size() + right.raw_.size(), 0);

        for (size_t i = 0; i < right.raw_.size(); ++i) {
            uint8_t over = 0;
            uint8_t mid_over = 0;

            uint8_t r_ret = right.raw_[i];

            for (size_t j = 0; j < left.raw_.size(); ++j) {
                uint8_t l_ret = left.raw_[j];

                auto ret = toUInt8MulOver(uint16_t(l_ret) * r_ret + over, &over);
                mid[j + i] = toUInt8AddOver(uint16_t(mid[j + i]) + ret + mid_over, &mid_over);
            }

            if (over > 0 || mid_over > 0) {
                mid[left.raw_.size() + i] += over + mid_over;
            }
        }

        BigInteger8 result;
        result.raw_ = std::move(mid);
        result.elimZero(false);
        return result;
    }

    void BigInteger8::shrink() {
        size_t idx = raw_.size();
        if (idx == 0) return;

        if (raw_.back() == 0) {
            for (size_t i = raw_.size(); i > 1; --i) {
                auto tmp = raw_[i - 1];
                if (tmp == 0 && !(raw_[i - 2] & 0x80)) {
                    idx = i - 1;
                } else {
                    break;
                }
            }
        } else if (raw_.back() == 0xFF) {
            for (size_t i = raw_.size(); i > 1; --i) {
                auto tmp = raw_[i - 1];
                if (tmp == 0xFF && (raw_[i - 2] & 0x80)) {
                    idx = i - 1;
                } else {
                    break;
                }
            }
        }

        if (idx < raw_.size()) {
            raw_.erase(raw_.begin() + idx, raw_.end());
        }
    }

    void BigInteger8::elimZero(bool all) {
        size_t lim = all ? 0 : 1;

        size_t idx = raw_.size();
        for (size_t i = raw_.size(); i > lim; --i) {
            if (raw_[i - 1] == 0) {
                idx = i - 1;
            } else {
                break;
            }
        }
        raw_.erase(raw_.begin() + idx, raw_.end());
    }

    void BigInteger8::divModInternal(const BigInteger8& rhs, bool is_mod, BigInteger8* rem) {
        std::vector<uint8_t> mid;

        BigInteger8 divid;
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
            divid.elimZero(false);
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
                        auto d2 = divid.raw_.back() * 256 + divid.raw_[divid.raw_.size() - 2];
                        auto t2 = rhs.raw_.back() * 256 + rhs.raw_[rhs.raw_.size() - 2];
                        top = d2 / t2;
                    } else {
                        top = divid.raw_.back() / rhs.raw_.back();
                    }
                } else {
                    if (divid.raw_.size() > 2 && rhs.raw_.size() > 1) {
                        auto d2 = uint32_t(divid.raw_.back()) * 256 * 256 + divid.raw_[divid.raw_.size() - 2] * 256 + divid.raw_[divid.raw_.size() - 3];
                        auto t2 = rhs.raw_.back() * 256 + rhs.raw_[rhs.raw_.size() - 2];
                        top = d2 / t2;
                    } else {
                        if (divid.raw_.size() > 1) {
                            auto t2 = divid.raw_.back() * 256 + divid.raw_[divid.raw_.size() - 2];
                            top = std::min(t2 / rhs.raw_.back(), 255);
                        } else {
                            top = divid.raw_.back() / rhs.raw_.back();
                        }
                    }
                }
            }

            int count = 0;

            for (uint8_t i = top; i > 0; --i) {
                ++count;
                auto tmp = fromVal(i);
                tmp.elimZero(false);

                BigInteger8 test = mulUs(rhs, tmp);
                if (compareUs(test, divid) <= 0) {
                    if (!is_mod) {
                        mid.insert(mid.begin(), i);
                    }

                    if (beyond) {
                        divid = subUs(divid, test);
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
            raw_ = std::move(divid.raw_);
        } else {
            if (rem) {
                *rem = std::move(divid);
            }
            raw_ = std::move(mid);
        }

        if (raw_.empty()) {
            raw_.push_back(0);
        }
    }

    BigInteger8 BigInteger8::powRecur(const BigInteger8& exp) {
        BigInteger8 expl = exp;
        BigInteger8 result = fromVal(1);

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

    BigInteger8 BigInteger8::powModRecur(const BigInteger8& exp, const BigInteger8& rem) {
        BigInteger8 expl = exp;
        BigInteger8 result = fromVal(1);

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

    uint8_t BigInteger8::get(size_t idx) const {
        return raw_[idx];
    }

    uint8_t BigInteger8::getInv(size_t idx) const {
        return 255 - raw_[idx];
    }

    uint8_t BigInteger8::getWithExt(size_t idx) const {
        return getWithExt(idx, isMinus());
    }

    uint8_t BigInteger8::getWithExt(size_t idx, bool minus) const {
        if (idx >= raw_.size()) {
            return minus ? 0xFF : 0x00;
        }
        return get(idx);
    }

    uint8_t BigInteger8::getInvWithExt(size_t idx) const {
        return getInvWithExt(idx, isMinus());
    }

    uint8_t BigInteger8::getInvWithExt(size_t idx, bool minus) const {
        uint8_t ret;
        if (idx >= raw_.size()) {
            ret = minus ? 0xFF : 0x00;
        } else {
            ret = get(idx);
        }
        return 255 - ret;
    }

    uint8_t BigInteger8::getBackNZ() const {
        size_t idx = raw_.size() - 1;
        auto tmp = raw_[idx];
        if (tmp == 0) {
            if (idx == 0) {
                return 0;
            }
            --idx;
        }
        return raw_[idx];
    }

    uint16_t BigInteger8::getBackNZ2() const {
        size_t idx = raw_.size() - 1;
        auto tmp = raw_[idx];
        if (tmp == 0) {
            if (idx == 0) {
                return 0;
            }
            --idx;
        }
        if (idx == 0) {
            return raw_[idx];
        }
        return raw_[idx] * 256 + raw_[idx - 1];
    }

    uint8_t BigInteger8::toUInt8AddOver(uint16_t val, uint8_t* over) {
        if (val > 255) {
            *over = 1;
        } else {
            *over = 0;
        }
        return static_cast<uint8_t>(val);
    }

    uint8_t BigInteger8::toUInt8MulOver(uint16_t val, uint8_t* over) {
        *over = val / 256;
        return static_cast<uint8_t>(val % 256);
    }

}
