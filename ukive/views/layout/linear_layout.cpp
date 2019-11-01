#include "linear_layout.h"

#include <algorithm>
#include <typeinfo>

#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/utils/stl_utils.h"
#include "ukive/resources/dimension_utils.h"

#include "oigka/layout_constants.h"


namespace ukive {

    LinearLayout::LinearLayout(Window* w)
        : LinearLayout(w, {}) {}

    LinearLayout::LinearLayout(Window* w, AttrsRef attrs)
        : ViewGroup(w, attrs),
          orientation_(VERTICAL)
    {
        auto ori = resolveAttrString(
            attrs, oigka::kAttrLinearLayoutViewOri, oigka::kAttrValLinearLayoutViewOriVert);
        if (isEqual(ori, oigka::kAttrValLinearLayoutViewOriVert, false)) {
            orientation_ = VERTICAL;
        } else if (isEqual(ori, oigka::kAttrValLinearLayoutViewOriHori, false)) {
            orientation_ = HORIZONTAL;
        }
    }

    LayoutParams* LinearLayout::generateLayoutParams(const LayoutParams& lp) const {
        return new LinearLayoutParams(lp);
    }

    LayoutParams* LinearLayout::generateDefaultLayoutParams() const {
        return new LinearLayoutParams(
            LayoutParams::FIT_CONTENT,
            LayoutParams::FIT_CONTENT);
    }

    bool LinearLayout::checkLayoutParams(LayoutParams* lp) const {
        return typeid(*lp) == typeid(LinearLayoutParams);
    }

    LayoutParams* LinearLayout::generateLayoutParamsByAttrs(AttrsRef attrs) const {
        auto lp = ViewGroup::generateLayoutParamsByAttrs(attrs);
        auto llp = static_cast<LinearLayoutParams*>(generateLayoutParams(*lp));
        delete lp;

        auto weight = resolveAttrInt(attrs, oigka::kAttrLinearLayoutWeight, 0);
        llp->weight = weight;

        return llp;
    }

    void LinearLayout::measureLinearChild(
        View* child, int parent_w, int parent_h, int parent_wm, int parent_hm)
    {
        auto child_lp = child->getLayoutParams();

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        int hori_margin = child_lp->left_margin + child_lp->right_margin;
        int vert_margin = child_lp->top_margin + child_lp->bottom_margin;

        int child_width;
        int child_width_mode;
        int child_height;
        int child_height_mode;

        getChildMeasure(
            parent_w, parent_wm,
            hori_margin + hori_padding,
            child_lp->width, &child_width, &child_width_mode);

        getChildMeasure(
            parent_h, parent_hm,
            vert_margin + vert_padding,
            child_lp->height, &child_height, &child_height_mode);

        child->measure(child_width, child_height, child_width_mode, child_height_mode);
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

                int hori_margin = child_lp->left_margin + child_lp->right_margin;
                int vert_margin = child_lp->top_margin + child_lp->bottom_margin;

                int child_width;
                int child_width_mode;
                int child_height;
                int child_height_mode;

                getChildMeasure(
                    parent_w, parent_wm,
                    hori_margin + hori_padding,
                    child_lp->width, &child_width, &child_width_mode);

                getChildMeasure(
                    parent_h, parent_hm,
                    vert_margin + vert_padding,
                    child_lp->height, &child_height, &child_height_mode);

                if (orientation_ == VERTICAL) {
                    child_height = std::round((child_lp->weight / static_cast<float>(total_weight))*parent_h);
                    child_height = std::max(0, child_height - vert_margin);
                } else if (orientation_ == HORIZONTAL) {
                    child_width = std::round((child_lp->weight / static_cast<float>(total_weight))*parent_w);
                    child_width = std::max(0, child_width - hori_margin);
                }

                child->measure(
                    child_width, child_height,
                    child_width_mode, child_height_mode);
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

                int hori_margin = child_lp->left_margin + child_lp->right_margin;
                int vert_margin = child_lp->top_margin + child_lp->bottom_margin;

                int child_width;
                int child_width_mode;
                int child_height;
                int child_height_mode;

                getChildMeasure(
                    parent_w, parent_wm,
                    hori_margin + hori_padding,
                    child_lp->width, &child_width, &child_width_mode);

                getChildMeasure(
                    parent_h, parent_hm,
                    vert_margin + vert_padding,
                    child_lp->height, &child_height, &child_height_mode);

                child->measure(child_width, child_height, child_width_mode, child_height_mode);
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
                    fill_mode |= ((orientation_ == VERTICAL) ?
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
        int final_width;
        int final_height;

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
                    total_height += child->getMeasuredHeight() + lp->top_margin + lp->bottom_margin;
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
                    total_height += child->getMeasuredHeight() + lp->top_margin + lp->bottom_margin;
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
        int final_width;
        int final_height;

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
                    total_width += child->getMeasuredWidth() + lp->left_margin + lp->right_margin;
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
                    total_width += child->getMeasuredWidth() + lp->left_margin + lp->right_margin;
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

                cur_top += lp->top_margin;

                child->layout(
                    getPaddingLeft() + lp->left_margin,
                    cur_top,
                    getPaddingLeft() + lp->left_margin + width,
                    cur_top + height);

                cur_top += height + lp->bottom_margin;
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

                cur_left += lp->left_margin;

                child->layout(
                    cur_left,
                    getPaddingTop() + lp->top_margin,
                    cur_left + width,
                    getPaddingTop() + lp->top_margin + height);

                cur_left += width + lp->right_margin;
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
