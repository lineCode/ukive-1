#ifndef CYRONENO_IMAGE_H_
#define CYRONENO_IMAGE_H_

#include <vector>

#include "color.h"


namespace cyro {

    // һ��ͼ�����ݡ�����ԭ�������½�
    class Image {
    public:
        Image(int width, int height);

        void setColor(int x, int y, const Color& c);
        Color getColor(int x, int y) const;

        int width_, height_;
        std::vector<Color> data_;
    };


    // PNG ͼ�����ݡ�����ԭ�������½�
    class ImagePng {
    public:
        ImagePng(int width, int height);
        ImagePng(int width, int height, const ColorBGRAInt& c);

        void setColor(int x, int y, const ColorBGRAInt& c);
        void drawColor(int x, int y, const ColorBGRAInt& c);
        ColorBGRAInt getColor(int x, int y) const;

        int width_, height_;
        std::vector<ColorBGRAInt> data_;
    };

}

#endif  // CYRONENO_IMAGE_H_