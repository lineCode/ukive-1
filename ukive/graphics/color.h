#ifndef UKIVE_GRAPHICS_COLOR_H_
#define UKIVE_GRAPHICS_COLOR_H_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Color {
    public:
        Color();
        Color(const Color& color);
        Color(float r, float g, float b, float a = 1.f);

        Color& operator=(const Color& rhs);

        float a;
        float r;
        float g;
        float b;

    public:
        // 解析颜色字符串。
        // 支持的格式：#RRGGBB 或 #AARRGGBB
        static Color parse(const string16& color);

        static Color ofInt(int r, int g, int b, int a = 255);
        static Color ofRGB(unsigned int rgb, float a = 1.f);
        static Color ofARGB(unsigned int argb);

        static int getA(unsigned int argb);
        static int getR(unsigned int argb);
        static int getG(unsigned int argb);
        static int getB(unsigned int argb);

        // Color Black.
        const static Color Black;
        // Color White.
        const static Color White;
        // Color Transparent
        const static Color Transparent;

        // Material Color Red.
        static Color Red50, Red100, Red200, Red300, Red400,
            Red500, Red600, Red700, Red800, Red900;

        // Material Color Orange.
        static Color Orange50, Orange100, Orange200, Orange300, Orange400,
            Orange500, Orange600, Orange700, Orange800, Orange900;

        // Material Color Yellow.
        static Color Yellow50, Yellow100, Yellow200, Yellow300, Yellow400,
            Yellow500, Yellow600, Yellow700, Yellow800, Yellow900;

        // Material Color Pink.
        static Color Pink50, Pink100, Pink200, Pink300, Pink400,
            Pink500, Pink600, Pink700, Pink800, Pink900;

        // Material Color Green.
        static Color Green50, Green100, Green200, Green300, Green400,
            Green500, Green600, Green700, Green800, Green900;

        // Material Color Blue.
        static Color Blue50, Blue100, Blue200, Blue300, Blue400,
            Blue500, Blue600, Blue700, Blue800, Blue900;

        // Material Color Grey.
        static Color Grey50, Grey100, Grey200, Grey300, Grey400,
            Grey500, Grey600, Grey700, Grey800, Grey900;

    private:
        static const unsigned int alpha_shift = 24;
        static const unsigned int red_shift = 16;
        static const unsigned int green_shift = 8;
        static const unsigned int blue_shift = 0;

        static const unsigned int alpha_mask = 0xff << alpha_shift;
        static const unsigned int red_mask = 0xff << red_shift;
        static const unsigned int green_mask = 0xff << green_shift;
        static const unsigned int blue_mask = 0xff << blue_shift;
    };

}

#endif  // UKIVE_GRAPHICS_COLOR_H_