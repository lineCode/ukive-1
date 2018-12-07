#include "color.h"

#include "ukive/log.h"
#include "ukive/utils/number.h"


namespace ukive {

    Color::Color()
        :r(0.f), g(0.f), b(0.f), a(0.f) {}

    Color::Color(const Color& color)
        :r(color.r), g(color.g), b(color.b), a(color.a) {}

    Color::Color(float r, float g, float b, float a)
        :r(r), g(g), b(b), a(a) {}

    Color& Color::operator=(const Color& rhs) {
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
        a = rhs.a;

        return *this;
    }


    Color Color::parse(const string16& color) {
        if (color.empty() || color.at(0) != L'#') {
            LOG(Log::ERR) << "Unknown color: " << color;
            return Color::Red500;
        }

        if (color.length() == 7) {
            int r = Number::parseInt(color.substr(1, 2), 16);
            int g = Number::parseInt(color.substr(3, 2), 16);
            int b = Number::parseInt(color.substr(5, 2), 16);

            return Color::ofInt(r, g, b);
        } else if (color.length() == 9) {
            int a = Number::parseInt(color.substr(1, 2), 16);
            int r = Number::parseInt(color.substr(3, 2), 16);
            int g = Number::parseInt(color.substr(5, 2), 16);
            int b = Number::parseInt(color.substr(7, 2), 16);

            return Color::ofInt(r, g, b, a);
        } else {
            LOG(Log::ERR) << "Unknown color: " << color;
            return Color::Red500;
        }
    }

    Color Color::ofInt(int r, int g, int b, int a) {
        return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    Color Color::ofRGB(unsigned int rgb, float a) {
        return Color(
            ((rgb & red_mask) >> red_shift) / 255.f,
            ((rgb & green_mask) >> green_shift) / 255.f,
            ((rgb & blue_mask) >> blue_shift) / 255.f,
            a);
    }

    Color Color::ofARGB(unsigned int argb) {
        return Color(
            ((argb & red_mask) >> red_shift) / 255.f,
            ((argb & green_mask) >> green_shift) / 255.f,
            ((argb & blue_mask) >> blue_shift) / 255.f,
            ((argb & alpha_mask) >> alpha_shift) / 255.f);
    }


    int Color::GetA(unsigned int argb) {
        return (argb & alpha_mask) >> alpha_shift;
    }

    int Color::GetR(unsigned int argb) {
        return (argb & red_mask) >> red_shift;
    }

    int Color::GetG(unsigned int argb) {
        return (argb & green_mask) >> green_shift;
    }

    int Color::GetB(unsigned int argb) {
        return (argb & blue_mask) >> blue_shift;
    }


    // Color White.
    const Color Color::White = Color::parse(L"#FFFFFF");
    // Color Black.
    const Color Color::Black = Color::parse(L"#000000");
    // Color Transparent.
    const Color Color::Transparent = Color::parse(L"#00000000");

    // Material Color Red.
    Color Color::Red50 = Color::parse(L"#FFEBEE");
    Color Color::Red100 = Color::parse(L"#FFCDD2");
    Color Color::Red200 = Color::parse(L"#EF9A9A");
    Color Color::Red300 = Color::parse(L"#E57373");
    Color Color::Red400 = Color::parse(L"#EF5350");
    Color Color::Red500 = Color::parse(L"#F44336");
    Color Color::Red600 = Color::parse(L"#E53935");
    Color Color::Red700 = Color::parse(L"#D32F2F");
    Color Color::Red800 = Color::parse(L"#C62828");
    Color Color::Red900 = Color::parse(L"#B71C1C");

    // Material Color Orange.
    Color Color::Orange50 = Color::parse(L"#FFF3E0");
    Color Color::Orange100 = Color::parse(L"#FFE0B2");
    Color Color::Orange200 = Color::parse(L"#FFCC80");
    Color Color::Orange300 = Color::parse(L"#FFB74D");
    Color Color::Orange400 = Color::parse(L"#FFA726");
    Color Color::Orange500 = Color::parse(L"#FF9800");
    Color Color::Orange600 = Color::parse(L"#FB8C00");
    Color Color::Orange700 = Color::parse(L"#F57C00");
    Color Color::Orange800 = Color::parse(L"#EF6C00");
    Color Color::Orange900 = Color::parse(L"#E65100");

    // Material Color Yellow.
    Color Color::Yellow50 = Color::parse(L"#FFFDE7");
    Color Color::Yellow100 = Color::parse(L"#FFF9C4");
    Color Color::Yellow200 = Color::parse(L"#FFF59D");
    Color Color::Yellow300 = Color::parse(L"#FFF176");
    Color Color::Yellow400 = Color::parse(L"#FFEE58");
    Color Color::Yellow500 = Color::parse(L"#FFEB3B");
    Color Color::Yellow600 = Color::parse(L"#FDD835");
    Color Color::Yellow700 = Color::parse(L"#FBC02D");
    Color Color::Yellow800 = Color::parse(L"#F9A825");
    Color Color::Yellow900 = Color::parse(L"#F57F17");

    // Material Color Pink.
    Color Color::Pink50 = Color::parse(L"#FCE4EC");
    Color Color::Pink100 = Color::parse(L"#F8BBD0");
    Color Color::Pink200 = Color::parse(L"#F48FB1");
    Color Color::Pink300 = Color::parse(L"#F06292");
    Color Color::Pink400 = Color::parse(L"#EC407A");
    Color Color::Pink500 = Color::parse(L"#E91E63");
    Color Color::Pink600 = Color::parse(L"#D81B60");
    Color Color::Pink700 = Color::parse(L"#C2185B");
    Color Color::Pink800 = Color::parse(L"#AD1457");
    Color Color::Pink900 = Color::parse(L"#880E4F");

    // Material Color Green.
    Color Color::Green50 = Color::parse(L"#E8F5E9");
    Color Color::Green100 = Color::parse(L"#C8E6C9");
    Color Color::Green200 = Color::parse(L"#A5D6A7");
    Color Color::Green300 = Color::parse(L"#81C784");
    Color Color::Green400 = Color::parse(L"#66BB6A");
    Color Color::Green500 = Color::parse(L"#4CAF50");
    Color Color::Green600 = Color::parse(L"#43A047");
    Color Color::Green700 = Color::parse(L"#388E3C");
    Color Color::Green800 = Color::parse(L"#2E7D32");
    Color Color::Green900 = Color::parse(L"#1B5E20");

    // Material Color Blue.
    Color Color::Blue50 = Color::parse(L"#E3F2FD");
    Color Color::Blue100 = Color::parse(L"#BBDEFB");
    Color Color::Blue200 = Color::parse(L"#90CAF9");
    Color Color::Blue300 = Color::parse(L"#64B5F6");
    Color Color::Blue400 = Color::parse(L"#42A5F5");
    Color Color::Blue500 = Color::parse(L"#2196F3");
    Color Color::Blue600 = Color::parse(L"#1E88E5");
    Color Color::Blue700 = Color::parse(L"#1976D2");
    Color Color::Blue800 = Color::parse(L"#1565C0");
    Color Color::Blue900 = Color::parse(L"#0D47A1");

    // Material Color Grey.
    Color Color::Grey50 = Color::parse(L"#FAFAFA");
    Color Color::Grey100 = Color::parse(L"#F5F5F5");
    Color Color::Grey200 = Color::parse(L"#EEEEEE");
    Color Color::Grey300 = Color::parse(L"#E0E0E0");
    Color Color::Grey400 = Color::parse(L"#BDBDBD");
    Color Color::Grey500 = Color::parse(L"#9E9E9E");
    Color Color::Grey600 = Color::parse(L"#757575");
    Color Color::Grey700 = Color::parse(L"#616161");
    Color Color::Grey800 = Color::parse(L"#424242");
    Color Color::Grey900 = Color::parse(L"#212121");

}