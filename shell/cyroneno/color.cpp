#include "color.h"

#include <algorithm>


namespace cyro {

    namespace {

        uint8_t clampAdd(uint8_t u1, uint8_t u2) {
            return static_cast<uint8_t>(
                std::min(255U, static_cast<uint32_t>(u1) + static_cast<uint32_t>(u2)));
        }

        uint8_t clampSub(uint8_t u1, uint8_t u2) {
            return static_cast<uint8_t>(
                std::max(0, static_cast<int32_t>(u1) - static_cast<int32_t>(u2)));
        }

    }

    //////////
    Color::Color()
        : r_(0), g_(0), b_(0), a_(1.f) {
    }

    Color::Color(float r, float g, float b, float a)
        : r_(r), g_(g), b_(b), a_(a) {
    }

    Color::Color(const Color& rhs)
        : r_(rhs.r_), g_(rhs.g_), b_(rhs.b_), a_(rhs.a_) {
    }

    Color& Color::operator=(const Color& rhs) {
        r_ = rhs.r_;
        g_ = rhs.g_;
        b_ = rhs.b_;
        a_ = rhs.a_;
        return *this;
    }

    Color Color::operator+(const Color& rhs) const {
        Color c(*this);
        c.add(rhs);
        return c;
    }

    Color Color::operator-(const Color& rhs) const {
        Color c(*this);
        c.sub(rhs);
        return c;
    }

    Color Color::operator*(float factor) const {
        Color c(*this);
        c.mul(factor);
        return c;
    }

    Color Color::operator/(float factor) const {
        Color c(*this);
        c.div(factor);
        return c;
    }

    Color& Color::operator+=(const Color& rhs) {
        add(rhs);
        return *this;
    }

    Color& Color::operator-=(const Color& rhs) {
        sub(rhs);
        return *this;
    }

    Color& Color::operator*=(float factor) {
        mul(factor);
        return *this;
    }

    Color& Color::operator/=(float factor) {
        div(factor);
        return *this;
    }

    void Color::add(const Color& rhs) {
        r_ = std::min(1.f, r_ + rhs.r_);
        g_ = std::min(1.f, g_ + rhs.g_);
        b_ = std::min(1.f, b_ + rhs.b_);
        a_ = std::min(1.f, a_ + rhs.a_);
    }

    void Color::sub(const Color& rhs) {
        r_ = std::max(0.f, r_ - rhs.r_);
        g_ = std::max(0.f, g_ - rhs.g_);
        b_ = std::max(0.f, b_ - rhs.b_);
        a_ = std::max(0.f, a_ - rhs.a_);
    }

    void Color::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r_ *= factor;
        g_ *= factor;
        b_ *= factor;
        a_ *= factor;
    }

    void Color::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r_ /= factor;
        g_ /= factor;
        b_ /= factor;
        a_ /= factor;
    }


    //////////
    ColorRGB::ColorRGB()
        : r_(0), g_(0), b_(0) {}

    ColorRGB::ColorRGB(float r, float g, float b)
        : r_(r), g_(g), b_(b) {}

    ColorRGB::ColorRGB(const ColorRGB& rhs)
        : r_(rhs.r_), g_(rhs.g_), b_(rhs.b_) {}

    ColorRGB& ColorRGB::operator=(const ColorRGB& rhs) {
        r_ = rhs.r_;
        g_ = rhs.g_;
        b_ = rhs.b_;
        return *this;
    }

    ColorRGB ColorRGB::operator+(const ColorRGB& rhs) const {
        ColorRGB c(*this);
        c.add(rhs);
        return c;
    }

    ColorRGB ColorRGB::operator-(const ColorRGB& rhs) const {
        ColorRGB c(*this);
        c.sub(rhs);
        return c;
    }

    ColorRGB ColorRGB::operator*(float factor) const {
        ColorRGB c(*this);
        c.mul(factor);
        return c;
    }

    ColorRGB ColorRGB::operator/(float factor) const {
        ColorRGB c(*this);
        c.div(factor);
        return c;
    }

    ColorRGB& ColorRGB::operator+=(const ColorRGB& rhs) {
        add(rhs);
        return *this;
    }

    ColorRGB& ColorRGB::operator-=(const ColorRGB& rhs) {
        sub(rhs);
        return *this;
    }

    ColorRGB& ColorRGB::operator*=(float factor) {
        mul(factor);
        return *this;
    }

    ColorRGB& ColorRGB::operator/=(float factor) {
        div(factor);
        return *this;
    }

    void ColorRGB::add(const ColorRGB& rhs) {
        r_ = std::min(1.f, r_ + rhs.r_);
        g_ = std::min(1.f, g_ + rhs.g_);
        b_ = std::min(1.f, b_ + rhs.b_);
    }

    void ColorRGB::sub(const ColorRGB& rhs) {
        r_ = std::max(0.f, r_ - rhs.r_);
        g_ = std::max(0.f, g_ - rhs.g_);
        b_ = std::max(0.f, b_ - rhs.b_);
    }

    void ColorRGB::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r_ *= factor;
        g_ *= factor;
        b_ *= factor;
    }

    void ColorRGB::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r_ /= factor;
        g_ /= factor;
        b_ /= factor;
    }


    //////////
    ColorBGRAInt::ColorBGRAInt()
        : b_(0), g_(0), r_(0), a_(255) {
    }

    ColorBGRAInt::ColorBGRAInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : b_(b), g_(g), r_(r), a_(a) {
    }

    ColorBGRAInt::ColorBGRAInt(const ColorBGRAInt& rhs)
        : b_(rhs.b_), g_(rhs.g_), r_(rhs.r_), a_(rhs.a_) {
    }

    ColorBGRAInt& ColorBGRAInt::operator=(const ColorBGRAInt& rhs) {
        b_ = rhs.b_;
        g_ = rhs.g_;
        r_ = rhs.r_;
        a_ = rhs.a_;
        return *this;
    }

    ColorBGRAInt ColorBGRAInt::operator+(const ColorBGRAInt& rhs) const {
        ColorBGRAInt c(*this);
        c.add(rhs);
        return c;
    }

    ColorBGRAInt ColorBGRAInt::operator-(const ColorBGRAInt& rhs) const {
        ColorBGRAInt c(*this);
        c.sub(rhs);
        return c;
    }

    ColorBGRAInt ColorBGRAInt::operator*(float factor) const {
        ColorBGRAInt c(*this);
        c.mul(factor);
        return c;
    }

    ColorBGRAInt ColorBGRAInt::operator*(const Color& rhs) const {
        ColorBGRAInt c(*this);
        c.mul(rhs);
        return c;
    }

    ColorBGRAInt ColorBGRAInt::operator/(float factor) const {
        ColorBGRAInt c(*this);
        c.div(factor);
        return c;
    }

    ColorBGRAInt& ColorBGRAInt::operator+=(const ColorBGRAInt& rhs) {
        add(rhs);
        return *this;
    }

    ColorBGRAInt& ColorBGRAInt::operator-=(const ColorBGRAInt& rhs) {
        sub(rhs);
        return *this;
    }

    ColorBGRAInt& ColorBGRAInt::operator*=(float factor) {
        mul(factor);
        return *this;
    }

    ColorBGRAInt& ColorBGRAInt::operator*=(const Color& rhs) {
        mul(rhs);
        return *this;
    }

    ColorBGRAInt& ColorBGRAInt::operator/=(float factor) {
        div(factor);
        return *this;
    }

    void ColorBGRAInt::add(const ColorBGRAInt& rhs) {
        b_ = clampAdd(b_, rhs.b_);
        g_ = clampAdd(g_, rhs.g_);
        r_ = clampAdd(r_, rhs.r_);
        a_ = clampAdd(a_, rhs.a_);
    }

    void ColorBGRAInt::sub(const ColorBGRAInt& rhs) {
        b_ = clampSub(b_, rhs.b_);
        g_ = clampSub(g_, rhs.g_);
        r_ = clampSub(r_, rhs.r_);
        a_ = clampSub(a_, rhs.a_);
    }

    void ColorBGRAInt::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b_ *= factor;
        g_ *= factor;
        r_ *= factor;
        a_ *= factor;
    }

    void ColorBGRAInt::mul(const Color& rhs) {
        b_ *= rhs.b_;
        g_ *= rhs.g_;
        r_ *= rhs.r_;
        a_ *= rhs.a_;
    }

    void ColorBGRAInt::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b_ /= factor;
        g_ /= factor;
        r_ /= factor;
        a_ /= factor;
    }


    //////////
    ColorBGRInt::ColorBGRInt()
        : b_(0), g_(0), r_(0) {}

    ColorBGRInt::ColorBGRInt(uint8_t r, uint8_t g, uint8_t b)
        : b_(b), g_(g), r_(r) {}

    ColorBGRInt::ColorBGRInt(const ColorBGRInt& rhs)
        : b_(rhs.b_), g_(rhs.g_), r_(rhs.r_) {}

    ColorBGRInt& ColorBGRInt::operator=(const ColorBGRInt& rhs) {
        b_ = rhs.b_;
        g_ = rhs.g_;
        r_ = rhs.r_;
        return *this;
    }

    ColorBGRInt ColorBGRInt::operator+(const ColorBGRInt& rhs) const {
        ColorBGRInt c(*this);
        c.add(rhs);
        return c;
    }

    ColorBGRInt ColorBGRInt::operator-(const ColorBGRInt& rhs) const {
        ColorBGRInt c(*this);
        c.sub(rhs);
        return c;
    }

    ColorBGRInt ColorBGRInt::operator*(float factor) const {
        ColorBGRInt c(*this);
        c.mul(factor);
        return c;
    }

    ColorBGRInt ColorBGRInt::operator*(const ColorRGB& rhs) const {
        ColorBGRInt c(*this);
        c.mul(rhs);
        return c;
    }

    ColorBGRInt ColorBGRInt::operator/(float factor) const {
        ColorBGRInt c(*this);
        c.div(factor);
        return c;
    }

    ColorBGRInt& ColorBGRInt::operator+=(const ColorBGRInt& rhs) {
        add(rhs);
        return *this;
    }

    ColorBGRInt& ColorBGRInt::operator-=(const ColorBGRInt& rhs) {
        sub(rhs);
        return *this;
    }

    ColorBGRInt& ColorBGRInt::operator*=(float factor) {
        mul(factor);
        return *this;
    }

    ColorBGRInt& ColorBGRInt::operator*=(const ColorRGB& rhs) {
        mul(rhs);
        return *this;
    }

    ColorBGRInt& ColorBGRInt::operator/=(float factor) {
        div(factor);
        return *this;
    }

    void ColorBGRInt::add(const ColorBGRInt& rhs) {
        b_ = clampAdd(b_, rhs.b_);
        g_ = clampAdd(g_, rhs.g_);
        r_ = clampAdd(r_, rhs.r_);
    }

    void ColorBGRInt::sub(const ColorBGRInt& rhs) {
        b_ = clampSub(b_, rhs.b_);
        g_ = clampSub(g_, rhs.g_);
        r_ = clampSub(r_, rhs.r_);
    }

    void ColorBGRInt::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b_ *= factor;
        g_ *= factor;
        r_ *= factor;
    }

    void ColorBGRInt::mul(const ColorRGB& rhs) {
        b_ *= rhs.b_;
        g_ *= rhs.g_;
        r_ *= rhs.r_;
    }

    void ColorBGRInt::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b_ /= factor;
        g_ /= factor;
        r_ /= factor;
    }

    ColorBGRAInt ColorBGRInt::toBGRAInt(uint8_t a) const {
        return ColorBGRAInt(r_, g_, b_, a);
    }

}