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
        auto min_x = std::min({ p0.x, p1.x, p2.x });
        auto max_x = std::max({ p0.x, p1.x, p2.x });
        auto min_y = std::min({ p0.y, p1.y, p2.y });
        auto max_y = std::max({ p0.y, p1.y, p2.y });

        for (double y = min_y; y <= max_y; ++y) {
            for (double x = min_x; x <= max_x; ++x) {
                double ¦Á = line01.cal(x, y) / line01.cal(p2.x, p2.y);
                double ¦Â = line02.cal(x, y) / line02.cal(p1.x, p1.y);
                double ¦Ã = line12.cal(x, y) / line12.cal(p0.x, p0.y);
                if (¦Á > 0 && ¦Â > 0 && ¦Ã > 0) {
                    auto c = c2 * ¦Á + c1 * ¦Â + c0 * ¦Ã;
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