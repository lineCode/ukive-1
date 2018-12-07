#include "ukive/graphics/vector2.h"

#include <cmath>


namespace ukive {

    Vector2::Vector2()
        :x(0), y(0) {
    }

    Vector2::Vector2(float x, float y)
        : x(x), y(y) {
    }


    bool Vector2::operator==(const Vector2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool Vector2::operator!=(const Vector2& rhs) const {
        return x != rhs.x || y != rhs.y;
    }

    Vector2 Vector2::operator+(const Vector2& rhs) const {
        return Vector2(x + rhs.x, y + rhs.y);
    }

    Vector2 Vector2::operator-(const Vector2& rhs) const {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    Vector2 Vector2::operator*(float value) const {
        return Vector2(x * value, y * value);
    }

    Vector2 Vector2::operator*(const Vector2& rhs) const {
        return Vector2(x * rhs.x, y * rhs.y);
    }

    Vector2 Vector2::operator/(float value) const {
        return Vector2(x / value, y / value);
    }

    void Vector2::normalize() {
        float l = length();
        x /= l;
        y /= l;
    }

    float Vector2::length() const {
        return std::sqrt(x * x + y * y);
    }
}