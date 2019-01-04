#include "linear_layout.h"

#include <algorithm>
#include <typeinfo>

#include "ukive/views/layout/linear_layout_params.h"


namespace ukive {

    LinearLayout::LinearLayout(Window* w)
        :ViewGroup(w),
        orientation_(VERTICAL) {}


    LayoutParams* LinearLayout::generateLayoutParams(const LayoutParams& lp) {
        return new LinearLayoutParams(lp);
    }

    LayoutParams* LinearLayout::generateDefaultLayoutParams() {
        return new LinearLayoutParams(
            LayoutParams::FIT_CONTENT,
            LayoutParams::FIT_CONTENT);
    }

    bool LinearLayout::checkLayoutParams(LayoutParams* lp) {
        return typeid(*lp) == typeid(LinearLayoutParams);
    }


    void LinearLayout::measureWeightedChildren(
        int totalWeight,
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED)
            {
                auto child_lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

                int hori_padding = getPaddingLeft() + getPaddingRight();
                int vert_padding = getPaddingTop() + getPaddingBottom();

                int hori_margin = child_lp->leftMargin + child_lp->rightMargin;
                int vert_margin = child_lp->topMargin + child_lp->bottomMargin;

                int childWidth;
                int childWidthSpec;
                int childHeight;
                int childHeightSpec;

                getChildMeasure(
                    parentWidth, parentWidthMode,
                    hori_margin + hori_padding,
                    child_lp->width, &childWidth, &childWidthSpec);

                getChildMeasure(
                    parentHeight, parentHeightMode,
                    vert_margin + vert_padding,
                    child_lp->height, &childHeight, &childHeightSpec);

                if (orientation_ == VERTICAL) {
                    childHeight = std::round((child_lp->weight / (float)totalWeight)*parentHeight);
                    childHeight = std::max(0, childHeight - vert_margin);
                } else if (orientation_ == HORIZONTAL) {
                    childWidth = std::round((child_lp->weight / (float)totalWeight)*parentWidth);
                    childWidth = std::max(0, childWidth - hori_margin);
                }

                child->measure(
                    childWidth, childHeight,
                    childWidthSpec, childHeightSpec);
            }
        }
    }

    void LinearLayout::measureSequenceChildren(
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED)
            {
                LayoutParams* child_lp = child->getLayoutParams();

                int hori_padding = getPaddingLeft() + getPaddingRight();
                int vert_padding = getPaddingTop() + getPaddingBottom();

                int hori_margin = child_lp->leftMargin + child_lp->rightMargin;
                int vert_margin = child_lp->topMargin + child_lp->bottomMargin;

                int childWidth;
                int childWidthSpec;
                int childHeight;
                int childHeightSpec;

                getChildMeasure(
                    parentWidth, parentWidthMode,
                    hori_margin + hori_padding,
                    child_lp->width, &childWidth, &childWidthSpec);

                getChildMeasure(
                    parentHeight, parentHeightMode,
                    vert_margin + vert_padding,
                    child_lp->height, &childHeight, &childHeightSpec);

                child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
            }
        }
    }

    void LinearLayout::measureLinearLayoutChildren(
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode) {
        int total_weight = 0;

        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                auto lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());
                total_weight += lp->weight;
            }
        }

        if (total_weight > 0) {
            measureWeightedChildren(
                total_weight,
                parentWidth, parentHeight,
                parentWidthMode, parentHeightMode);
        } else {
            measureSequenceChildren(
                parentWidth, parentHeight,
                parentWidthMode, parentHeightMode);
        }
    }


    void LinearLayout::measureVertical(int width, int height, int widthSpec, int heightSpec) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        measureLinearLayoutChildren(width, height, widthSpec, heightSpec);

        int total_height = 0;

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
            final_width = width;
            break;
        }

        switch (heightSpec) {
        case FIT: {
            View* child = nullptr;
            LayoutParams* lp = nullptr;
            for (int i = 0; i < getChildCount(); ++i) {
                child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    lp = child->getLayoutParams();
                    total_height += child->getMeasuredHeight() + lp->topMargin + lp->bottomMargin;
                }
            }
            final_height = std::min(height, total_height + vert_padding);
            final_height = std::max(getMinimumHeight(), final_height);
            break;
        }

        case UNKNOWN: {
            View* child = nullptr;
            LayoutParams* lp = nullptr;
            for (int i = 0; i < getChildCount(); ++i) {
                child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    lp = child->getLayoutParams();
                    total_height += child->getMeasuredHeight() + lp->topMargin + lp->bottomMargin;
                }
            }
            final_height = std::max(getMinimumHeight(), total_height + vert_padding);
            break;
        }

        case EXACTLY:
            final_height = height;
            break;
        }

        setMeasuredDimension(final_width, final_height);
    }

    void LinearLayout::measureHorizontal(int width, int height, int widthSpec, int heightSpec) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        measureLinearLayoutChildren(width, height, widthSpec, heightSpec);

        int total_width = 0;

        switch (widthSpec) {
        case FIT: {
            View* child = nullptr;
            LayoutParams* lp = nullptr;
            for (int i = 0; i < getChildCount(); ++i) {
                child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    lp = child->getLayoutParams();
                    total_width += child->getMeasuredWidth() + lp->leftMargin + lp->rightMargin;
                }
            }
            final_width = std::min(total_width + hori_padding, width);
            final_width = std::max(getMinimumWidth(), final_width);
            break;
        }

        case UNKNOWN: {
            View* child = nullptr;
            LayoutParams* lp = nullptr;
            for (int i = 0; i < getChildCount(); ++i) {
                child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    lp = child->getLayoutParams();
                    total_width += child->getMeasuredWidth() + lp->leftMargin + lp->rightMargin;
                }
            }
            final_width = std::max(getMinimumWidth(), total_width + hori_padding);
            break;
        }

        case EXACTLY:
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
            final_height = height;
            break;
        }

        setMeasuredDimension(final_width, final_height);
    }


    void LinearLayout::layoutVertical(int left, int top, int right, int bottom) {
        View* child = nullptr;
        LayoutParams* lp = nullptr;

        int cur_top = getPaddingTop();

        for (int i = 0; i < getChildCount(); ++i) {
            child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                lp = child->getLayoutParams();

                int width = child->getMeasuredWidth();
                int height = child->getMeasuredHeight();

                cur_top += lp->topMargin;

                child->layout(
                    getPaddingLeft() + lp->leftMargin,
                    cur_top,
                    getPaddingLeft() + lp->leftMargin + width,
                    cur_top + height);

                cur_top += height + lp->bottomMargin;
            }
        }
    }

    void LinearLayout::layoutHorizontal(int left, int top, int right, int bottom) {
        View* child = nullptr;
        LayoutParams* lp = nullptr;

        int cur_left = getPaddingLeft();

        for (int i = 0; i < getChildCount(); ++i) {
            child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                lp = child->getLayoutParams();

                int width = child->getMeasuredWidth();
                int height = child->getMeasuredHeight();

                cur_left += lp->leftMargin;

                child->layout(
                    cur_left,
                    getPaddingTop() + lp->topMargin,
                    cur_left + width,
                    getPaddingTop() + lp->topMargin + height);

                cur_left += width + lp->rightMargin;
            }
        }
    }


    void LinearLayout::onMeasure(int width, int height, int widthSpec, int heightSpec) {
        if (orientation_ == VERTICAL) {
            measureVertical(width, height, widthSpec, heightSpec);
        } else {
            measureHorizontal(width, height, widthSpec, heightSpec);
        }
    }


    void LinearLayout::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        if (orientation_ == VERTICAL) {
            layoutVertical(left, top, right, bottom);
        } else {
            layoutHorizontal(left, top, right, bottom);
        }
    }


    void LinearLayout::setOrientation(int orientation) {
        if (orientation == orientation_) {
            return;
        }

        orientation_ = orientation;

        requestLayout();
        invalidate();
    }

}