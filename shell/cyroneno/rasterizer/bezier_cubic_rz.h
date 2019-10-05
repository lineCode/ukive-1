#ifndef CYRONENO_RASTERIZER_BEZIER_CUBIC_H_
#define CYRONENO_RASTERIZER_BEZIER_CUBIC_H_


namespace cyro {

    class Color;
    class Point2;
    class ImagePng;

    /**
     * Based on Alois Zingl "A Rasterizing Algorithm for Drawing Curves"
     */
    class BezierCubicRz {
    public:
        static void draw(
            const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
            const Color& c, ImagePng* target);

    private:
        static void drawSegment(
            const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
            const Color& c, ImagePng* target);

        static void drawParam(
            const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4,
            const Color& c, ImagePng* target);
    };

}

#endif  // CYRONENO_RASTERIZER_BEZIER_CUBIC_H_