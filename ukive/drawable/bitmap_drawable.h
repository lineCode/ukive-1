#ifndef UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_
#define UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_

#include "ukive/utils/com_ptr.h"
#include "ukive/drawable/drawable.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class BitmapDrawable : public Drawable
    {
    private:
        float mOpacity;
        ComPtr<ID2D1Bitmap> mBitmap;

    public:
        BitmapDrawable(ComPtr<ID2D1Bitmap> bitmap);
        ~BitmapDrawable();

        void setOpacity(float opt);

        virtual void draw(Canvas *canvas) override;

        virtual float getOpacity() override;

        virtual int getIncWidth() override;
        virtual int getIncHeight() override;

        ComPtr<ID2D1Bitmap> getBitmap();
    };

}

#endif  // UKIVE_DRAWABLE_BITMAP_DRAWABLE_H_