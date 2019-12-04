#include "ukive/graphics/bitmap.h"

#include "utils/stl_utils.h"


namespace ukive {

    // Bitmap
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


    // SWBitmap
    SWBitmap::SWBitmap(const ComPtr<IWICBitmap> &source)
        : native_bitmap_(source) {}

    int SWBitmap::getWidth() const {
        UINT width = 0, height = 0;
        HRESULT hr = native_bitmap_->GetSize(&width, &height);
        if (FAILED(hr)) {
            DCHECK(false);
            return 0;
        }
        return width;
    }
    int SWBitmap::getHeight() const {
        UINT width = 0, height = 0;
        HRESULT hr = native_bitmap_->GetSize(&width, &height);
        if (FAILED(hr)) {
            DCHECK(false);
            return 0;
        }
        return height;
    }

    ComPtr<IWICBitmap> SWBitmap::getNative() const {
        return native_bitmap_;
    }
}