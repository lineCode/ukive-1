#ifndef UKIVE_VIEWS_IMAGE_VIEW_H_
#define UKIVE_VIEWS_IMAGE_VIEW_H_

#include <memory>

#include "ukive/views/view.h"


namespace ukive {

    class Bitmap;
    class BitmapDrawable;

    class ImageView : public View
    {
    private:
        BitmapDrawable *mBitmapDrawable;

    public:
        ImageView(Window *w);
        ~ImageView();

        virtual void onMeasure(int width, int height, int widthSpec, int heightSpec);
        virtual void onDraw(Canvas *canvas);
        virtual bool onInputEvent(InputEvent *e);

        virtual void onSizeChanged(int width, int height, int oldWidth, int oldHeight);

        void setImageBitmap(std::shared_ptr<Bitmap> bitmap);
    };

}

#endif  // UKIVE_VIEWS_IMAGE_VIEW_H_