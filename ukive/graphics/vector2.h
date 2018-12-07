#ifndef UKIVE_GRAPHICS_VECTOR2_H_
#define UKIVE_GRAPHICS_VECTOR2_H_


namespace ukive {

    class Vector2 {
    public:
        Vector2();
        Vector2(float x, float y);

        bool operator==(const Vector2& rhs) const;
        bool operator!=(const Vector2& rhs) const;

        Vector2 operator+(const Vector2& rhs) const;
        Vector2 operator-(const Vector2& rhs) const;
        Vector2 operator*(float value) const;
        Vector2 operator*(const Vector2& rhs) const;
        Vector2 operator/(float value) const;

        void normalize();
        float length() const;

    public:
        float x, y;
    };

}

#endif  // UKIVE_GRAPHICS_VECTOR2_H_