#ifndef UKIVE_GRAPHICS_BITMAP_H_
#define UKIVE_GRAPHICS_BITMAP_H_

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class Bitmap {
    public:
        Bitmap(const ComPtr<ID2D1Bitmap> &source);

        unsigned int getWidth() const;
        unsigned int getHeight() const;

        ComPtr<ID2D1Bitmap> getNative() const;

    private:
        ComPtr<ID2D1Bitmap> native_bitmap_;
    };

}

#endif  // UKIVE_GRAPHICS_BITMAP_H_
