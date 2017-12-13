#include "frame_layout.h"

#include <algorithm>

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    FrameLayout::FrameLayout(Window *wnd)
        :ViewGroup(wnd)
    {
    }

    FrameLayout::FrameLayout(Window *wnd, int id)
        : ViewGroup(wnd, id)
    {
    }

    FrameLayout::~FrameLayout()
    {
    }


    void FrameLayout::onMeasure(int width, int height, int widthSpec, int heightSpec)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        this->measureChildrenWithMargins(width, height, widthSpec, heightSpec);

        switch (widthSpec)
        {
        case FIT:
            finalWidth = getWrappedWidth();
            finalWidth = std::min(finalWidth + horizontalPadding, width);
            finalWidth = std::max(getMinimumWidth(), finalWidth);
            break;

        case UNKNOWN:
            finalWidth = getWrappedWidth();
            finalWidth = std::max(getMinimumWidth(), finalWidth);
            break;

        case EXACTLY:
            finalWidth = width;
            break;
        }

        switch (heightSpec)
        {
        case FIT:
            finalHeight = getWrappedHeight();
            finalHeight = std::min(finalHeight + verticalPadding, height);
            finalHeight = std::max(getMinimumHeight(), finalHeight);
            break;

        case UNKNOWN:
            finalHeight = getWrappedHeight();
            finalHeight = std::max(getMinimumHeight(), finalHeight);
            break;

        case EXACTLY:
            finalHeight = height;
            break;
        }

        this->setMeasuredDimension(finalWidth, finalHeight);
    }


    void FrameLayout::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        View *widget;
        LayoutParams *lp;

        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            widget = getChildAt(i);
            if (widget->getVisibility() != View::VANISHED)
            {
                lp = widget->getLayoutParams();

                int width = widget->getMeasuredWidth();
                int height = widget->getMeasuredHeight();

                int left = getPaddingLeft() + lp->leftMargin;
                int top = getPaddingTop() + lp->topMargin;

                widget->layout(
                    left,
                    top,
                    width + left,
                    height + top);
            }
        }
    }

}