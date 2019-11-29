#include "triangle_rz.h"

#include <algorithm>

#include "../color.h"
#include "../equations.h"
#include "point_rz.h"


namespace cyro {

    void TriangleRz::draw(
        const Point2& p0, const Point2& p1, const Point2& p2,
        const Color& c0, const Color& c1, const Color& c2, ImagePng* target)
    {
        Line2DEqu line01(p0, p1);
        Line2DEqu line02(p0, p2);
        Line2DEqu line12(p1, p2);
        auto min_x = std::min({ p0.x_, p1.x_, p2.x_ });
        auto max_x = std::max({ p0.x_, p1.x_, p2.x_ });
        auto min_y = std::min({ p0.y_, p1.y_, p2.y_ });
        auto max_y = std::max({ p0.y_, p1.y_, p2.y_ });

        for (double y = min_y; y <= max_y; ++y) {
            for (double x = min_x; x <= max_x; ++x) {
                double �� = line01.cal(x, y) / line01.cal(p2.x_, p2.y_);
                double �� = line02.cal(x, y) / line02.cal(p1.x_, p1.y_);
                double �� = line12.cal(x, y) / line12.cal(p0.x_, p0.y_);
                if (�� > 0 && �� > 0 && �� > 0) {
                    auto c = c2 * �� + c1 * �� + c0 * ��;
                    PointRz::draw(x, y, c, target);
                }
            }
        }
    }

    void TriangleRz::drawAA(
        const Point2& p0, const Point2& p1, const Point2& p2,
        const Color& c0, const Color& c1, const Color& c2, ImagePng* target)
    {
    }

}