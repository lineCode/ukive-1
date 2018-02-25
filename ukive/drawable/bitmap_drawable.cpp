#include "bitmap_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/log.h"


namespace ukive {

    BitmapDrawable::BitmapDrawable(std::shared_ptr<Bitmap> bitmap)
        :bitmap_(bitmap), opacity_(1.f) {}


    void BitmapDrawable::setOpacity(float opt) {
        opacity_ = opt;
    }

    void BitmapDrawable::draw(Canvas *canvas) {
        canvas->drawBitmap(getBounds().toRectF(), opacity_, bitmap_.get());
    }

    float BitmapDrawable::getOpacity() const {
        return opacity_;
    }

    int BitmapDrawable::getIncWidth() const {
        return bitmap_->getWidth();
    }

    int BitmapDrawable::getIncHeight() const {
        return bitmap_->getHeight();
    }

    std::shared_ptr<Bitmap> BitmapDrawable::getBitmap() const {
        return bitmap_;
    }
}