#include "rasterizer.h"

#include "../equations.h"
#include "line_rz.h"
#include "circle_rz.h"
#include "point_rz.h"
#include "bezier_quad_rz.h"
#include "bezier_cubic_rz.h"
#include "triangle_rz.h"


/**
 * TODO:
 * 1. 顶点坐标需要支持浮点数
 * 2. 绘制三角形公共边时需要一些额外处理以防出现空洞
 **/

namespace cyro {

    Rasterizer::Rasterizer(int image_width, int image_height)
        : image_(image_width, image_height, ColorBGRAInt(255, 255, 255, 255)) {
    }

    void Rasterizer::drawPoint(double x, double y, const Color& c) {
        PointRz::draw(int(x), int(y), c, &image_);
    }

    void Rasterizer::drawLine(const Point2& p0, const Point2& p1, const Color& c) {
        LineRz::drawSeg(p0, p1, c, &image_);
    }

    void Rasterizer::drawQuadBezier(
        const Point2& p1, const Point2& p2, const Point2& p3, const Color& c)
    {
        BezierQuadRz::draw(p1, p2, p3, c, &image_);
    }

    void Rasterizer::drawQuadBezier(
        const Point2& p1, const Point2& p2, const Point2& p3,
        double w0, double w1, double w2,
        const Color& c)
    {
        BezierQuadRz::draw(p1, p2, p3, w0, w1, w2, c, &image_);
    }

    void Rasterizer::drawCubicBezier(
        const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4, const Color& c)
    {
        BezierCubicRz::draw(p1, p2, p3, p4, c, &image_);
    }

    void Rasterizer::drawCircle(const Point2& center, double r, const Color& c) {
        CircleRz::draw(center, int(r), c, &image_);
    }

    void Rasterizer::drawEllipse(const Point2& center, double a, double b, const Color& c) {
        CircleRz::draw(center, int(a), int(b), c, &image_);
    }

    void Rasterizer::drawTriangle(
        const Point2& p0, const Point2& p1, const Point2& p2,
        const Color& c0, const Color& c1, const Color& c2)
    {
        TriangleRz::draw(p0, p1, p2, c0, c1, c2, &image_);
    }

    const ImagePng* Rasterizer::getOutput() const {
        return &image_;
    }

}