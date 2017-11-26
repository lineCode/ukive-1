#include "bitmap.h"


namespace ukive {

    Bitmap::Bitmap(const ComPtr<ID2D1Bitmap> &source) {
        native_bitmap_ = source;
    }


    ComPtr<ID2D1Bitmap> Bitmap::getNative() {
        return native_bitmap_;
    }
}