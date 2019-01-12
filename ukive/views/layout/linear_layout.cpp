#include "linear_layout.h"

#include <algorithm>
#include <typeinfo>

#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/utils/stl_utils.h"


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

    void LinearLayout::measureLinearChild(
        View* child, int parent_w, int parent_h, int parent_wm, int parent_hm)
    {
        auto child_lp = child->getLayoutParams();

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        int hori_margin = child_lp->leftMargin + child_lp->rightMargin;
        int vert_margin = child_lp->topMargin + child_lp->bottomMargin;

        int childWidth;
        int childWidthSpec;
        int childHeight;
        int childHeightSpec;

        getChildMeasure(
            parent_w, parent_wm,
            hori_margin + hori_padding,
            child_lp->width, &childWidth, &childWidthSpec);

        getChildMeasure(
            parent_h, parent_hm,
            vert_margin + vert_padding,
            child_lp->height, &childHeight, &childHeightSpec);

        child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
    }

    void LinearLayout::measureWeightedChildren(
        int total_weight, int parent_w, int parent_h, int parent_wm, int parent_hm)
    {
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
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
                    parent_w, parent_wm,
                    hori_margin + hori_padding,
                    child_lp->width, &childWidth, &childWidthSpec);

                getChildMeasure(
                    parent_h, parent_hm,
                    vert_margin + vert_padding,
                    child_lp->height, &childHeight, &childHeightSpec);

                if (orientation_ == VERTICAL) {
                    childHeight = std::round((child_lp->weight / static_cast<float>(total_weight))*parent_h);
                    childHeight = std::max(0, childHeight - vert_margin);
                } else if (orientation_ == HORIZONTAL) {
                    childWidth = std::round((child_lp->weight / static_cast<float>(total_weight))*parent_w);
                    childWidth = std::max(0, childWidth - hori_margin);
                }

                child->measure(
                    childWidth, childHeight,
                    childWidthSpec, childHeightSpec);
            }
        }
    }

    void LinearLayout::measureSequenceChildren(
        int parent_w, int parent_h, int parent_wm, int parent_hm)
    {
        for (int i = 0; i < getChildCount(); ++i) {
            auto child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                measureLinearChild(child, parent_w, parent_h, parent_wm, parent_hm);
            }
        }
    }

    void LinearLayout::measureFillModeChildren(
        int parent_w, int parent_h, int parent_wm, int parent_hm)
    {
        int cur_size = 0;
        View* delay_view = nullptr;
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                auto child_lp = child->getLayoutParams();
                bool need_delay = ((orientation_ == VERTICAL) ?
                    child_lp->height == LayoutParams::MATCH_PARENT : child_lp->width == LayoutParams::MATCH_PARENT);
                if (need_delay) {
                    delay_view = child;
                    continue;
                }

                int hori_padding = getPaddingLeft() + getPaddingRight();
                int vert_padding = getPaddingTop() + getPaddingBottom();

                int hori_margin = child_lp->leftMargin + child_lp->rightMargin;
                int vert_margin = child_lp->topMargin + child_lp->bottomMargin;

                int childWidth;
                int childWidthSpec;
                int childHeight;
                int childHeightSpec;

                getChildMeasure(
                    parent_w, parent_wm,
                    hori_margin + hori_padding,
                    child_lp->width, &childWidth, &childWidthSpec);

                getChildMeasure(
                    parent_h, parent_hm,
                    vert_margin + vert_padding,
                    child_lp->height, &childHeight, &childHeightSpec);

                child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
                cur_size += (orientation_ == VERTICAL) ?
                    child->getMeasuredHeight() + vert_margin : child->getMeasuredWidth() + hori_margin;
            }
        }

        if (delay_view) {
            if (orientation_ == VERTICAL) {
                parent_h = std::max(parent_h - cur_size, 0);
            } else {
                parent_w = std::max(parent_w - cur_size, 0);
            }
            measureLinearChild(delay_view, parent_w, parent_h, parent_wm, parent_hm);
        }
    }

    void LinearLayout::measureLinearLayoutChildren(
        int parent_w, int parent_h, int parent_wm, int parent_hm)
    {
        int total_weight = 0;
        bool fill_mode = false;

        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                auto lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());
                total_weight += lp->weight;
                if (lp->weight <= 0) {
                    fill_mode = ((orientation_ == VERTICAL) ?
                        lp->height == LayoutParams::MATCH_PARENT : lp->width == LayoutParams::MATCH_PARENT);
                }
            }
        }

        if (total_weight > 0) {
            measureWeightedChildren(
                total_weight, parent_w, parent_h, parent_wm, parent_hm);
        } else {
            if (fill_mode) {
                measureFillModeChildren(
                    parent_w, parent_h, parent_wm, parent_hm);
            } else {
                measureSequenceChildren(
                    parent_w, parent_h, parent_wm, parent_hm);
            }
        }
    }


    void LinearLayout::measureVertical(int width, int height, int width_mode, int height_mode) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        measureLinearLayoutChildren(width, height, width_mode, height_mode);

        int total_height = 0;

        switch (width_mode) {
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

        switch (height_mode) {
        case FIT: {
            for (int i = 0; i < getChildCount(); ++i) {
                auto child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    auto lp = child->getLayoutParams();
                    total_height += child->getMeasuredHeight() + lp->topMargin + lp->bottomMargin;
                }
            }
            final_height = std::min(height, total_height + vert_padding);
            final_height = std::max(getMinimumHeight(), final_height);
            break;
        }

        case UNKNOWN: {
            for (int i = 0; i < getChildCount(); ++i) {
                auto child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    auto lp = child->getLayoutParams();
                    total_height += child->getMeasuredHeight() + lp->topMargin + lp->bottomMargin;
                }
            }
            final_height = std::max(getMinimumHeight(), total_height + vert_padding);
            break;
        }

        case EXACTLY:
        default:
            final_height = height;
            break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void LinearLayout::measureHorizontal(int width, int height, int width_mode, int height_mode) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        measureLinearLayoutChildren(width, height, width_mode, height_mode);

        int total_width = 0;

        switch (width_mode) {
        case FIT: {
            for (int i = 0; i < getChildCount(); ++i) {
                auto child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    auto lp = child->getLayoutParams();
                    total_width += child->getMeasuredWidth() + lp->leftMargin + lp->rightMargin;
                }
            }
            final_width = std::min(total_width + hori_padding, width);
            final_width = std::max(getMinimumWidth(), final_width);
            break;
        }

        case UNKNOWN: {
            for (int i = 0; i < getChildCount(); ++i) {
                auto child = getChildAt(i);
                if (child->getVisibility() != View::VANISHED) {
                    auto lp = child->getLayoutParams();
                    total_width += child->getMeasuredWidth() + lp->leftMargin + lp->rightMargin;
                }
            }
            final_width = std::max(getMinimumWidth(), total_width + hori_padding);
            break;
        }

        case EXACTLY:
        default:
            final_width = width;
            break;
        }

        switch (height_mode) {
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

        setMeasuredSize(final_width, final_height);
    }


    void LinearLayout::layoutVertical(int left, int top, int right, int bottom) {
        int cur_top = getPaddingTop();

        for (int i = 0; i < getChildCount(); ++i) {
            auto child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                auto lp = child->getLayoutParams();

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
        int cur_left = getPaddingLeft();

        for (int i = 0; i < getChildCount(); ++i) {
            auto child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                auto lp = child->getLayoutParams();

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


    void LinearLayout::onMeasure(int width, int height, int width_mode, int height_mode) {
        if (orientation_ == VERTICAL) {
            measureVertical(width, height, width_mode, height_mode);
        } else {
            measureHorizontal(width, height, width_mode, height_mode);
        }
    }


    void LinearLayout::onLayout(
        bool changed, bool size_changed,
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
