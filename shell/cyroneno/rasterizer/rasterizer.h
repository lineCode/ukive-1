#ifndef CYRONENO_RASTERIZER_RASTERIZER_H_
#define CYRONENO_RASTERIZER_RASTERIZER_H_

#include "../color.h"
#include "../point.h"
#include "../image.h"


namespace cyro {

    class Rasterizer {
    public:
        Rasterizer(int image_width, int image_height);

        const ImagePng* getOutput() const;

        void drawPoint(double x, double y, const Color& c);
        void drawLine(const Point2& p0, const Point2& p1, const Color& c);
        void drawQuadBezier(const Point2& p1, const Point2& p2, const Point2& p3, const Color& c);
        void drawQuadBezier(
            const Point2& p1, const Point2& p2, const Point2& p3,
            double w0, double w1, double w2,
            const Color& c);
        void drawCubicBezier(
            const Point2& p1, const Point2& p2, const Point2& p3, const Point2& p4, const Color& c);
        void drawCircle(const Point2& center, double r, const Color& c);
        void drawEllipse(const Point2& center, double a, double b, const Color& c);
        void drawTriangle(
            const Point2& p0, const Point2& p1, const Point2& p2,
            const Color& c0, const Color& c1, const Color& c2);

    private:
        ImagePng image_;
    };

}

#endif  // CYRONENO_RASTERIZER_RASTERIZER_H_