#include "image_view.h"

#include <algorithm>

#include "ukive/drawable/bitmap_drawable.h"
#include "ukive/graphics/bitmap.h"


namespace ukive {

    ImageView::ImageView(Window *wnd)
        :View(wnd)
    {
        mBitmapDrawable = 0;
    }

    ImageView::ImageView(Window *wnd, int id)
        : View(wnd, id)
    {
        mBitmapDrawable = 0;
    }

    ImageView::~ImageView()
    {
    }


    void ImageView::onMeasure(int width, int height, int widthSpec, int heightSpec)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        int verticalPadding = getPaddingTop() + getPaddingBottom();
        int horizontalPadding = getPaddingLeft() + getPaddingRight();

        switch (widthSpec)
        {
        case FIT:
            if (mBitmapDrawable != 0)
                finalWidth = mBitmapDrawable->getIncWidth();

            finalWidth = std::max(getMinimumWidth(), finalWidth + horizontalPadding);
            finalWidth = std::min(width, finalWidth);
            break;

        case UNKNOWN:
            if (mBitmapDrawable != 0)
                finalWidth = mBitmapDrawable->getIncWidth();

            finalWidth = std::max(getMinimumWidth(), finalWidth + horizontalPadding);
            break;

        case EXACTLY:
            finalWidth = width;
            break;
        }

        switch (heightSpec)
        {
        case FIT:
            if (mBitmapDrawable != 0)
                finalHeight = mBitmapDrawable->getIncHeight();

            finalHeight = std::max(getMinimumHeight(), finalHeight + verticalPadding);
            finalHeight = std::min(height, finalHeight);
            break;

        case UNKNOWN:
            if (mBitmapDrawable != 0)
                finalHeight = mBitmapDrawable->getIncHeight();

            finalHeight = std::max(getMinimumHeight(), finalHeight + verticalPadding);
            break;

        case EXACTLY:
            finalHeight = height;
            break;
        }

        setMeasuredDimension(finalWidth, finalHeight);
    }

    void ImageView::onDraw(Canvas *canvas)
    {
        View::onDraw(canvas);

        if (mBitmapDrawable)
            mBitmapDrawable->draw(canvas);
    }

    bool ImageView::onInputEvent(InputEvent *e)
    {
        View::onInputEvent(e);

        return false;
    }

    void ImageView::onSizeChanged(int width, int height, int oldWidth, int oldHeight)
    {
    }


    void ImageView::setImageBitmap(std::shared_ptr<Bitmap> bitmap)
    {
        if (bitmap != nullptr) {
            mBitmapDrawable = new BitmapDrawable(bitmap);
            mBitmapDrawable->setBounds(0, 0, bitmap->getWidth(), bitmap->getHeight());
        }
    }

}