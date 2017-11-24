﻿#include "image_view.h"

#include <algorithm>

#include "ukive/drawable/bitmap_drawable.h"


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

        int verticalPadding = mPaddingTop + mPaddingBottom;
        int horizontalPadding = mPaddingLeft + mPaddingRight;

        switch (widthSpec)
        {
        case FIT:
            if (mBitmapDrawable != 0)
                finalWidth = mBitmapDrawable->getIncWidth();

            finalWidth = std::max(mMinimumWidth, finalWidth + horizontalPadding);
            finalWidth = std::min(width, finalWidth);
            break;

        case UNKNOWN:
            if (mBitmapDrawable != 0)
                finalWidth = mBitmapDrawable->getIncWidth();

            finalWidth = std::max(mMinimumWidth, finalWidth + horizontalPadding);
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

            finalHeight = std::max(mMinimumHeight, finalHeight + verticalPadding);
            finalHeight = std::min(height, finalHeight);
            break;

        case UNKNOWN:
            if (mBitmapDrawable != 0)
                finalHeight = mBitmapDrawable->getIncHeight();

            finalHeight = std::max(mMinimumHeight, finalHeight + verticalPadding);
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


    void ImageView::setImageBitmap(ComPtr<ID2D1Bitmap> bitmap)
    {
        if (bitmap != nullptr)
        {
            D2D1_SIZE_F size = bitmap->GetSize();

            mBitmapDrawable = new BitmapDrawable(bitmap);
            mBitmapDrawable->setBound(0, 0, size.width, size.height);
        }
    }

}