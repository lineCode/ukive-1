#include "restraint_layout.h"

#include <algorithm>
#include <typeinfo>

#include "utils/log.h"

#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/resources/dimension_utils.h"

#include "oigka/layout_constants.h"


namespace ukive {

    namespace {

        RestraintLayoutParams::Edge resolveEdge(const string16& sh_edge_str) {
            RestraintLayoutParams::Edge sh_edge = RestraintLayoutParams::Edge::START;
            if (utl::isEqual(sh_edge_str, oigka::kAttrValRestraintLayoutHEStart, false)) {
                sh_edge = RestraintLayoutParams::Edge::START;
            } else if (utl::isEqual(sh_edge_str, oigka::kAttrValRestraintLayoutHEEnd, false)) {
                sh_edge = RestraintLayoutParams::Edge::END;
            } else if (utl::isEqual(sh_edge_str, oigka::kAttrValRestraintLayoutHETop, false)) {
                sh_edge = RestraintLayoutParams::Edge::TOP;
            } else if (utl::isEqual(sh_edge_str, oigka::kAttrValRestraintLayoutHEBottom, false)) {
                sh_edge = RestraintLayoutParams::Edge::BOTTOM;
            }
            return sh_edge;
        }

    }


    RestraintLayout::RestraintLayout(Window* w)
        : RestraintLayout(w, {}) {}

    RestraintLayout::RestraintLayout(Window* w, AttrsRef attrs)
        : ViewGroup(w, attrs) {}

    LayoutParams* RestraintLayout::generateLayoutParams(const LayoutParams &lp) const {
        return new RestraintLayoutParams(lp);
    }

    LayoutParams* RestraintLayout::generateDefaultLayoutParams() const {
        return new RestraintLayoutParams(
            LayoutParams::FIT_CONTENT, LayoutParams::FIT_CONTENT);
    }

    LayoutParams* RestraintLayout::generateLayoutParamsByAttrs(AttrsRef attrs) const {
        auto lp = ViewGroup::generateLayoutParamsByAttrs(attrs);
        auto rlp = static_cast<RestraintLayoutParams*>(generateLayoutParams(*lp));
        delete lp;

        // Start Handle
        auto sh_id = resolveAttrInt(
            attrs, oigka::kAttrRestraintLayoutSHId, -1);
        if (sh_id != -1) {
            auto sh_edge_str = resolveAttrString(
                attrs, oigka::kAttrRestraintLayoutSHEdge, oigka::kAttrValRestraintLayoutHEStart);
            auto sh_edge = resolveEdge(sh_edge_str);
            auto sh_margin = resolveAttrDimension(
                getWindow(), attrs, oigka::kAttrRestraintLayoutSHMargin, 0);
            rlp->startHandle(sh_id, sh_edge, sh_margin);
        }

        // End Handle
        sh_id = resolveAttrInt(
            attrs, oigka::kAttrRestraintLayoutEHId, -1);
        if (sh_id != -1) {
            auto sh_edge_str = resolveAttrString(
                attrs, oigka::kAttrRestraintLayoutEHEdge, oigka::kAttrValRestraintLayoutHEEnd);
            auto sh_edge = resolveEdge(sh_edge_str);
            auto sh_margin = resolveAttrDimension(
                getWindow(), attrs, oigka::kAttrRestraintLayoutEHMargin, 0);
            rlp->endHandle(sh_id, sh_edge, sh_margin);
        }

        // Top Handle
        sh_id = resolveAttrInt(
            attrs, oigka::kAttrRestraintLayoutTHId, -1);
        if (sh_id != -1) {
            auto sh_edge_str = resolveAttrString(
                attrs, oigka::kAttrRestraintLayoutTHEdge, oigka::kAttrValRestraintLayoutHETop);
            auto sh_edge = resolveEdge(sh_edge_str);
            auto sh_margin = resolveAttrDimension(
                getWindow(), attrs, oigka::kAttrRestraintLayoutTHMargin, 0);
            rlp->topHandle(sh_id, sh_edge, sh_margin);
        }

        // Bottom Handle
        sh_id = resolveAttrInt(
            attrs, oigka::kAttrRestraintLayoutBHId, -1);
        if (sh_id != -1) {
            auto sh_edge_str = resolveAttrString(
                attrs, oigka::kAttrRestraintLayoutBHEdge, oigka::kAttrValRestraintLayoutHEBottom);
            auto sh_edge = resolveEdge(sh_edge_str);
            auto sh_margin = resolveAttrDimension(
                getWindow(), attrs, oigka::kAttrRestraintLayoutBHMargin, 0);
            rlp->bottomHandle(sh_id, sh_edge, sh_margin);
        }
        return rlp;
    }

    bool RestraintLayout::checkLayoutParams(LayoutParams* lp) const {
        return typeid(*lp) == typeid(RestraintLayoutParams);
    }

    void RestraintLayout::clearMeasureFlag() {
        auto count = getChildCount();
        for (int i = 0; i < count; ++i) {
            View* child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());
            lp->is_width_measured = lp->is_height_measured = false;
            lp->is_vert_layouted = lp->is_hori_layouted = false;
        }
    }

    void RestraintLayout::measureRestrainedChildren(
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        auto count = getChildCount();
        for (int i = 0; i < count; ++i) {
            View* child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());
            if (lp->is_width_measured) {
                continue;
            }

            int childWidth = 0;
            int childWidthMode = 0;

            getRestrainedChildWidth(
                child, lp,
                parentWidth, parentWidthMode,
                &childWidth, &childWidthMode);

            lp->is_width_measured = true;
            lp->spec_width = childWidth;
            lp->spec_width_mode = childWidthMode;
        }

        for (int i = 0; i < count; ++i) {
            View* child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());
            if (lp->is_width_measured && lp->is_height_measured) {
                continue;
            }

            int childHeight = 0;
            int childHeightMode = 0;

            if (!lp->is_height_measured) {
                getRestrainedChildHeight(
                    child, lp,
                    parentHeight, parentHeightMode,
                    &childHeight, &childHeightMode);

                lp->is_height_measured = true;
                lp->spec_height = childHeight;
                lp->spec_height_mode = childHeightMode;
            }
            else {
                childHeight = lp->spec_height;
                childHeightMode = lp->spec_height_mode;
            }

            child->measure(lp->spec_width, childHeight, lp->spec_width_mode, childHeightMode);
        }
    }

    void RestraintLayout::checkRestrainedChildrenWeight() {
        auto count = getChildCount();
        for (int i = 0; i < count; ++i) {
            View* child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());
        }
    }

    void RestraintLayout::getRestrainedChildWidth(
        View* child, RestraintLayoutParams* lp,
        int parentWidth, int parentWidthMode, int* width, int* widthMode)
    {
        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int horizontalMargins = lp->left_margin + lp->right_margin;

        int childWidth = 0;
        int childWidthMode = 0;

        if (lp->hasHorizontalCouple())
        {
            int size = std::max(0,
                parentWidth - horizontalPadding - horizontalMargins);

            // child 有固定的大小。
            if (lp->width > 0) {
                childWidth = lp->width;
                childWidthMode = EXACTLY;
                lp->hori_couple_handler_type = RestraintLayoutParams::CH_FIXED;
            }
            // child 将填充 handler couple 之间的区域。
            else {
                // handler couple 绑定于父 View。
                if (lp->start_handled_id == getId()
                    && lp->end_handled_id == getId())
                {
                    switch (parentWidthMode) {
                    case FIT:
                    {
                        childWidth = size;
                        childWidthMode = FIT;
                        lp->hori_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                        break;
                    }
                    case EXACTLY:
                    {
                        if (lp->width == 0
                            || lp->width == LayoutParams::MATCH_PARENT)
                        {
                            childWidth = size;
                            childWidthMode = EXACTLY;
                            lp->hori_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->width == LayoutParams::FIT_CONTENT)
                        {
                            childWidth = size;
                            childWidthMode = FIT;
                            lp->hori_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                        }
                        break;
                    }
                    case UNKNOWN:
                    {
                        // 此处，由于不知道父 View 的大小，无法填充 handler couple 之间的区域。
                        // 让 View 自己决定大小即可。
                        childWidth = size;
                        childWidthMode = UNKNOWN;
                        lp->hori_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        break;
                    }
                    }
                }
                // handler couple 没有绑定于父 View。这意味着它与其他子 View 绑定。
                // 此时必须先测量绑定到的 View，然后才能测量该 View。将使用
                // 递归完成此过程，为此需要额外的变量来记录某一 View 是否已测量。
                else
                {
                    // 左右 Handler 绑定在同一个 View 上。
                    if (lp->start_handled_id == lp->end_handled_id)
                    {
                        int measuredTargetWidth = 0;
                        View* target = getChildById(lp->start_handled_id);
                        auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

                        // 测量此 target view 的宽度。
                        int targetWidth;
                        int targetWidthMode;
                        if (!targetLp->is_width_measured)
                        {
                            getRestrainedChildWidth(
                                target, targetLp,
                                parentWidth, parentWidthMode,
                                &targetWidth, &targetWidthMode);

                            targetLp->is_width_measured = true;
                            targetLp->spec_width = targetWidth;
                            targetLp->spec_width_mode = targetWidthMode;
                        }
                        else
                        {
                            targetWidth = targetLp->spec_width;
                            targetWidthMode = targetLp->spec_width_mode;
                        }

                        int targetHeight = 0;
                        int targetHeightMode = UNKNOWN;
                        if (targetLp->is_height_measured)
                        {
                            targetHeight = targetLp->spec_height;
                            targetHeightMode = targetLp->spec_height_mode;
                        }

                        // 让 target view 测量自身。
                        // 这将会使 target view 的 onMeasure() 方法多调用一次。
                        target->measure(
                            targetWidth, targetHeight,
                            targetWidthMode, targetHeightMode);
                        measuredTargetWidth = target->getMeasuredWidth();

                        childWidth = std::max(0, measuredTargetWidth - horizontalMargins);

                        if (lp->width == 0
                            || lp->width == LayoutParams::MATCH_PARENT)
                        {
                            childWidthMode = EXACTLY;
                            lp->hori_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->width == LayoutParams::FIT_CONTENT)
                        {
                            childWidthMode = FIT;
                            lp->hori_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                        }
                    }
                    else
                    {
                        if (parentWidthMode == EXACTLY
                            || parentWidthMode == FIT)
                        {
                            // 前向遍历。
                            int measuredStartMargin = lp->left_margin;
                            RestraintLayoutParams* childLp = lp;
                            while (childLp->hasStart()
                                && childLp->start_handled_id != this->getId())
                            {
                                View* target = getChildById(childLp->start_handled_id);
                                auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

                                // 测量此 target view 的宽度。
                                int targetWidth;
                                int targetWidthMode;
                                if (!targetLp->is_width_measured)
                                {
                                    getRestrainedChildWidth(
                                        target, targetLp,
                                        parentWidth, parentWidthMode,
                                        &targetWidth, &targetWidthMode);

                                    targetLp->is_width_measured = true;
                                    targetLp->spec_width = targetWidth;
                                    targetLp->spec_width_mode = targetWidthMode;
                                }
                                else
                                {
                                    targetWidth = targetLp->spec_width;
                                    targetWidthMode = targetLp->spec_width_mode;
                                }

                                int targetHeight = 0;
                                int targetHeightMode = UNKNOWN;
                                if (targetLp->is_height_measured)
                                {
                                    targetHeight = targetLp->spec_height;
                                    targetHeightMode = targetLp->spec_height_mode;
                                }

                                // 让 target view 测量自身。
                                // 这将会使 target view 的 onMeasure() 方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->start_handled_edge
                                    == RestraintLayoutParams::END)
                                {
                                    measuredStartMargin += target->getMeasuredWidth()
                                        + (targetLp->hasStart() ? targetLp->left_margin : 0);
                                }
                                else if (childLp->start_handled_edge
                                    == RestraintLayoutParams::START)
                                {
                                    measuredStartMargin +=
                                        (targetLp->hasStart() ? targetLp->left_margin : 0);
                                }

                                childLp = targetLp;
                            }

                            // 后向遍历。
                            int measuredEndMargin = lp->right_margin;
                            childLp = lp;
                            while (childLp->hasEnd()
                                && childLp->end_handled_id != this->getId())
                            {
                                View* target = getChildById(childLp->end_handled_id);
                                auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

                                // 测量此 target view 的宽度。
                                int targetWidth;
                                int targetWidthMode;
                                if (!targetLp->is_width_measured)
                                {
                                    getRestrainedChildWidth(
                                        target, targetLp,
                                        parentWidth, parentWidthMode,
                                        &targetWidth, &targetWidthMode);

                                    targetLp->is_width_measured = true;
                                    targetLp->spec_width = targetWidth;
                                    targetLp->spec_width_mode = targetWidthMode;
                                }
                                else
                                {
                                    targetWidth = targetLp->spec_width;
                                    targetWidthMode = targetLp->spec_width_mode;
                                }

                                int targetHeight = 0;
                                int targetHeightMode = UNKNOWN;
                                if (targetLp->is_height_measured)
                                {
                                    targetHeight = targetLp->spec_height;
                                    targetHeightMode = targetLp->spec_height_mode;
                                }

                                // 让 target view 测量自身。
                                // 这将会使 target view 的 onMeasure() 方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->end_handled_edge
                                    == RestraintLayoutParams::START)
                                {
                                    measuredEndMargin += target->getMeasuredWidth()
                                        + (targetLp->hasEnd() ? targetLp->right_margin : 0);
                                }
                                else if (childLp->end_handled_edge
                                    == RestraintLayoutParams::END)
                                {
                                    measuredEndMargin +=
                                        (targetLp->hasEnd() ? targetLp->right_margin : 0);
                                }

                                childLp = targetLp;
                            }

                            childWidth = std::max(0, parentWidth - horizontalPadding
                                - measuredStartMargin - measuredEndMargin);

                            if (lp->width == 0
                                || lp->width == LayoutParams::MATCH_PARENT)
                            {
                                if (parentWidthMode == FIT)
                                {
                                    childWidthMode = FIT;
                                    lp->hori_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                                }
                                else if (parentWidthMode == EXACTLY)
                                {
                                    childWidthMode = EXACTLY;
                                    lp->hori_couple_handler_type = RestraintLayoutParams::CH_FILL;
                                }
                            }
                            else if (lp->width == LayoutParams::FIT_CONTENT)
                            {
                                childWidthMode = FIT;
                                lp->hori_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                            }
                        }
                        else if (parentWidthMode == UNKNOWN)
                        {
                            childWidth = size;
                            childWidthMode = UNKNOWN;
                            lp->hori_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        }
                    }
                }
            }
        }
        else
        {
            getChildMeasure(
                parentWidth, UNKNOWN,
                horizontalMargins + horizontalPadding,
                lp->width, &childWidth, &childWidthMode);
        }

        *width = childWidth;
        *widthMode = childWidthMode;
    }

    void RestraintLayout::getRestrainedChildHeight(
        View* child, RestraintLayoutParams* lp,
        int parentHeight, int parentHeightMode, int* height, int* heightSpec)
    {
        int verticalPadding = getPaddingTop() + getPaddingBottom();
        int verticalMargins = lp->top_margin + lp->bottom_margin;

        int childHeight = 0;
        int childHeightMode = 0;

        if (lp->hasVerticalCouple())
        {
            int size = std::max(0, parentHeight - verticalPadding - verticalMargins);

            // child 有固定的大小。
            if (lp->height > 0)
            {
                childHeight = lp->height;
                childHeightMode = EXACTLY;
                lp->vert_couple_handler_type = RestraintLayoutParams::CH_FIXED;
            }
            // child 将填充 handler couple 之间的区域。
            else
            {
                // handler couple 绑定于父 View。
                if (lp->top_handled_id == getId() && lp->bottom_handled_id == getId())
                {
                    switch (parentHeightMode)
                    {
                    case FIT:
                    {
                        childHeight = size;
                        childHeightMode = FIT;
                        lp->vert_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                        break;
                    }
                    case EXACTLY:
                    {
                        if (lp->height == 0 || lp->height == LayoutParams::MATCH_PARENT)
                        {
                            childHeight = size;
                            childHeightMode = EXACTLY;
                            lp->vert_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->height == LayoutParams::FIT_CONTENT)
                        {
                            childHeight = size;
                            childHeightMode = FIT;
                            lp->vert_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                        }
                        break;
                    }
                    case UNKNOWN:
                    {
                        // 此处，由于不知道父 View 的大小，无法填充 handler couple 之间的区域。
                        // 让 View 自己决定大小即可。
                        childHeight = size;
                        childHeightMode = UNKNOWN;
                        lp->vert_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        break;
                    }
                    }
                }
                // handler couple 没有绑定于父 View。这意味着它与其他子 View 绑定。
                // 此时必须先测量绑定到的 View，然后才能测量该 View。将使用
                // 递归完成此过程，为此需要额外的变量来记录某一 View 是否已测量。
                else
                {
                    // 上下 Handler 绑定在同一个 View 上。
                    if (lp->top_handled_id == lp->bottom_handled_id)
                    {
                        int measuredTargetHeight = 0;
                        View* target = getChildById(lp->top_handled_id);
                        auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

                        // 测量此 target view 的高度。
                        int targetHeight;
                        int targetHeightMode;
                        if (!targetLp->is_height_measured)
                        {
                            getRestrainedChildHeight(
                                target, targetLp,
                                parentHeight, parentHeightMode,
                                &targetHeight, &targetHeightMode);

                            targetLp->is_height_measured = true;
                            targetLp->spec_height = targetHeight;
                            targetLp->spec_height_mode = targetHeightMode;
                        }
                        else
                        {
                            targetHeight = targetLp->spec_height;
                            targetHeightMode = targetLp->spec_height_mode;
                        }

                        int targetWidth = 0;
                        int targetWidthMode = UNKNOWN;
                        if (targetLp->is_width_measured)
                        {
                            targetWidth = targetLp->spec_width;
                            targetWidthMode = targetLp->spec_width_mode;
                        }

                        // 让 target view 测量自身。
                        // 这将会使 target view 的 onMeasure() 方法多调用一次。
                        target->measure(
                            targetWidth, targetHeight,
                            targetWidthMode, targetHeightMode);
                        measuredTargetHeight = target->getMeasuredHeight();

                        childHeight = std::max(0, measuredTargetHeight - verticalMargins);

                        if (lp->height == 0
                            || lp->height == LayoutParams::MATCH_PARENT)
                        {
                            childHeightMode = EXACTLY;
                            lp->vert_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->height == LayoutParams::FIT_CONTENT)
                        {
                            childHeightMode = FIT;
                            lp->vert_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                        }
                    }
                    else
                    {
                        if (parentHeightMode == EXACTLY || parentHeightMode == FIT)
                        {
                            // 上向遍历。
                            int measuredTopMargin = lp->top_margin;
                            RestraintLayoutParams* childLp = lp;
                            while (childLp->hasTop()
                                && childLp->top_handled_id != this->getId())
                            {
                                View* target = getChildById(childLp->top_handled_id);
                                auto targetLp
                                    = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

                                // 测量此 target view 的高度。
                                int targetHeight;
                                int targetHeightMode;
                                if (!targetLp->is_height_measured)
                                {
                                    getRestrainedChildHeight(
                                        target, targetLp,
                                        parentHeight, parentHeightMode,
                                        &targetHeight, &targetHeightMode);

                                    targetLp->is_height_measured = true;
                                    targetLp->spec_height = childHeight;
                                    targetLp->spec_height_mode = childHeightMode;
                                }
                                else
                                {
                                    targetHeight = targetLp->spec_height;
                                    targetHeightMode = targetLp->spec_height_mode;
                                }

                                int targetWidth = 0;
                                int targetWidthMode = UNKNOWN;
                                if (targetLp->is_width_measured)
                                {
                                    targetWidth = targetLp->spec_width;
                                    targetWidthMode = targetLp->spec_width_mode;
                                }

                                // 让 target view 测量自身。
                                // 这将会使 target view 的 onMeasure() 方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->top_handled_edge == RestraintLayoutParams::BOTTOM) {
                                    measuredTopMargin += target->getMeasuredHeight()
                                        + (targetLp->hasTop() ? targetLp->top_margin : 0);
                                } else if (childLp->top_handled_edge == RestraintLayoutParams::TOP) {
                                    measuredTopMargin +=
                                        (targetLp->hasTop() ? targetLp->top_margin : 0);
                                }

                                childLp = targetLp;
                            }

                            // 下向遍历。
                            int measuredBottomMargin = lp->bottom_margin;
                            childLp = lp;
                            while (childLp->hasBottom() && childLp->bottom_handled_id != getId())
                            {
                                View* target = getChildById(childLp->bottom_handled_id);
                                auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

                                // 测量此 target view 的高度。
                                int targetHeight;
                                int targetHeightMode;
                                if (!targetLp->is_height_measured)
                                {
                                    getRestrainedChildHeight(
                                        target, targetLp,
                                        parentHeight, parentHeightMode,
                                        &targetHeight, &targetHeightMode);

                                    targetLp->is_height_measured = true;
                                    targetLp->spec_height = targetHeight;
                                    targetLp->spec_height_mode = targetHeightMode;
                                }
                                else
                                {
                                    targetHeight = targetLp->spec_height;
                                    targetHeightMode = targetLp->spec_height_mode;
                                }

                                int targetWidth = 0;
                                int targetWidthMode = UNKNOWN;
                                if (targetLp->is_width_measured)
                                {
                                    targetWidth = targetLp->spec_width;
                                    targetWidthMode = targetLp->spec_width_mode;
                                }

                                // 让 target view 测量自身。
                                // 这将会使 target view 的 onMeasure() 方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->bottom_handled_edge == RestraintLayoutParams::TOP) {
                                    measuredBottomMargin += target->getMeasuredHeight()
                                        + (targetLp->hasBottom() ? targetLp->bottom_margin : 0);
                                } else if (childLp->bottom_handled_edge == RestraintLayoutParams::BOTTOM) {
                                    measuredBottomMargin +=
                                        (targetLp->hasBottom() ? targetLp->bottom_margin : 0);
                                }

                                childLp = targetLp;
                            }

                            // TODO: 这里不应该用 parentHeight，而应该使用最后遍历到的 view
                            // 作为边界来计算。此处假定最终绑定到 parent 边界。
                            childHeight = std::max(0, parentHeight - verticalPadding
                                - measuredTopMargin - measuredBottomMargin);

                            if (lp->height == 0 || lp->height == LayoutParams::MATCH_PARENT) {
                                if (parentHeightMode == FIT) {
                                    childHeightMode = FIT;
                                    lp->vert_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                                } else if (parentHeightMode == EXACTLY) {
                                    childHeightMode = EXACTLY;
                                    lp->vert_couple_handler_type = RestraintLayoutParams::CH_FILL;
                                }
                            } else if (lp->height == LayoutParams::FIT_CONTENT) {
                                childHeightMode = FIT;
                                lp->vert_couple_handler_type = RestraintLayoutParams::CH_WRAP;
                            }
                        } else if (parentHeightMode == UNKNOWN) {
                            childHeight = size;
                            childHeightMode = UNKNOWN;
                            lp->vert_couple_handler_type = RestraintLayoutParams::CH_FILL;
                        }
                    }
                }
            }
        } else {
        getChildMeasure(
            parentHeight, UNKNOWN,
            verticalMargins + verticalPadding,
            lp->height, &childHeight, &childHeightMode);
        }

        *height = childHeight;
        *heightSpec = childHeightMode;
    }

    int RestraintLayout::getLeftSpacing(View* child, RestraintLayoutParams* lp) {
        int left_spacing = 0;
        RestraintLayoutParams* childLp = lp;
        while (childLp->hasStart() && childLp->start_handled_id != getId()) {
            View* target = getChildById(lp->start_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (childLp->start_handled_edge == RestraintLayoutParams::END) {
                left_spacing += target->getMeasuredWidth()
                    + (targetLp->hasStart() ? targetLp->left_margin : 0);
            } else if (childLp->start_handled_edge == RestraintLayoutParams::START) {
                left_spacing +=
                    (targetLp->hasStart() ? targetLp->left_margin : 0);
            }

            childLp = targetLp;
        }

        return left_spacing;
    }

    int RestraintLayout::getTopSpacing(View* child, RestraintLayoutParams* lp) {
        int top_spacing = 0;
        RestraintLayoutParams* childLp = lp;
        while (childLp->hasTop() && childLp->top_handled_id != getId()) {
            View* target = getChildById(lp->top_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (childLp->top_handled_edge == RestraintLayoutParams::BOTTOM) {
                top_spacing += target->getMeasuredHeight()
                    + (targetLp->hasTop() ? targetLp->top_margin : 0);
            } else if (childLp->top_handled_edge == RestraintLayoutParams::TOP) {
                top_spacing +=
                    (targetLp->hasTop() ? targetLp->top_margin : 0);
            }

            childLp = targetLp;
        }

        return top_spacing;
    }

    int RestraintLayout::getRightSpacing(View* child, RestraintLayoutParams* lp) {
        int right_spacing = 0;
        RestraintLayoutParams* childLp = lp;
        while (childLp->hasEnd() && childLp->end_handled_id != getId()) {
            View* target = getChildById(childLp->end_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (childLp->end_handled_edge == RestraintLayoutParams::START) {
                right_spacing += target->getMeasuredWidth()
                    + (targetLp->hasEnd() ? targetLp->right_margin : 0);
            } else if (childLp->end_handled_edge == RestraintLayoutParams::END) {
                right_spacing +=
                    (targetLp->hasEnd() ? targetLp->right_margin : 0);
            }

            childLp = targetLp;
        }

        return right_spacing;
    }

    int RestraintLayout::getBottomSpacing(View* child, RestraintLayoutParams* lp) {
        int bottom_spacing = 0;
        RestraintLayoutParams* child_lp = lp;
        while (child_lp->hasBottom() && child_lp->bottom_handled_id != getId()) {
            auto target = getChildById(child_lp->bottom_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (child_lp->bottom_handled_edge == RestraintLayoutParams::TOP) {
                bottom_spacing += target->getMeasuredHeight()
                    + (targetLp->hasBottom() ? targetLp->bottom_margin : 0);
            } else if (child_lp->bottom_handled_edge == RestraintLayoutParams::BOTTOM) {
                bottom_spacing +=
                    (targetLp->hasBottom() ? targetLp->bottom_margin : 0);
            }

            child_lp = targetLp;
        }

        return bottom_spacing;
    }


    int RestraintLayout::measureWrappedWidth() {
        int wrapped_width = 0;

        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());

            int startSpacing = getLeftSpacing(child, lp);
            int endSpacing = getRightSpacing(child, lp);

            int chainWidth =
                startSpacing + endSpacing
                + child->getMeasuredWidth()
                + (lp->hasStart() ? lp->left_margin : 0)
                + (lp->hasEnd() ? lp->right_margin : 0);

            wrapped_width = std::max(wrapped_width, chainWidth);
        }

        return wrapped_width;
    }

    int RestraintLayout::measureWrappedHeight() {
        int wrapped_height = 0;

        for (int i = 0; i < getChildCount(); ++i) {
            auto child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());

            int topSpacing = getTopSpacing(child, lp);
            int bottomSpacing = getBottomSpacing(child, lp);

            int chainHeight =
                topSpacing + bottomSpacing
                + child->getMeasuredHeight()
                + (lp->hasTop() ? lp->top_margin : 0)
                + (lp->hasBottom() ? lp->bottom_margin : 0);
            wrapped_height = std::max(wrapped_height, chainHeight);
        }

        return wrapped_height;
    }


    void RestraintLayout::layoutChild(
        View* child, RestraintLayoutParams* lp,
        int left, int top, int right, int bottom)
    {
        if (!lp->is_vert_layouted) {
            layoutChildVertical(child, lp, top, bottom);
        }
        if (!lp->is_hori_layouted) {
            layoutChildHorizontal(child, lp, left, right);
        }

        child->layout(lp->left, lp->top, lp->right, lp->bottom);
    }


    void RestraintLayout::layoutChildVertical(
        View* child, RestraintLayoutParams* lp, int top, int bottom)
    {
        int childTop = top + getPaddingTop();
        if (lp->hasTop() && lp->top_handled_id != getId()) {
            auto target = getChildById(lp->top_handled_id);
            auto targetLp = reinterpret_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (!targetLp->is_vert_layouted) {
                layoutChildVertical(target, targetLp, top, bottom);
            }

            if (lp->top_handled_edge == RestraintLayoutParams::BOTTOM) {
                childTop = targetLp->bottom;
            } else if (lp->top_handled_edge == RestraintLayoutParams::TOP) {
                childTop = targetLp->top;
            }
        }

        int childBottom = bottom - getPaddingBottom();
        if (lp->hasBottom() && lp->bottom_handled_id != getId()) {
            auto target = getChildById(lp->bottom_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (!targetLp->is_vert_layouted) {
                layoutChildVertical(target, targetLp, top, bottom);
            }

            if (lp->bottom_handled_edge == RestraintLayoutParams::TOP) {
                childBottom = targetLp->top;
            } else if (lp->bottom_handled_edge == RestraintLayoutParams::BOTTOM) {
                childBottom = targetLp->bottom;
            }
        }

        if (lp->hasVerticalCouple()) {
            childTop += lp->top_margin;
            childBottom -= lp->bottom_margin;

            switch (lp->vert_couple_handler_type) {
            case RestraintLayoutParams::CH_FILL:
                break;
            case RestraintLayoutParams::CH_WRAP:
            case RestraintLayoutParams::CH_FIXED:
                childTop += static_cast<int>(
                    (childBottom - childTop - child->getMeasuredHeight())*lp->vertical_percent);
                childBottom = childTop + child->getMeasuredHeight();
                break;
            }
        } else {
            if (lp->hasTop()) {
                childTop += lp->top_margin;
                childBottom = childTop + child->getMeasuredHeight();
            } else if (lp->hasBottom()) {
                childBottom -= lp->bottom_margin;
                childTop = childBottom - child->getMeasuredHeight();
            } else
                childBottom = childTop + child->getMeasuredHeight();
        }

        lp->is_vert_layouted = true;
        lp->top = childTop;
        lp->bottom = childBottom;
    }

    void RestraintLayout::layoutChildHorizontal(
        View* child, RestraintLayoutParams* lp,
        int left, int right)
    {
        int childLeft = left + getPaddingLeft();
        if (lp->hasStart() && lp->start_handled_id != getId()) {
            View* target = getChildById(lp->start_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (!targetLp->is_hori_layouted) {
                layoutChildHorizontal(target, targetLp, left, right);
            }

            if (lp->start_handled_edge == RestraintLayoutParams::END) {
                childLeft = targetLp->right;
            } else if (lp->start_handled_edge == RestraintLayoutParams::START) {
                childLeft = targetLp->left;
            }
        }

        int childRight = right - getPaddingRight();
        if (lp->hasEnd() && lp->end_handled_id != getId()) {
            View* target = getChildById(lp->end_handled_id);
            auto targetLp = static_cast<RestraintLayoutParams*>(target->getLayoutParams());

            if (!targetLp->is_hori_layouted) {
                layoutChildHorizontal(target, targetLp, left, right);
            }

            if (lp->end_handled_edge == RestraintLayoutParams::START) {
                childRight = targetLp->left;
            } else if (lp->end_handled_edge == RestraintLayoutParams::END) {
                childRight = targetLp->right;
            }
        }

        if (lp->hasHorizontalCouple()) {
            childLeft += lp->left_margin;
            childRight -= lp->right_margin;

            switch (lp->hori_couple_handler_type) {
            case RestraintLayoutParams::CH_FILL:
                break;
            case RestraintLayoutParams::CH_WRAP:
            case RestraintLayoutParams::CH_FIXED:
                childLeft += static_cast<int>(
                    (childRight - childLeft - child->getMeasuredWidth())*lp->horizontal_percent);
                childRight = childLeft + child->getMeasuredWidth();
                break;
            }
        } else {
            if (lp->hasStart()) {
                childLeft += lp->left_margin;
                childRight = childLeft + child->getMeasuredWidth();
            } else if (lp->hasEnd()) {
                childRight -= lp->right_margin;
                childLeft = childRight - child->getMeasuredWidth();
            } else {
                childRight = childLeft + child->getMeasuredWidth();
            }
        }

        lp->is_hori_layouted = true;
        lp->left = childLeft;
        lp->right = childRight;
    }


    void RestraintLayout::onMeasure(
        int width, int height,
        int widthMode, int heightMode)
    {
        int final_width = 0;
        int final_height = 0;

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        clearMeasureFlag();
        measureRestrainedChildren(width, height, widthMode, heightMode);

        switch (widthMode) {
        case FIT:
            final_width = measureWrappedWidth() + horizontalPadding;
            final_width = std::min(width, final_width);
            final_width = std::max(getMinimumWidth(), final_width);
            break;

        case EXACTLY:
            final_width = width;
            break;

        case UNKNOWN:
            final_width = measureWrappedWidth() + horizontalPadding;
            final_width = std::max(getMinimumWidth(), final_width);
            break;
        }

        switch (heightMode) {
        case FIT:
            final_height = measureWrappedHeight() + verticalPadding;
            final_height = std::min(height, final_height);
            final_height = std::max(getMinimumHeight(), final_height);
            break;

        case EXACTLY:
            final_height = height;
            break;

        case UNKNOWN:
            final_height = measureWrappedHeight() + verticalPadding;
            final_height = std::max(getMinimumHeight(), final_height);
            break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void RestraintLayout::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        for (int i = 0; i < getChildCount(); ++i) {
            auto child = getChildAt(i);
            auto lp = static_cast<RestraintLayoutParams*>(child->getLayoutParams());

            layoutChild(child, lp, 0, 0, right - left, bottom - top);
        }
    }

}
