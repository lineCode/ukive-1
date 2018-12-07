#include "bitmap.h"


namespace ukive {

    Bitmap::Bitmap(const ComPtr<ID2D1Bitmap> &source) {
        native_bitmap_ = source;
    }


    unsigned int Bitmap::getWidth() const {
        return native_bitmap_->GetPixelSize().width;
    }

    unsigned int Bitmap::getHeight() const {
        return native_bitmap_->GetPixelSize().height;
    }


    ComPtr<ID2D1Bitmap> Bitmap::getNative() const {
        return native_bitmap_;
    }
}