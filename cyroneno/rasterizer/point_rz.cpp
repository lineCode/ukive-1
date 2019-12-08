#include "point_rz.h"

#include <algorithm>

#include "../image.h"


namespace cyro {

    void PointRz::draw(int x, int y, const Color& c, ImagePng* target) {
        ColorBGRAInt color(
            uint8_t(255 * c.r),
            uint8_t(255 * c.g),
            uint8_t(255 * c.b),
            uint8_t(255 * c.a));
        target->drawColor(x, y, color);
    }

    void PointRz::drawAA(double x, double y, const Color& c, ImagePng* target) {
        float dx = float(x - std::floor(x));
        float dy = float(y - std::floor(y));
        ColorBGRAInt color(
            uint8_t(255 * c.r),
            uint8_t(255 * c.g),
            uint8_t(255 * c.b),
            uint8_t(255 * c.a));

        if (dx == 0 && dy == 0) {
            // 不需要插值
            target->drawColor(int(x), int(y), color);
        } else if (dx > 0 && dy == 0) {
            // x 轴插值
            target->drawColor(int(x), int(y), color * (1 - dx));
            target->drawColor(int(x + 1), int(y), color * dx);
        } else if (dy > 0 && dx == 0) {
            // y 轴插值
            target->drawColor(int(x), int(y), color * (1 - dy));
            target->drawColor(int(x), int(y + 1), color * dy);
        } else {
            // xy 轴插值
            target->drawColor(int(x), int(y), color * ((1 - dx) * (1 - dy)));
            target->drawColor(int(x + 1), int(y), color * (dx * (1 - dy)));
            target->drawColor(int(x), int(y + 1), color * ((1 - dx) * dy));
            target->drawColor(int(x + 1), int(y + 1), color * (dx * dy));
        }
    }

}