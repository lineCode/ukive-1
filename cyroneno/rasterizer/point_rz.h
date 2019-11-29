#ifndef CYRONENO_RASTERIZER_POINT_H_
#define CYRONENO_RASTERIZER_POINT_H_


namespace cyro {

    class Color;
    class ImagePng;

    /**
     * Based on Alois Zingl "A Rasterizing Algorithm for Drawing Curves"
     */
    class PointRz {
    public:
        static void draw(int x, int y, const Color& c, ImagePng* target);
        static void drawAA(double x, double y, const Color& c, ImagePng* target);
    };

}

#endif  // CYRONENO_RASTERIZER_POINT_H_