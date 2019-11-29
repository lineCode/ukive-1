#include "circle_rz.h"

#include "point_rz.h"


namespace cyro {

    void CircleRz::draw(
        const Point2& center, int r, const Color& c, ImagePng* img)
    {
        if (r <= 0) {
            return;
        }

        int xk = -r;
        int yk = 0;
        int ek = 2 * (1 - r);

        PointRz::draw(-r + center.x, 0 + center.y, c, img);
        PointRz::draw(0 + center.x, r + center.y, c, img);
        PointRz::draw(r + center.x, 0 + center.y, c, img);
        PointRz::draw(0 + center.x, -r + center.y, c, img);

        while (xk < 0) {
            int exy = ek;
            int ex = exy - (2 * xk + 1);
            int ey = exy - (2 * yk + 1);

            if (ey + exy <= 0) {
                ++yk;
                ek += (2 * yk + 1);
            }
            if (ex + exy >= 0/* || ek - (2 * yk + 1) + ek > 0*/) {
                ++xk;
                ek += (2 * xk + 1);
            }

            if (xk != 0) {
                PointRz::draw(xk + center.x, yk + center.y, c, img);
                PointRz::draw(-xk + center.x, yk + center.y, c, img);
                PointRz::draw(xk + center.x, -yk + center.y, c, img);
                PointRz::draw(-xk + center.x, -yk + center.y, c, img);
            }
        }
    }

    void CircleRz::draw(
        const Point2& center, int a, int b, const Color& c, ImagePng* img)
    {
        if (a <= 0 || b <= 0) {
            return;
        }

        int xk = -a;
        int yk = 0;
        int ek = a * a - b * b * (2 * a - 1);

        PointRz::draw(-a + center.x, 0 + center.y, c, img);
        PointRz::draw(0 + center.x, b + center.y, c, img);
        PointRz::draw(a + center.x, 0 + center.y, c, img);
        PointRz::draw(0 + center.x, -b + center.y, c, img);

        while (xk < 0) {
            int exy = ek;
            int ex = exy - (2 * xk + 1)*b*b;
            int ey = exy - (2 * yk + 1)*a*a;

            if (ex + exy >= 0) {
                ++xk;
                ek += (2 * xk + 1)*b*b;
            }
            if (ey + exy <= 0) {
                ++yk;
                ek += (2 * yk + 1)*a*a;
            }

            if (xk != 0) {
                PointRz::draw(xk + center.x, yk + center.y, c, img);
                PointRz::draw(-xk + center.x, yk + center.y, c, img);
                PointRz::draw(xk + center.x, -yk + center.y, c, img);
                PointRz::draw(-xk + center.x, -yk + center.y, c, img);
            }
        }

        while (yk < b) {
            PointRz::draw(xk + center.x, yk + center.y, c, img);
            PointRz::draw(-xk + center.x, -yk + center.y, c, img);
            ++yk;
        }
    }

    void CircleRz::drawAA(
        const Point2& center, double r, const Color& c, ImagePng* img)
    {
    }

    void CircleRz::drawOld(const Point2& center, int radius, const Color& c, ImagePng* img) {
        if (radius <= 0) {
            return;
        }

        int pk = 1 - radius;

        int xk = 0;
        int yk = radius;
        PointRz::draw(xk + center.x, yk + center.y, c, img);
        PointRz::draw(yk + center.y, xk + center.x, c, img);
        PointRz::draw(xk + center.x, -yk + center.y, c, img);
        PointRz::draw(-yk + center.y, xk + center.x, c, img);

        int _2xk = 0;
        int _2yk = 2 * radius;

        while (xk < yk) {
            ++xk;
            _2xk += 2;
            if (pk <= 0) {
                pk += _2xk + 1;
            } else {
                --yk;
                _2yk -= 2;
                pk += _2xk + 1 - _2yk;
            }

            if (xk <= yk) {
                PointRz::draw(xk + center.x, yk + center.y, c, img);
                PointRz::draw(-xk + center.x, yk + center.y, c, img);
                PointRz::draw(xk + center.x, -yk + center.y, c, img);
                PointRz::draw(-xk + center.x, -yk + center.y, c, img);
                if (xk != yk) {
                    PointRz::draw(yk + center.y, xk + center.x, c, img);
                    PointRz::draw(-yk + center.y, xk + center.x, c, img);
                    PointRz::draw(yk + center.y, -xk + center.x, c, img);
                    PointRz::draw(-yk + center.y, -xk + center.x, c, img);
                }
            }
        }
    }

}