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
        : r(0), g(0), b(0), a(1.f) {
    }

    Color::Color(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a) {
    }

    Color::Color(const Color& rhs)
        : r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {
    }

    Color& Color::operator=(const Color& rhs) {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
        a = rhs.a;
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
        r = std::min(1.f, r + rhs.r);
        g = std::min(1.f, g + rhs.g);
        b = std::min(1.f, b + rhs.b);
        a = std::min(1.f, a + rhs.a);
    }

    void Color::sub(const Color& rhs) {
        r = std::max(0.f, r - rhs.r);
        g = std::max(0.f, g - rhs.g);
        b = std::max(0.f, b - rhs.b);
        a = std::max(0.f, a - rhs.a);
    }

    void Color::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r *= factor;
        g *= factor;
        b *= factor;
        a *= factor;
    }

    void Color::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r /= factor;
        g /= factor;
        b /= factor;
        a /= factor;
    }


    //////////
    ColorRGB::ColorRGB()
        : r(0), g(0), b(0) {}

    ColorRGB::ColorRGB(float r, float g, float b)
        : r(r), g(g), b(b) {}

    ColorRGB::ColorRGB(const ColorRGB& rhs)
        : r(rhs.r), g(rhs.g), b(rhs.b) {}

    ColorRGB& ColorRGB::operator=(const ColorRGB& rhs) {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
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
        r = std::min(1.f, r + rhs.r);
        g = std::min(1.f, g + rhs.g);
        b = std::min(1.f, b + rhs.b);
    }

    void ColorRGB::sub(const ColorRGB& rhs) {
        r = std::max(0.f, r - rhs.r);
        g = std::max(0.f, g - rhs.g);
        b = std::max(0.f, b - rhs.b);
    }

    void ColorRGB::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r *= factor;
        g *= factor;
        b *= factor;
    }

    void ColorRGB::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        r /= factor;
        g /= factor;
        b /= factor;
    }


    //////////
    ColorBGRAInt::ColorBGRAInt()
        : b(0), g(0), r(0), a(255) {
    }

    ColorBGRAInt::ColorBGRAInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : b(b), g(g), r(r), a(a) {
    }

    ColorBGRAInt::ColorBGRAInt(const ColorBGRAInt& rhs)
        : b(rhs.b), g(rhs.g), r(rhs.r), a(rhs.a) {
    }

    ColorBGRAInt& ColorBGRAInt::operator=(const ColorBGRAInt& rhs) {
        b = rhs.b;
        g = rhs.g;
        r = rhs.r;
        a = rhs.a;
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
        b = clampAdd(b, rhs.b);
        g = clampAdd(g, rhs.g);
        r = clampAdd(r, rhs.r);
        a = clampAdd(a, rhs.a);
    }

    void ColorBGRAInt::sub(const ColorBGRAInt& rhs) {
        b = clampSub(b, rhs.b);
        g = clampSub(g, rhs.g);
        r = clampSub(r, rhs.r);
        a = clampSub(a, rhs.a);
    }

    void ColorBGRAInt::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b = uint8_t(b*factor);
        g = uint8_t(g*factor);
        r = uint8_t(r*factor);
        a = uint8_t(a*factor);
    }

    void ColorBGRAInt::mul(const Color& rhs) {
        b = uint8_t(b*rhs.b);
        g = uint8_t(g*rhs.g);
        r = uint8_t(r*rhs.r);
        a = uint8_t(a*rhs.a);
    }

    void ColorBGRAInt::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b = uint8_t(b/factor);
        g = uint8_t(g/factor);
        r = uint8_t(r/factor);
        a = uint8_t(a/factor);
    }


    //////////
    ColorBGRInt::ColorBGRInt()
        : b(0), g(0), r(0) {}

    ColorBGRInt::ColorBGRInt(uint8_t r, uint8_t g, uint8_t b)
        : b(b), g(g), r(r) {}

    ColorBGRInt::ColorBGRInt(const ColorBGRInt& rhs)
        : b(rhs.b), g(rhs.g), r(rhs.r) {}

    ColorBGRInt& ColorBGRInt::operator=(const ColorBGRInt& rhs) {
        b = rhs.b;
        g = rhs.g;
        r = rhs.r;
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
        b = clampAdd(b, rhs.b);
        g = clampAdd(g, rhs.g);
        r = clampAdd(r, rhs.r);
    }

    void ColorBGRInt::sub(const ColorBGRInt& rhs) {
        b = clampSub(b, rhs.b);
        g = clampSub(g, rhs.g);
        r = clampSub(r, rhs.r);
    }

    void ColorBGRInt::mul(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b = uint8_t(b*factor);
        g = uint8_t(g*factor);
        r = uint8_t(r*factor);
    }

    void ColorBGRInt::mul(const ColorRGB& rhs) {
        b = uint8_t(b*rhs.b);
        g = uint8_t(g*rhs.g);
        r = uint8_t(r*rhs.r);
    }

    void ColorBGRInt::div(float factor) {
        factor = std::max(0.f, factor);
        factor = std::min(1.f, factor);

        b = uint8_t(b/factor);
        g = uint8_t(g/factor);
        r = uint8_t(r/factor);
    }

    ColorBGRAInt ColorBGRInt::toBGRAInt(uint8_t a) const {
        return ColorBGRAInt(r, g, b, a);
    }

}