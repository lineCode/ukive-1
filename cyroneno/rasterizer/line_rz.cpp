#include "line_rz.h"

#include <algorithm>

#include "common.h"
#include "point_rz.h"
#include "../equations.h"


namespace {

    /**
     *  1 -> 0
     * -1 -> 1
     */
    int zeroOne(int val) {
        return (1 - val) / 2;
    }

}

namespace cyro {

    void LineRz::draw(const Point2I& p0, const Point2I& p1, const Color& c, ImagePng* target) {
        int dx = std::abs(p1.x - p0.x);
        int dy = std::abs(p1.y - p0.y);

        int x_inc = sgn(p1.x - p0.x);
        int y_inc = sgn(p1.y - p0.y);

        int xk = p0.x;
        int yk = p0.y;
        int ek = dx - dy;

        for (;;) {
            PointRz::draw(xk, yk, c, target);

            int exy = ek;
            int ex = exy + dy;
            int ey = exy - dx;
            if (ex + exy >= zeroOne(x_inc)) {
                if (xk == p1.x) return;
                xk += x_inc;
                // 将误差值移动至新的对角线像素
                ek -= dy;
            }
            if (ey + exy <= -zeroOne(x_inc)) {
                if (yk == p1.y) return;
                yk += y_inc;
                // 将误差值移动至新的对角线像素
                ek += dx;
            }
        }
    }

    void LineRz::drawSeg(const Point2I& p0, const Point2I& p1, const Color& c, ImagePng* target) {
        int x0 = p0.x, y0 = p0.y;
        int x1 = p1.x, y1 = p1.y;

        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);

        int x_inc = sgn(x1 - x0);
        int y_inc = sgn(y1 - y0);

        int xk = x0;
        int yk = y0;
        int prev_xk = xk;
        int prev_yk = yk;
        int ek = dx - dy;

        for (;;) {
            int exy = ek;
            int ex = exy + dy;
            int ey = exy - dx;
            if (ex + exy >= zeroOne(x_inc)) {
                if (xk == x1) return;
                xk += x_inc;
                ek -= dy;
            }
            if (ey + exy <= -zeroOne(x_inc)) {
                if (yk == y1) return;
                yk += y_inc;
                ek += dx;
            }

            PointRz::draw(prev_xk, prev_yk, c, target);
            prev_xk = xk;
            prev_yk = yk;
        }
    }

    void LineRz::drawAA(const Point2& p0, const Point2& p1, const Color& c, ImagePng* target) {
    }

}