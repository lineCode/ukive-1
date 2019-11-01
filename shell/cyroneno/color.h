#ifndef CYRONENO_COLOR_H_
#define CYRONENO_COLOR_H_

#include <cstdint>


namespace cyro {

    class Color {
    public:
        Color();
        Color(float r, float g, float b, float a);
        Color(const Color& rhs);

        Color& operator=(const Color& rhs);

        Color operator+(const Color& rhs) const;
        Color operator-(const Color& rhs) const;
        Color operator*(float factor) const;
        Color operator/(float factor) const;

        Color& operator+=(const Color& rhs);
        Color& operator-=(const Color& rhs);
        Color& operator*=(float factor);
        Color& operator/=(float factor);

        void add(const Color& rhs);
        void sub(const Color& rhs);
        void mul(float factor);
        void div(float factor);

        float r_, g_, b_, a_;
    };


    class ColorRGB {
    public:
        ColorRGB();
        ColorRGB(float r, float g, float b);
        ColorRGB(const ColorRGB& rhs);

        ColorRGB& operator=(const ColorRGB& rhs);

        ColorRGB operator+(const ColorRGB& rhs) const;
        ColorRGB operator-(const ColorRGB& rhs) const;
        ColorRGB operator*(float factor) const;
        ColorRGB operator/(float factor) const;

        ColorRGB& operator+=(const ColorRGB& rhs);
        ColorRGB& operator-=(const ColorRGB& rhs);
        ColorRGB& operator*=(float factor);
        ColorRGB& operator/=(float factor);

        void add(const ColorRGB& rhs);
        void sub(const ColorRGB& rhs);
        void mul(float factor);
        void div(float factor);

        float r_, g_, b_;
    };


    class ColorBGRAInt {
    public:
        ColorBGRAInt();
        ColorBGRAInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        ColorBGRAInt(const ColorBGRAInt& rhs);

        ColorBGRAInt& operator=(const ColorBGRAInt& rhs);

        ColorBGRAInt operator+(const ColorBGRAInt& rhs) const;
        ColorBGRAInt operator-(const ColorBGRAInt& rhs) const;
        ColorBGRAInt operator*(float factor) const;
        ColorBGRAInt operator*(const Color& rhs) const;
        ColorBGRAInt operator/(float factor) const;

        ColorBGRAInt& operator+=(const ColorBGRAInt& rhs);
        ColorBGRAInt& operator-=(const ColorBGRAInt& rhs);
        ColorBGRAInt& operator*=(float factor);
        ColorBGRAInt& operator*=(const Color& rhs);
        ColorBGRAInt& operator/=(float factor);

        void add(const ColorBGRAInt& rhs);
        void sub(const ColorBGRAInt& rhs);
        void mul(float factor);
        void mul(const Color& rhs);
        void div(float factor);

        uint8_t b_, g_, r_, a_;
    };


    class ColorBGRInt {
    public:
        ColorBGRInt();
        ColorBGRInt(uint8_t r, uint8_t g, uint8_t b);
        ColorBGRInt(const ColorBGRInt& rhs);

        ColorBGRInt& operator=(const ColorBGRInt& rhs);

        ColorBGRInt operator+(const ColorBGRInt& rhs) const;
        ColorBGRInt operator-(const ColorBGRInt& rhs) const;
        ColorBGRInt operator*(float factor) const;
        ColorBGRInt operator*(const ColorRGB& rhs) const;
        ColorBGRInt operator/(float factor) const;

        ColorBGRInt& operator+=(const ColorBGRInt& rhs);
        ColorBGRInt& operator-=(const ColorBGRInt& rhs);
        ColorBGRInt& operator*=(float factor);
        ColorBGRInt& operator*=(const ColorRGB& rhs);
        ColorBGRInt& operator/=(float factor);

        void add(const ColorBGRInt& rhs);
        void sub(const ColorBGRInt& rhs);
        void mul(float factor);
        void mul(const ColorRGB& rhs);
        void div(float factor);

        ColorBGRAInt toBGRAInt(uint8_t a) const;

        uint8_t b_, g_, r_;
    };

}

#endif  // CYRONENO_COLOR_H_