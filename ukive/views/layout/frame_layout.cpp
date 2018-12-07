#include "frame_layout.h"

#include <algorithm>

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    FrameLayout::FrameLayout(Window* wnd)
        :ViewGroup(wnd) {}


    void FrameLayout::onMeasure(int width, int height, int widthSpec, int heightSpec) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        measureChildrenWithMargins(width, height, widthSpec, heightSpec);

        switch (widthSpec) {
        case FIT:
            final_width = getWrappedWidth();
            final_width = std::min(final_width + hori_padding, width);
            final_width = std::max(getMinimumWidth(), final_width);
            break;

        case UNKNOWN:
            final_width = getWrappedWidth();
            final_width = std::max(getMinimumWidth(), final_width);
            break;

        case EXACTLY:
        default:
            final_width = width;
            break;
        }

        switch (heightSpec) {
        case FIT:
            final_height = getWrappedHeight();
            final_height = std::min(final_height + vert_padding, height);
            final_height = std::max(getMinimumHeight(), final_height);
            break;

        case UNKNOWN:
            final_height = getWrappedHeight();
            final_height = std::max(getMinimumHeight(), final_height);
            break;

        case EXACTLY:
        default:
            final_height = height;
            break;
        }

        setMeasuredDimension(final_width, final_height);
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