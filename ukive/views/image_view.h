#ifndef UKIVE_VIEWS_IMAGE_VIEW_H_
#define UKIVE_VIEWS_IMAGE_VIEW_H_

#include <memory>

#include "ukive/views/view.h"


namespace ukive {

    class Bitmap;
    class BitmapDrawable;

    class ImageView : public View {
    public:
        ImageView(Window* w);
        ~ImageView();

        void onMeasure(int width, int height, int widthSpec, int heightSpec) override;
        void onSizeChanged(int width, int height, int oldWidth, int oldHeight) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        void setImageBitmap(std::shared_ptr<Bitmap> bitmap);

    private:
        BitmapDrawable* bitmap_drawable_;
    };

}

#endif  // UKIVE_VIEWS_IMAGE_VIEW_H_