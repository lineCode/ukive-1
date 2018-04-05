#ifndef UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_
#define UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_

#include <memory>

#include "ukive/drawable/drawable.h"


namespace ukive {

    class Bitmap;

    class BitmapDrawable : public Drawable {
    public:
        BitmapDrawable(std::shared_ptr<Bitmap> bitmap);
        ~BitmapDrawable() = default;

        void setOpacity(float opt);

        void draw(Canvas *canvas) override;

        float getOpacity() const override;

        int getIncWidth() const override;
        int getIncHeight() const override;

        std::shared_ptr<Bitmap> getBitmap() const;

    private:
        float opacity_;
        std::shared_ptr<Bitmap> bitmap_;
    };

}

#endif  // UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_