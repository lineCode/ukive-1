#ifndef CYRONENO_PIPELINE_RASTERIZER_H_
#define CYRONENO_PIPELINE_RASTERIZER_H_

#include "../color.h"
#include "../point.h"
#include "../image.h"


namespace cyro {

    class Rasterizer {
    public:
        Rasterizer(int image_width, int image_height);

        ImagePng getOutput() const;

        void drawPoint(double x, double y, const Color& c);
        void drawLine(
            const Point2& p0, const Point2& p1, const Color& c);
        void drawTriangle(
            const Point2& p0, const Point2& p1, const Point2& p2,
            const Color& c0, const Color& c1, const Color& c2);

    private:
        void drawLineInternal(
            const Point2& p0, const Point2& p1, const Color& c);

        ImagePng image_;
    };

}

#endif  // CYRONENO_PIPELINE_RASTERIZER_H_