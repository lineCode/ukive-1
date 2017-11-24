#include "bitmap_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/log.h"


namespace ukive {

    BitmapDrawable::BitmapDrawable(ComPtr<ID2D1Bitmap> bitmap)
        :Drawable(),
        mBitmap(bitmap),
        mOpacity(1.f)
    {
    }


    BitmapDrawable::~BitmapDrawable()
    {
    }


    void BitmapDrawable::setOpacity(float opt)
    {
        if (opt < 0.f)
            opt = 0.f;

        if (opt > 1.f)
            opt = 1.f;

        mOpacity = opt;
    }


    void BitmapDrawable::draw(Canvas *canvas)
    {
        canvas->drawBitmap(this->getBound(), mOpacity, mBitmap.get());
    }

    float BitmapDrawable::getOpacity()
    {
        return mOpacity;
    }

    int BitmapDrawable::getIncWidth()
    {
        D2D1_SIZE_F size = mBitmap->GetSize();
        return size.width;
    }

    int BitmapDrawable::getIncHeight()
    {
        D2D1_SIZE_F size = mBitmap->GetSize();
        return size.height;
    }


    ComPtr<ID2D1Bitmap> BitmapDrawable::getBitmap()
    {
        return mBitmap;
    }

}