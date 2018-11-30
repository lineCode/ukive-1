#include "image.h"


namespace cyro {

    //////////
    Image::Image(size_t width, size_t height)
        : width_(width), height_(height)
    {
        data_.resize(width * height, Color());
    }

    void Image::setColor(int x, int y, const Color& c) {
        size_t index = x + y * width_;
        data_[index] = c;
    }

    Color Image::getColor(int x, int y) const {
        size_t index = x + y * width_;
        return data_[index];
    }


    //////////
    ImagePng::ImagePng(size_t width, size_t height)
        : width_(width), height_(height) {
        data_.resize(width * height, ColorBGRAInt());
    }

    void ImagePng::setColor(int x, int y, const ColorBGRAInt& c) {
        size_t index = x + y * width_;
        data_[index] = c;
    }

    ColorBGRAInt ImagePng::getColor(int x, int y) const {
        size_t index = x + y * width_;
        return data_[index];
    }

}