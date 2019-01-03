#include "rasterizer.h"

#include <algorithm>

#include "../equations.h"

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
        ColorBGRAInt color(255 * c.r_, 255 * c.g_, 255 * c.b_, 255 * c.a_);

        image_.drawColor(x, y, color);
        return;

        auto dx = x - std::floor(x);
        auto dy = y - std::floor(y);
        if (dx == 0 && dy == 0) {
            // 不需要插值
            image_.drawColor(x, y, color);
        } else if (dx > 0 && dy == 0) {
            // x 轴插值
            image_.drawColor(x, y, color * (1 - dx));
            image_.drawColor(x + 1, y, color * dx);
        } else if (dy > 0 && dx == 0) {
            // y 轴插值
            image_.drawColor(x, y, color * (1 - dy));
            image_.drawColor(x, y + 1, color * dy);
        } else {
            // xy 轴插值
            image_.drawColor(x, y, color * ((1 - dx) * (1 - dy)));
            image_.drawColor(x + 1, y, color * (dx * (1 - dy)));
            image_.drawColor(x, y + 1, color * ((1 - dx) * dy));
            image_.drawColor(x + 1, y + 1, color * (dx * dy));
        }
    }

    void Rasterizer::drawLine(const Point2& p0, const Point2& p1, const Color& c) {
        if (p0.x_ <= p1.x_) {
            drawLineInternal(p0, p1, c);
        } else {
            drawLineInternal(p1, p0, c);
        }
    }

    void Rasterizer::drawLineInternal(const Point2& p0, const Point2& p1, const Color& c) {
        Line2DEqu line(p0, p1);
        if (line.m_ > 0 && line.m_ <= 1) {
            double y = p0.y_;
            for (double x = p0.x_; x <= p1.x_; ++x) {
                drawPoint(x, y, c);
                if (line.cal(x + 1, y + 0.5) < 0) {
                    ++y;
                }
            }
        } else if (line.m_ <= -1) {
            double x = p0.x_;
            for (double y = p0.y_; y >= p1.y_; --y) {
                drawPoint(x, y, c);
                if (line.cal(x + 0.5, y - 1) < 0) {
                    ++x;
                }
            }
        } else if (line.m_ > -1 && line.m_ <= 0) {
            double y = p0.y_;
            for (double x = p0.x_; x <= p1.x_; ++x) {
                drawPoint(x, y, c);
                if (line.cal(x + 1, y - 0.5) > 0) {
                    --y;
                }
            }
        } else {  // line.m_ > 1
            double x = p0.x_;
            for (double y = p0.y_; y <= p1.y_; ++y) {
                drawPoint(x, y, c);
                if (line.cal(x + 0.5, y + 1) > 0) {
                    ++x;
                }
            }
        }
    }

    void Rasterizer::drawTriangle(
        const Point2& p0, const Point2& p1, const Point2& p2,
        const Color& c0, const Color& c1, const Color& c2)
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
                double α = line01.cal(x, y) / line01.cal(p2.x_, p2.y_);
                double β = line02.cal(x, y) / line02.cal(p1.x_, p1.y_);
                double γ = line12.cal(x, y) / line12.cal(p0.x_, p0.y_);
                if (α > 0 && β > 0 && γ > 0) {
                    auto c = c2 * α + c1 * β + c0 * γ;
                    drawPoint(x, y, c);
                }
            }
        }
    }

    ImagePng Rasterizer::getOutput() const {
        return image_;
    }

}