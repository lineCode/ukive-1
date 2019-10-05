#ifndef CYRONENO_RASTERIZER_BEZIER_QUAD_H_
#define CYRONENO_RASTERIZER_BEZIER_QUAD_H_


namespace cyro {

    class Color;
    class Point2;
    class ImagePng;

    /**
     * Based on Alois Zingl "A Rasterizing Algorithm for Drawing Curves"
     */
    class BezierQuadRz {
    public:
        static void draw(
            const Point2& p1, const Point2& p2, const Point2& p3,
            const Color& c, ImagePng* target);
        static void draw(
            const Point2& p1, const Point2& p2, const Point2& p3,
            double w0, double w1, double w2,
            const Color& c, ImagePng* target);
        static void drawAA(
            const Point2& p1, const Point2& p2, const Point2& p3, const Color& c, ImagePng* target);

    private:
        static void drawSegment(
            const Point2& p1, const Point2& p2, const Point2& p3, const Color& c, ImagePng* target);

        static void drawParam(
            const Point2& p1, const Point2& p2, const Point2& p3,
            const Color& c, ImagePng* target);
        static void drawParam(
            const Point2& p1, const Point2& p2, const Point2& p3,
            double w0, double w1, double w2,
            const Color& c, ImagePng* target);
    };

}

#endif  // CYRONENO_RASTERIZER_BEZIER_QUAD_H_