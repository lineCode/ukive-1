#ifndef CYRONENO_RASTERIZER_TRIANGLE_H_
#define CYRONENO_RASTERIZER_TRIANGLE_H_


namespace cyro {

    class Color;
    class Point2;
    class ImagePng;

    class TriangleRz {
    public:
        static void draw(
            const Point2& p0, const Point2& p1, const Point2& p2,
            const Color& c0, const Color& c1, const Color& c2, ImagePng* target);
        static void drawAA(
            const Point2& p0, const Point2& p1, const Point2& p2,
            const Color& c0, const Color& c1, const Color& c2, ImagePng* target);
    };

}

#endif  // CYRONENO_RASTERIZER_TRIANGLE_H_