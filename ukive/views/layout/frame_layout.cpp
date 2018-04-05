#include "frame_layout.h"

#include <algorithm>

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    FrameLayout::FrameLayout(Window* wnd)
        :ViewGroup(wnd) {}


    void FrameLayout::onMeasure(int width, int height, int widthSpec, int heightSpec) {
        int finalWidth = 0;
        int finalHeight = 0;

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        measureChildrenWithMargins(width, height, widthSpec, heightSpec);

        switch (widthSpec) {
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

        switch (heightSpec) {
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

        setMeasuredDimension(finalWidth, finalHeight);
    }


    void FrameLayout::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        View* child = nullptr;
        LayoutParams* lp = nullptr;

        for (size_t i = 0; i < getChildCount(); ++i) {
            child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                lp = child->getLayoutParams();

                int width = child->getMeasuredWidth();
                int height = child->getMeasuredHeight();

                int left = getPaddingLeft() + lp->leftMargin;
                int top = getPaddingTop() + lp->topMargin;

                child->layout(
                    left, top,
                    width + left,
                    height + top);
            }
        }
    }

}