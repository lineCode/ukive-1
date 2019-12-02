#include "bezier_quad_rz.h"

#include "utils/log.h"

#include "common.h"
#include "line_rz.h"
#include "point_rz.h"
#include "../vector.hpp"


namespace cyro {

    void BezierQuadRz::draw(
        const Point2I& p1, const Point2I& p2, const Point2I& p3, const Color& c, ImagePng* target)
    {
        const int x2 = p2.x - p1.x;
        const int y2 = p2.y - p1.y;
        const int x3 = p3.x - p1.x;
        const int y3 = p3.y - p1.y;

        const double z_pred = y2 * (y2 - y3);
        const double p_pred = x2 * (x2 - x3);

        const double x_de = 2 * x2 - x3;
        const double y_de = 2 * y2 - y3;
        const double tp = x2 / x_de;
        const double tz = y2 / y_de;

        // 斜率过无穷的位置，使用前需判断 p_pred
        const double xp = x2 * x2 / x_de;
        const double yp = 2 * (1 - tp)*tp*y2 + tp * tp * y3;

        // 斜率过零的位置，使用前需判断 z_pred
        const double xz = 2 * (1 - tz)*tz*x2 + tz * tz * x3;
        const double yz = y2 * y2 / y_de;

        if (z_pred > 0 && p_pred > 0) {
            if (tp <= tz) {
                Point2I p1_p(int(std::round(xp + p1.x)), int(std::round(xp*y2 / x2 + p1.y)));
                Point2I pp(int(std::round(xp + p1.x)), int(std::round(yp + p1.y)));
                drawSegment(p1, p1_p, pp, c, target);

                Point2I pp_z(int(std::round(xp + p1.x)), int(std::round(yz + p1.y)));
                Point2I pz(int(std::round(xz + p1.x)), int(std::round(yz + p1.y)));
                drawSegment(pp, pp_z, pz, c, target);

                Point2I pz_3(int(std::round(x3 - (y3 - yz)*(x3 - x2) / (y3 - y2) + p1.x)), int(std::round(yz + p1.y)));
                drawSegment(pz, pz_3, p3, c, target);
            } else {
                Point2I p1_z(int(std::round((yz - y2)*x2 / y2 + x2 + p1.x)), int(std::round(yz + p1.y)));
                Point2I pz(int(std::round(xz + p1.x)), int(std::round(yz + p1.y)));
                drawSegment(p1, p1_z, pz, c, target);

                Point2I pz_p(int(std::round(xp + p1.x)), int(std::round(yz + p1.y)));
                Point2I pp(int(std::round(xp + p1.x)), int(std::round(yp + p1.y)));
                drawSegment(pz, pz_p, pp, c, target);

                Point2I pp_3(int(std::round(xp + p1.x)), int(std::round(y2 + (xp - x2)*(y3 - y2) / (x3 - x2) + p1.y)));
                drawSegment(pp, pp_3, p3, c, target);
            }
        } else if (p_pred > 0) {
            Point2I p1_p(int(std::round(xp + p1.x)), int(std::round(xp*y2 / x2 + p1.y)));
            Point2I pp(int(std::round(xp + p1.x)), int(std::round(yp + p1.y)));
            drawSegment(p1, p1_p, pp, c, target);

            Point2I pp_3(int(std::round(xp + p1.x)), int(std::round(y2 + (xp - x2)*(y3 - y2) / (x3 - x2) + p1.y)));
            drawSegment(pp, pp_3, p3, c, target);
        } else if (z_pred > 0) {
            Point2I p1_z(int(std::round((yz - y2)*x2 / y2 + x2 + p1.x)), int(std::round(yz + p1.y)));
            Point2I pz(int(std::round(xz + p1.x)), int(std::round(yz + p1.y)));
            drawSegment(p1, p1_z, pz, c, target);

            Point2I pz_3(int(std::round(x3 - (y3 - yz)*(x3 - x2) / (y3 - y2) + p1.x)), int(std::round(yz + p1.y)));
            drawSegment(pz, pz_3, p3, c, target);
        } else {
            drawSegment(p1, p2, p3, c, target);
        }
    }

    void BezierQuadRz::draw(
        const Point2& p1, const Point2& p2, const Point2& p3,
        double w0, double w1, double w2,
        const Color& c, ImagePng* target)
    {
        drawParam(p1, p2, p3, w0, w1, w2, c, target);
    }

    void BezierQuadRz::drawAA(
        const Point2& p1, const Point2& p2, const Point2& p3, const Color& c, ImagePng* target)
    {
    }

    void BezierQuadRz::drawSegment(
        const Point2I& p1, const Point2I& p2, const Point2I& p3, const Color& c, ImagePng* target)
    {
        int x0, y0;
        int x2, y2;
        bool is_swapped;
        if ((p2 - p1).lengthSq() >= (p3 - p2).lengthSq()) {
            x0 = p1.x - p2.x, y0 = p1.y - p2.y;
            x2 = p3.x - p2.x, y2 = p3.y - p2.y;
            is_swapped = false;
        } else {
            x0 = p3.x - p2.x, y0 = p3.y - p2.y;
            x2 = p1.x - p2.x, y2 = p1.y - p2.y;
            is_swapped = true;
        }

        DCHECK(x0*x2 <= 0 && y0*y2 <= 0);

        // 曲率
        double K = x0 * y2 - x2 * y0;

        auto e = [x0, y0, x2, y2, K](double x, double y) -> double {
            return std::pow(x*(y0 + y2) - y * (x0 + x2) + K, 2) - 4 * (x*y2 - y * x2)*K;
        };

        int xk = x0;
        int yk = y0;

        if (K != 0) {
            int x_inc = sgn(x2 - x0);
            int y_inc = sgn(y2 - y0);
            int sign = sgnd(K) * x_inc * y_inc;
            int remainder = x_inc * y_inc * 2 * (x0 + x2)*(y0 + y2);

            double dx = (1 + x_inc * 2 * xk)*std::pow(y0 + y2, 2) - x_inc * 2 * yk*(x0 + x2)*(y0 + y2) + x_inc * 2 * K*(y0 - y2);
            double dy = (1 + y_inc * 2 * yk)*std::pow(x0 + x2, 2) - y_inc * 2 * xk*(x0 + x2)*(y0 + y2) - y_inc * 2 * K*(x0 - x2);

            // 初始误差：e(x0+1, y0+1) - e(x0, y0)
            double ek = e(x0 + x_inc, y0 + y_inc);
            double ek_t = dx + dy - remainder;
            DCHECK(ek == ek_t);

            dx -= remainder;
            dy -= remainder;

            while (true) {
                if (xk == x2 && yk == y2) {
                    return;
                }
                if (sign * dx > 0 || sign * dy < 0) {
                    break;
                }

                if (!is_swapped) {
                    PointRz::draw(xk + p2.x, yk + p2.y, c, target);
                }

                double exy = ek;
                double ex = e(xk, yk + y_inc);
                double ex_t = exy - dx;
                DCHECK(ex == ex_t);

                double ey = e(xk + x_inc, yk);
                double ey_t = exy - dy;
                DCHECK(ey == ey_t);

                if (sign * (ex + exy) >= 0) {
                    xk += x_inc;

                    double tmp_dx = (1 + x_inc * 2 * xk)*std::pow(y0 + y2, 2) - x_inc * 2 * yk*(x0 + x2)*(y0 + y2) + x_inc * 2 * K*(y0 - y2);
                    dx = tmp_dx;

                    double tmp_dy = (1 + y_inc * 2 * yk)*std::pow(x0 + x2, 2) - y_inc * 2 * xk*(x0 + x2)*(y0 + y2) - y_inc * 2 * K*(x0 - x2);
                    dy = tmp_dy;

                    dx -= remainder;
                    dy -= remainder;

                    ek = e(xk + x_inc, yk + y_inc);
                    ek_t += dx;
                    DCHECK(ek == ek_t);
                }
                if (sign * (ey + exy) <= 0) {
                    yk += y_inc;

                    double tmp_dx = (1 + x_inc * 2 * xk)*std::pow(y0 + y2, 2) - x_inc * 2 * yk*(x0 + x2)*(y0 + y2) + x_inc * 2 * K*(y0 - y2);
                    dx = tmp_dx;

                    double tmp_dy = (1 + y_inc * 2 * yk)*std::pow(x0 + x2, 2) - y_inc * 2 * xk*(x0 + x2)*(y0 + y2) - y_inc * 2 * K*(x0 - x2);
                    dy = tmp_dy;

                    dx -= remainder;
                    dy -= remainder;

                    ek = e(xk + x_inc, yk + y_inc);
                    ek_t += dy;
                    DCHECK(ek == ek_t);
                }

                if (is_swapped) {
                    PointRz::draw(xk + p2.x, yk + p2.y, c, target);
                }
            }
        }

        if (is_swapped) {
            LineRz::drawSeg(p1, Point2I(xk + p2.x, yk + p2.y), c, target);
        } else {
            LineRz::drawSeg(Point2I(xk + p2.x, yk + p2.y), p3, c, target);
        }
    }

    void BezierQuadRz::drawParam(
        const Point2& p1, const Point2& p2, const Point2& p3, const Color& c, ImagePng* target)
    {
        for (double i = 0; i < 1; i += 0.05) {
            double xk = (1 - i)*(1 - i)*p1.x + 2 * (1 - i)*i*p2.x + i * i*p3.x;
            double yk = (1 - i)*(1 - i)*p1.y + 2 * (1 - i)*i*p2.y + i * i*p3.y;
            PointRz::draw(int(xk), int(yk), c, target);
        }
    }

    void BezierQuadRz::drawParam(
        const Point2& p1, const Point2& p2, const Point2& p3,
        double w0, double w1, double w2,
        const Color& c, ImagePng* target)
    {
        double w_2 = w1 * w1 / (w0*w2);

        double xk = p1.x;
        double yk = p1.y;
        for (double i = 0; i < 1; i += 0.05) {
            double de = (1 - i)*(1 - i)*w0 + 2 * (1 - i)*i*w1 + i * i*w2;

            double t_xk = ((1 - i)*(1 - i)*w0*p1.x + 2 * (1 - i)*i*w1*p2.x + i * i*w2*p3.x) / de;
            double t_yk = ((1 - i)*(1 - i)*w0*p1.y + 2 * (1 - i)*i*w1*p2.y + i * i*w2*p3.y) / de;

            t_xk = std::round(t_xk);
            t_yk = std::round(t_yk);

            //PointRz::draw(xk, yk, c, target);
            LineRz::draw(Point2I(int(xk), int(yk)), Point2I(int(t_xk), int(t_yk)), c, target);

            xk = t_xk;
            yk = t_yk;
        }
    }

}