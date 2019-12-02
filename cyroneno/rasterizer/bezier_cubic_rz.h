#ifndef CYRONENO_RASTERIZER_BEZIER_CUBIC_H_
#define CYRONENO_RASTERIZER_BEZIER_CUBIC_H_

#include "../point.hpp"


namespace cyro {

    class Color;
    class ImagePng;

    /**
     * Based on Alois Zingl "A Rasterizing Algorithm for Drawing Curves"
     */
    class BezierCubicRz {
    public:
        static void draw(
            const Point2I& p1, const Point2I& p2, const Point2I& p3, const Point2I& p4,
            const Color& c, ImagePng* target);

    private:
        static void drawSegment(
            const Point2I& p1, const Point2I& p2, const Point2I& p3, const Point2I& p4,
            const Color& c, ImagePng* target);

        static void drawParam(
            const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
            const Color& c, ImagePng* target);
    };

}

#endif  // CYRONENO_RASTERIZER_BEZIER_CUBIC_H_