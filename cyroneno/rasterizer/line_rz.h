#ifndef CYRONENO_RASTERIZER_LINE_H_
#define CYRONENO_RASTERIZER_LINE_H_

#include "../point.hpp"

namespace cyro {

    class Color;
    class ImagePng;

    /**
     * Based on Alois Zingl "A Rasterizing Algorithm for Drawing Curves"
     */
    class LineRz {
    public:
        static void draw(const Point2& p0, const Point2& p1, const Color& c, ImagePng* target);
        static void drawSeg(const Point2& p0, const Point2& p1, const Color& c, ImagePng* target);
        static void drawAA(const Point2& p0, const Point2& p1, const Color& c, ImagePng* target);
    };

}

#endif  // CYRONENO_RASTERIZER_LINE_H_