#ifndef UKIVE_VIEWS_IMAGE_VIEW_H_
#define UKIVE_VIEWS_IMAGE_VIEW_H_

#include "ukive/views/view.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class BitmapDrawable;

    class ImageView : public View
    {
    private:
        BitmapDrawable *mBitmapDrawable;

    public:
        ImageView(Window *wnd);
        ImageView(Window *wnd, int id);
        ~ImageView();

        virtual void onMeasure(int width, int height, int widthSpec, int heightSpec);
        virtual void onDraw(Canvas *canvas);
        virtual bool onInputEvent(InputEvent *e);

        virtual void onSizeChanged(int width, int height, int oldWidth, int oldHeight);

        void setImageBitmap(ComPtr<ID2D1Bitmap> bitmap);
    };

}

#endif  // UKIVE_VIEWS_IMAGE_VIEW_H_