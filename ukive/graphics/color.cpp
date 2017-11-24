#include "color.h"

#include "ukive/log.h"
#include "ukive/utils/number.h"


namespace ukive {

    D2D1::ColorF Color::parse(string16 color) {
        if (color.empty() || color.at(0) != L'#') {
            Log::e(L"unknown color");
            return D2D1::ColorF::Red;
        }

        if (color.length() == 7) {
            int r = UNumber::parseInt(color.substr(1, 2), 16);
            int g = UNumber::parseInt(color.substr(3, 2), 16);
            int b = UNumber::parseInt(color.substr(5, 2), 16);

            return D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f);
        }
        else if (color.length() == 9) {
            int a = UNumber::parseInt(color.substr(1, 2), 16);
            int r = UNumber::parseInt(color.substr(3, 2), 16);
            int g = UNumber::parseInt(color.substr(5, 2), 16);
            int b = UNumber::parseInt(color.substr(7, 2), 16);

            return D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
        }
        else {
            throw std::invalid_argument("unknown color");
        }
    }

    D2D1::ColorF Color::rgb(int r, int g, int b) {
        return D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f);
    }

    D2D1::ColorF Color::rgb(float r, float g, float b) {
        return D2D1::ColorF(r, g, b);
    }

    D2D1::ColorF Color::argb(int r, int g, int b, int a) {
        return D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    D2D1::ColorF Color::argb(float r, float g, float b, float a) {
        return D2D1::ColorF(r, g, b, a);
    }


    //Color White.
    D2D1::ColorF Color::White = Color::parse(L"#FFFFFF");
    //Color Black.
    D2D1::ColorF Color::Black = Color::parse(L"#000000");

    //Material Color Red.
    D2D1::ColorF Color::Red50 = Color::parse(L"#FFEBEE");
    D2D1::ColorF Color::Red100 = Color::parse(L"#FFCDD2");
    D2D1::ColorF Color::Red200 = Color::parse(L"#EF9A9A");
    D2D1::ColorF Color::Red300 = Color::parse(L"#E57373");
    D2D1::ColorF Color::Red400 = Color::parse(L"#EF5350");
    D2D1::ColorF Color::Red500 = Color::parse(L"#F44336");
    D2D1::ColorF Color::Red600 = Color::parse(L"#E53935");
    D2D1::ColorF Color::Red700 = Color::parse(L"#D32F2F");
    D2D1::ColorF Color::Red800 = Color::parse(L"#C62828");
    D2D1::ColorF Color::Red900 = Color::parse(L"#B71C1C");

    //Material Color Yellow.
    D2D1::ColorF Color::Yellow50 = Color::parse(L"#FFFDE7");
    D2D1::ColorF Color::Yellow100 = Color::parse(L"#FFF9C4");
    D2D1::ColorF Color::Yellow200 = Color::parse(L"#FFF59D");
    D2D1::ColorF Color::Yellow300 = Color::parse(L"#FFF176");
    D2D1::ColorF Color::Yellow400 = Color::parse(L"#FFEE58");
    D2D1::ColorF Color::Yellow500 = Color::parse(L"#FFEB3B");
    D2D1::ColorF Color::Yellow600 = Color::parse(L"#FDD835");
    D2D1::ColorF Color::Yellow700 = Color::parse(L"#FBC02D");
    D2D1::ColorF Color::Yellow800 = Color::parse(L"#F9A825");
    D2D1::ColorF Color::Yellow900 = Color::parse(L"#F57F17");

    //Material Color Pink.
    D2D1::ColorF Color::Pink50 = Color::parse(L"#FCE4EC");
    D2D1::ColorF Color::Pink100 = Color::parse(L"#F8BBD0");
    D2D1::ColorF Color::Pink200 = Color::parse(L"#F48FB1");
    D2D1::ColorF Color::Pink300 = Color::parse(L"#F06292");
    D2D1::ColorF Color::Pink400 = Color::parse(L"#EC407A");
    D2D1::ColorF Color::Pink500 = Color::parse(L"#E91E63");
    D2D1::ColorF Color::Pink600 = Color::parse(L"#D81B60");
    D2D1::ColorF Color::Pink700 = Color::parse(L"#C2185B");
    D2D1::ColorF Color::Pink800 = Color::parse(L"#AD1457");
    D2D1::ColorF Color::Pink900 = Color::parse(L"#880E4F");

    //Material Color Green.
    D2D1::ColorF Color::Green50 = Color::parse(L"#E8F5E9");
    D2D1::ColorF Color::Green100 = Color::parse(L"#C8E6C9");
    D2D1::ColorF Color::Green200 = Color::parse(L"#A5D6A7");
    D2D1::ColorF Color::Green300 = Color::parse(L"#81C784");
    D2D1::ColorF Color::Green400 = Color::parse(L"#66BB6A");
    D2D1::ColorF Color::Green500 = Color::parse(L"#4CAF50");
    D2D1::ColorF Color::Green600 = Color::parse(L"#43A047");
    D2D1::ColorF Color::Green700 = Color::parse(L"#388E3C");
    D2D1::ColorF Color::Green800 = Color::parse(L"#2E7D32");
    D2D1::ColorF Color::Green900 = Color::parse(L"#1B5E20");

    //Material Color Blue.
    D2D1::ColorF Color::Blue50 = Color::parse(L"#E3F2FD");
    D2D1::ColorF Color::Blue100 = Color::parse(L"#BBDEFB");
    D2D1::ColorF Color::Blue200 = Color::parse(L"#90CAF9");
    D2D1::ColorF Color::Blue300 = Color::parse(L"#64B5F6");
    D2D1::ColorF Color::Blue400 = Color::parse(L"#42A5F5");
    D2D1::ColorF Color::Blue500 = Color::parse(L"#2196F3");
    D2D1::ColorF Color::Blue600 = Color::parse(L"#1E88E5");
    D2D1::ColorF Color::Blue700 = Color::parse(L"#1976D2");
    D2D1::ColorF Color::Blue800 = Color::parse(L"#1565C0");
    D2D1::ColorF Color::Blue900 = Color::parse(L"#0D47A1");

    //Material Color Grey.
    D2D1::ColorF Color::Grey50 = Color::parse(L"#FAFAFA");
    D2D1::ColorF Color::Grey100 = Color::parse(L"#F5F5F5");
    D2D1::ColorF Color::Grey200 = Color::parse(L"#EEEEEE");
    D2D1::ColorF Color::Grey300 = Color::parse(L"#E0E0E0");
    D2D1::ColorF Color::Grey400 = Color::parse(L"#BDBDBD");
    D2D1::ColorF Color::Grey500 = Color::parse(L"#9E9E9E");
    D2D1::ColorF Color::Grey600 = Color::parse(L"#757575");
    D2D1::ColorF Color::Grey700 = Color::parse(L"#616161");
    D2D1::ColorF Color::Grey800 = Color::parse(L"#424242");
    D2D1::ColorF Color::Grey900 = Color::parse(L"#212121");

}