#include "ukive/graphics/bitmap.h"

#include "ukive/utils/stl_utils.h"


namespace ukive {

    Bitmap::Bitmap(const ComPtr<ID2D1Bitmap> &source)
        : native_bitmap_(source) {}

    int Bitmap::getWidth() const {
        return STLCInt(native_bitmap_->GetPixelSize().width);
    }

    int Bitmap::getHeight() const {
        return STLCInt(native_bitmap_->GetPixelSize().height);
    }

    ComPtr<ID2D1Bitmap> Bitmap::getNative() const {
        return native_bitmap_;
    }
}