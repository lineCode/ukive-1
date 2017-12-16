#include "bitmap_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/log.h"


namespace ukive {

    BitmapDrawable::BitmapDrawable(std::shared_ptr<Bitmap> bitmap)
        :mBitmap(bitmap), mOpacity(1.f) {}


    void BitmapDrawable::setOpacity(float opt)
    {
        if (opt < 0.f)
            opt = 0.f;

        if (opt > 1.f)
            opt = 1.f;

        mOpacity = opt;
    }


    void BitmapDrawable::draw(Canvas *canvas) {
        canvas->drawBitmap(getBound(), mOpacity, mBitmap.get());
    }

    float BitmapDrawable::getOpacity() {
        return mOpacity;
    }

    int BitmapDrawable::getIncWidth() {
        return mBitmap->getWidth();
    }

    int BitmapDrawable::getIncHeight() {
        return mBitmap->getHeight();
    }


    std::shared_ptr<Bitmap> BitmapDrawable::getBitmap()
    {
        return mBitmap;
    }

}