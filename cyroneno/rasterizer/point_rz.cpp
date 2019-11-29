#include "point_rz.h"

#include <algorithm>

#include "../image.h"


namespace cyro {

    void PointRz::draw(int x, int y, const Color& c, ImagePng* target) {
        ColorBGRAInt color(255 * c.r_, 255 * c.g_, 255 * c.b_, 255 * c.a_);
        target->drawColor(x, y, color);
    }

    void PointRz::drawAA(double x, double y, const Color& c, ImagePng* target) {
        auto dx = x - std::floor(x);
        auto dy = y - std::floor(y);
        ColorBGRAInt color(255 * c.r_, 255 * c.g_, 255 * c.b_, 255 * c.a_);

        if (dx == 0 && dy == 0) {
            // 不需要插值
            target->drawColor(x, y, color);
        } else if (dx > 0 && dy == 0) {
            // x 轴插值
            target->drawColor(x, y, color * (1 - dx));
            target->drawColor(x + 1, y, color * dx);
        } else if (dy > 0 && dx == 0) {
            // y 轴插值
            target->drawColor(x, y, color * (1 - dy));
            target->drawColor(x, y + 1, color * dy);
        } else {
            // xy 轴插值
            target->drawColor(x, y, color * ((1 - dx) * (1 - dy)));
            target->drawColor(x + 1, y, color * (dx * (1 - dy)));
            target->drawColor(x, y + 1, color * ((1 - dx) * dy));
            target->drawColor(x + 1, y + 1, color * (dx * dy));
        }
    }

}