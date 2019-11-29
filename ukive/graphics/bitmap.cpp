#include "ukive/graphics/bitmap.h"

#include "utils/stl_utils.h"


namespace ukive {

    Bitmap::Bitmap(const ComPtr<ID2D1Bitmap> &source)
        : native_bitmap_(source) {}

    int Bitmap::getWidth() const {
        return utl::STLCInt(native_bitmap_->GetPixelSize().width);
    }

    int Bitmap::getHeight() const {
        return utl::STLCInt(native_bitmap_->GetPixelSize().height);
    }

    ComPtr<ID2D1Bitmap> Bitmap::getNative() const {
        return native_bitmap_;
    }
}