#include "image.h"


namespace cyro {

    //////////
    Image::Image(int width, int height)
        : width_(width), height_(height)
    {
        data_.resize(width * height, Color());
    }

    void Image::setColor(int x, int y, const Color& c) {
        int index = x + y * width_;
        data_[index] = c;
    }

    Color Image::getColor(int x, int y) const {
        int index = x + y * width_;
        return data_[index];
    }


    //////////
    ImagePng::ImagePng(int width, int height)
        : width_(width), height_(height) {
        data_.resize(width * height, ColorBGRAInt());
    }

    ImagePng::ImagePng(int width, int height, const ColorBGRAInt& c)
        : width_(width),
          height_(height) {
        data_.resize(width * height, c);
    }

    void ImagePng::setColor(int x, int y, const ColorBGRAInt& c) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return;

        y = height_ > 0 ? (height_ - y - 1) : 0;
        int index = x + y * width_;
        data_[index] = c;
    }

    void ImagePng::drawColor(int x, int y, const ColorBGRAInt& c) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
        if (c.a_ == 0) return;

        auto cur = getColor(x, y);
        auto blend = cur * (1 - c.a_ / 255.f) + c * (c.a_ / 255.f);

        y = height_ > 0 ? (height_ - y - 1) : 0;
        int index = x + y * width_;
        data_[index] = blend;
    }

    ColorBGRAInt ImagePng::getColor(int x, int y) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return {};

        y = height_ > 0 ? (height_ - y - 1) : 0;
        int index = x + y * width_;
        return data_[index];
    }

}