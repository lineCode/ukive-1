#ifndef CYRONENO_IMAGE_H_
#define CYRONENO_IMAGE_H_

#include <vector>

#include "color.h"


namespace cyro {

    class Image {
    public:
        Image(size_t width, size_t height);

        void setColor(int x, int y, const Color& c);
        Color getColor(int x, int y) const;

        size_t width_, height_;
        std::vector<Color> data_;
    };


    class ImagePng {
    public:
        ImagePng(size_t width, size_t height);

        void setColor(int x, int y, const ColorBGRAInt& c);
        ColorBGRAInt getColor(int x, int y) const;

        size_t width_, height_;
        std::vector<ColorBGRAInt> data_;
    };

}

#endif  // CYRONENO_IMAGE_H_