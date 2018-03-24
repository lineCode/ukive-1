#include "image_view.h"

#include <algorithm>

#include "ukive/drawable/bitmap_drawable.h"
#include "ukive/graphics/bitmap.h"


namespace ukive {

    ImageView::ImageView(Window *wnd)
        :View(wnd),
        bitmap_drawable_(nullptr) {}

    ImageView::~ImageView() {
    }


    void ImageView::onMeasure(int width, int height, int widthSpec, int heightSpec) {
        int finalWidth = 0;
        int finalHeight = 0;

        int verticalPadding = getPaddingTop() + getPaddingBottom();
        int horizontalPadding = getPaddingLeft() + getPaddingRight();

        switch (widthSpec) {
        case FIT:
            if (bitmap_drawable_ != nullptr) {
                finalWidth = bitmap_drawable_->getIncWidth();
            }

            finalWidth = std::max(getMinimumWidth(), finalWidth + horizontalPadding);
            finalWidth = std::min(width, finalWidth);
            break;

        case UNKNOWN:
            if (bitmap_drawable_ != nullptr) {
                finalWidth = bitmap_drawable_->getIncWidth();
            }

            finalWidth = std::max(getMinimumWidth(), finalWidth + horizontalPadding);
            break;

        case EXACTLY:
            finalWidth = width;
            break;
        }

        switch (heightSpec) {
        case FIT:
            if (bitmap_drawable_ != nullptr) {
                finalHeight = bitmap_drawable_->getIncHeight();
            }

            finalHeight = std::max(getMinimumHeight(), finalHeight + verticalPadding);
            finalHeight = std::min(height, finalHeight);
            break;

        case UNKNOWN:
            if (bitmap_drawable_ != nullptr) {
                finalHeight = bitmap_drawable_->getIncHeight();
            }

            finalHeight = std::max(getMinimumHeight(), finalHeight + verticalPadding);
            break;

        case EXACTLY:
            finalHeight = height;
            break;
        }

        setMeasuredDimension(finalWidth, finalHeight);
    }

    void ImageView::onDraw(Canvas *canvas) {
        View::onDraw(canvas);

        if (bitmap_drawable_) {
            bitmap_drawable_->draw(canvas);
        }
    }

    bool ImageView::onInputEvent(InputEvent *e) {
        return View::onInputEvent(e);
    }

    void ImageView::onSizeChanged(int width, int height, int oldWidth, int oldHeight) {
    }


    void ImageView::setImageBitmap(std::shared_ptr<Bitmap> bitmap)
    {
        if (bitmap != nullptr) {
            bitmap_drawable_ = new BitmapDrawable(bitmap);
            bitmap_drawable_->setBounds(0, 0, bitmap->getWidth(), bitmap->getHeight());
        }
    }

}