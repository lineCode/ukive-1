#include "bezier_cubic_rz.h"

#include "utils/log.h"

#include "common.h"
#include "line_rz.h"
#include "point_rz.h"


namespace cyro {

    void BezierCubicRz::draw(
        const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
        const Color& c, ImagePng* target)
    {
        /*drawParam(p1, p2, p3, p4, c, target);
        return;*/

        int x0 = 0, y0 = 0;
        int x1 = p2.x - p1.x, y1 = p2.y - p1.y;
        int x2 = p3.x - p1.x, y2 = p3.y - p1.y;
        int x3 = p4.x - p1.x, y3 = p4.y - p1.y;

        double xa = x0 - 3 * x1 + 3 * x2 - x3;
        double xb = x0 / 2.0 - x1 / 2.0 - x2 / 2.0 + x3 / 2.0;
        double xc = x0 / 4.0 + x1 / 4.0 - x2 / 4.0 - x3 / 4.0;
        double xd = x0 / 8.0 + 3 * x1 / 8.0 + 3 * x2 / 8.0 + x3 / 8.0;

        double ya = y0 - 3 * y1 + 3 * y2 - y3;
        double yb = y0 / 2.0 - y1 / 2.0 - y2 / 2.0 + y3 / 2.0;
        double yc = y0 / 4.0 + y1 / 4.0 - y2 / 4.0 - y3 / 4.0;
        double yd = y0 / 8.0 + 3 * y1 / 8.0 + 3 * y2 / 8.0 + y3 / 8.0;

        int ct = 0;
        double tn[5];
        double delta1 = 4 * xb*xb - 4 * xa*xc;
        double delta2 = 4 * yb*yb - 4 * ya*yc;
        if (xa == 0) {
            auto t = xc / (2 * xb);
            if (t > -0.5 && t < 0.5) { tn[ct++] = t; }
        } else {
            if (delta1 > 0) {
                // 两实根
                auto t = (2 * xb + std::sqrt(delta1)) / (2 * xa);
                if (t > -0.5 && t < 0.5) { tn[ct++] = t; }

                t = (2 * xb - std::sqrt(delta1)) / (2 * xa);
                if (t > -0.5 && t < 0.5) { tn[ct++] = t; }
            } else if (delta1 == 0) {
                // 一实根
                auto t = xb / xa;
                if (t > -0.5 && t < 0.5) { tn[ct++] = t; }
            }
        }

        if (ya == 0) {
            auto t = yc / (2 * yb);
            if (t > -0.5 && t < 0.5) { tn[ct++] = t; }
        } else {
            if (delta2 > 0) {
                // 两实根
                auto t = (2 * yb + std::sqrt(delta2)) / (2 * ya);
                if (t > -0.5 && t < 0.5) { tn[ct++] = t; }

                t = (2 * yb - std::sqrt(delta2)) / (2 * ya);
                if (t > -0.5 && t < 0.5) { tn[ct++] = t; }
            } else if (delta2 == 0) {
                // 一实根
                double t = yb / ya;
                if (t > -0.5 && t < 0.5) { tn[ct++] = t; }
            }
        }

        // 从小到大排序
        for (int i = 0; i < ct; ++i) {
            for (int j = i + 1; j < ct; ++j) {
                if (tn[i] > tn[j]) {
                    auto tmp = tn[i];
                    tn[i] = tn[j];
                    tn[j] = tmp;
                }
            }
        }
        tn[ct++] = 0.5;

        double t1 = -0.5;
        Point2 pp0(p1);
        for (int i = 0; i < ct; ++i) {
            double t2 = tn[i];
            Point2 pp1(
                std::round(-t2 * (t1*t1*xa - 2 * t1*xb + xc) + t1*t1*xb - 2 * t1*xc + xd + p1.x),
                std::round(-t2 * (t1*t1*ya - 2 * t1*yb + yc) + t1*t1*yb - 2 * t1*yc + yd + p1.y));
            Point2 pp2(
                std::round(-t1 * (t2*t2*xa - 2 * t2*xb + xc) + t2*t2*xb - 2 * t2*xc + xd + p1.x),
                std::round(-t1 * (t2*t2*ya - 2 * t2*yb + yc) + t2*t2*yb - 2 * t2*yc + yd + p1.y));
            Point2 pp3(
                std::round(-t2*t2*t2*xa + 3*t2*t2*xb - 3 * t2*xc + xd + p1.x),
                std::round(-t2*t2*t2*ya + 3*t2*t2*yb - 3 * t2*yc + yd + p1.y));
            drawSegment(pp0, pp1, pp2, pp3, c, target);

            pp0 = pp3;
            t1 = t2;
        }
    }

    void BezierCubicRz::drawSegment(
        const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
        const Color& c, ImagePng* target)
    {
        // TODO: 当前算法在绘制闭合曲线时可能会失败

        int xs = p1.x, ys = p1.y;
        int x0 = 0, y0 = 0;
        int x1 = p2.x - xs, y1 = p2.y - ys;
        int x2 = p3.x - xs, y2 = p3.y - ys;
        int x3 = p4.x - xs, y3 = p4.y - ys;

        int xk = x0;
        int yk = y0;
        int xe = x3, ye = y3;

        double xa = x0 - 3 * x1 + 3 * x2 - x3;
        double xb = (x0 - x1 - x2 + x3) / 2.0;
        double xc = (x0 + x1 - x2 - x3) / 4.0;
        double xd = (x0 + 3 * x1 + 3 * x2 + x3) / 8.0;

        double ya = y0 - 3 * y1 + 3 * y2 - y3;
        double yb = (y0 - y1 - y2 + y3) / 2.0;
        double yc = (y0 + y1 - y2 - y3) / 4.0;
        double yd = (y0 + 3 * y1 + 3 * y2 + y3) / 8.0;

        DCHECK((x0 - x1)*(x3 - x2) <= 0 && ((x2 - x1)*(x0 - x3) <= 0 || xb * xb < xa*xc));
        DCHECK((y0 - y1)*(y3 - y2) <= 0 && ((y2 - y1)*(y0 - y3) <= 0 || yb * yb < ya*yc));

        for (int i = 0; i < 2; ++i) {
            double cab = xa * yb - xb * ya;
            double cac = xa * yc - xc * ya;
            double cad = xa * yd - xd * ya;
            double cbc = xb * yc - xc * yb;
            double cbd = xb * yd - xd * yb;
            double ccd = xc * yd - xd * yc;

            double A = ya * ya*ya;
            double B = xa * ya*ya;
            double C = xa * xa*ya;
            double D = xa * xa*xa;
            double E = cad * ya*ya - 3 * (2 * cab*yc + cac * yb)*ya + 9 * cab*yb*yb;
            double F = (2 * cad + 3 * cbc)*xa*ya + 9 * (2 * cab*xb*yb + xb * xc*ya*ya - xa * xa*yb*yc);
            double G = cad * xa*xa - 3 * (2 * cab*xc + cac * xb)*xa + 9 * cab*xb*xb;
            double H = cad * cad*ya + 6 * (3 * cab*cbd - cac * cad)*yb + 3 * (3 * cac*cac - 4 * cab*cad - 9 * cab*cbc)*yc + 9 * cab*cac*yd;
            double I = cad * cad*xa + 6 * (3 * cab*cbd - cac * cad)*xb + 3 * (3 * cac*cac - 4 * cab*cad - 9 * cab*cbc)*xc + 9 * cab*cac*xd;
            double J = cad * cad*cad - 9 * cad*(cac*cbd + 2 * cab*ccd) + 27 * (cab*cbd*cbd + cac * cac*ccd) - 81 * cab*cbc*ccd;

            auto fe = [&A, &B, &C, &D, &E, &F, &G, &H, &I, &J](double x, double y)->double {
                return A * x*x*x - 3 * B*x*x*y + 3 * C*x*y*y - D * y*y*y + 3 * E*x*x - 3 * F*x*y + 3 * G*y*y + 3 * H*x - 3 * I*y + J;
            };

            int x_inc = sgn(x3 - x0);
            int y_inc = sgn(y3 - y0);

            double g = std::pow(cab + cac, 2) - cab * (cac + 3 * cbc);
            int sign = sgnd(g * x_inc * y_inc);

            double dx = fe(xk + x_inc, yk) - fe(xk, yk);
            double dy = fe(xk, yk + y_inc) - fe(xk, yk);
            double ek = fe(xk + x_inc, yk + y_inc);

            double rx = dx - (fe(xk + x_inc, yk + y_inc) - fe(xk, yk + y_inc));
            double ry = dy - (fe(xk + x_inc, yk + y_inc) - fe(xk + x_inc, yk));
            double ek_t = dx + dy - rx;
            DCHECK(ek == ek_t);

            dx -= rx;
            dy -= ry;
            bool is_first = true;

            for (;;) {
                if (xk == xe && yk == ye) {
                    return;
                }
                if (is_first) {
                    if (sign * dx <= 0 && sign * dy >= 0) {
                        is_first = false;
                    } else if (sign * (dx + rx) > 0 || sign * (dy + ry) < 0) {
                        break;
                    }
                } else {
                    if (sign * dx > 0 || sign * dy < 0) {
                        break;
                    }
                }

                PointRz::draw(xk + xs, yk + ys, c, target);

                double exy = ek;
                double ex = fe(xk, yk + y_inc);
                double ex_t = exy - dx;
                DCHECK(ex == ex_t);

                double ey = fe(xk + x_inc, yk);
                double ey_t = exy - dy;
                DCHECK(ey == ey_t);

                if (sign*(ex + exy) >= 0) {
                    xk += x_inc;

                    dx = fe(xk + x_inc, yk) - fe(xk, yk);
                    dy = fe(xk, yk + y_inc) - fe(xk, yk);
                    ek = fe(xk + x_inc, yk + y_inc);

                    rx = dx - (fe(xk + x_inc, yk + y_inc) - fe(xk, yk + y_inc));
                    ry = dy - (fe(xk + x_inc, yk + y_inc) - fe(xk + x_inc, yk));
                    dx -= rx;
                    dy -= ry;
                }

                if (sign*(ey + exy) <= 0) {
                    yk += y_inc;

                    dx = fe(xk + x_inc, yk) - fe(xk, yk);
                    dy = fe(xk, yk + y_inc) - fe(xk, yk);
                    ek = fe(xk + x_inc, yk + y_inc);

                    rx = dx - (fe(xk + x_inc, yk + y_inc) - fe(xk, yk + y_inc));
                    ry = dy - (fe(xk + x_inc, yk + y_inc) - fe(xk + x_inc, yk));
                    dx -= rx;
                    dy -= ry;
                }
            }

            if (i < 1) {
                x0 = x3; y0 = y3;
                x3 = 0; y3 = 0;
                xa = -xa; xc = -xc;
                ya = -ya; yc = -yc;
                xe = xk; ye = yk;
                xk = x0; yk = y0;
            }
        }

        LineRz::draw(Point2(xk + xs, yk + ys), Point2(xe + xs, ye + ys), c, target);
    }

    void BezierCubicRz::drawParam(
        const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
        const Color& c, ImagePng* target)
    {
        double xk = p1.x;
        double yk = p1.y;
        for (double i = 0; i < 1; i += 0.01) {
            double t_xk = std::pow(1 - i, 3)*p1.x + 3 * (1 - i)*(1 - i)*i*p2.x + 3 * (1 - i)*i*i*p3.x + i*i*i*p4.x;
            double t_yk = std::pow(1 - i, 3)*p1.y + 3 * (1 - i)*(1 - i)*i*p2.y + 3 * (1 - i)*i*i*p3.y + i*i*i*p4.y;

            t_xk = std::round(t_xk);
            t_yk = std::round(t_yk);

            //PointRz::draw(xk, yk, c, target);
            LineRz::draw(Point2(xk, yk), Point2(t_xk, t_yk), c, target);

            xk = t_xk;
            yk = t_yk;
        }
    }

}