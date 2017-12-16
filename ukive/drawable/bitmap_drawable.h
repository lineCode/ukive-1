#ifndef UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_
#define UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_

#include <memory>

#include "ukive/drawable/drawable.h"


namespace ukive {

    class Bitmap;

    class BitmapDrawable : public Drawable
    {
    private:
        float mOpacity;
        std::shared_ptr<Bitmap> mBitmap;

    public:
        BitmapDrawable(std::shared_ptr<Bitmap> bitmap);
        ~BitmapDrawable() = default;

        void setOpacity(float opt);

        void draw(Canvas *canvas) override;

        float getOpacity() override;

        int getIncWidth() override;
        int getIncHeight() override;

        std::shared_ptr<Bitmap> getBitmap();
    };

}

#endif  // UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_