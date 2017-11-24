#ifndef UKIVE_GRAPHICS_COLOR_H_
#define UKIVE_GRAPHICS_COLOR_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Color {
    public:
        /**
         * 解析颜色字符串。
         * 支持的格式：#RRGGBB 或 #AARRGGBB
        */
        static D2D1::ColorF parse(string16 color);

        static D2D1::ColorF rgb(int r, int g, int b);
        static D2D1::ColorF rgb(float r, float g, float b);
        static D2D1::ColorF argb(int r, int g, int b, int a);
        static D2D1::ColorF argb(float r, float g, float b, float a);

        //Color Black.
        static D2D1::ColorF Black;
        //Color White.
        static D2D1::ColorF White;

        //Material Color Red.
        static D2D1::ColorF Red50, Red100, Red200, Red300, Red400,
            Red500, Red600, Red700, Red800, Red900;

        //Material Color Yellow.
        static D2D1::ColorF Yellow50, Yellow100, Yellow200, Yellow300, Yellow400,
            Yellow500, Yellow600, Yellow700, Yellow800, Yellow900;

        //Material Color Pink.
        static D2D1::ColorF Pink50, Pink100, Pink200, Pink300, Pink400,
            Pink500, Pink600, Pink700, Pink800, Pink900;

        //Material Color Green.
        static D2D1::ColorF Green50, Green100, Green200, Green300, Green400,
            Green500, Green600, Green700, Green800, Green900;

        //Material Color Blue.
        static D2D1::ColorF Blue50, Blue100, Blue200, Blue300, Blue400,
            Blue500, Blue600, Blue700, Blue800, Blue900;

        //Material Color Grey.
        static D2D1::ColorF Grey50, Grey100, Grey200, Grey300, Grey400,
            Grey500, Grey600, Grey700, Grey800, Grey900;

    private:
        Color();
    };

}

#endif  // UKIVE_GRAPHICS_COLOR_H_