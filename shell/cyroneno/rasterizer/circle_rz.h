#ifndef CYRONENO_RASTERIZER_CIRCLE_H_
#define CYRONENO_RASTERIZER_CIRCLE_H_


namespace cyro {

    class Color;
    class Point2;
    class ImagePng;

    /**
     * Based on Alois Zingl "A Rasterizing Algorithm for Drawing Curves"
     */
    class CircleRz {
    public:
        static void draw(const Point2& center, int r, const Color& c, ImagePng* img);
        static void draw(const Point2& center, int a, int b, const Color& c, ImagePng* img);
        static void drawAA(const Point2& center, double r, const Color& c, ImagePng* img);

    private:
        static void drawOld(const Point2& center, int r, const Color& c, ImagePng* img);

    };

}

#endif  // CYRONENO_RASTERIZER_CIRCLE_H_