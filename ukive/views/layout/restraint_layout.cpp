#include "restraint_layout.h"

#include <algorithm>
#include <typeinfo>

#include "ukive/views/layout/restraint_layout_params.h"


namespace ukive {

    RestraintLayout::RestraintLayout(Window *wnd)
        : ViewGroup(wnd)
    {
    }

    RestraintLayout::RestraintLayout(Window *wnd, int id)
        : ViewGroup(wnd, id)
    {
    }

    RestraintLayout::~RestraintLayout()
    {
    }


    LayoutParams *RestraintLayout::generateLayoutParams(const LayoutParams &lp)
    {
        return new RestraintLayoutParams(lp);
    }

    LayoutParams *RestraintLayout::generateDefaultLayoutParams()
    {
        return new RestraintLayoutParams(
            LayoutParams::FIT_CONTENT,
            LayoutParams::FIT_CONTENT);
    }

    bool RestraintLayout::checkLayoutParams(LayoutParams *lp)
    {
        return typeid(*lp) == typeid(RestraintLayoutParams);
    }


    View *RestraintLayout::getChildById(int id)
    {
        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            if (child->getId() == id)
                return child;
        }

        throw std::runtime_error("child view id not existed.");
    }

    void RestraintLayout::clearMeasureFlag()
    {
        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            RestraintLayoutParams *lp = (RestraintLayoutParams*)child->getLayoutParams();
            lp->isWidthMeasured = lp->isHeightMeasured = false;
            lp->isVertLayouted = lp->isHoriLayouted = false;
        }
    }


    void RestraintLayout::measureRestrainedChildren(
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            RestraintLayoutParams *lp = (RestraintLayoutParams*)child->getLayoutParams();
            if (lp->isWidthMeasured)
                continue;

            int childWidth = 0;
            int childWidthMode = 0;

            getRestrainedChildWidth(
                child, lp,
                parentWidth, parentWidthMode,
                &childWidth, &childWidthMode);

            lp->isWidthMeasured = true;
            lp->specWidth = childWidth;
            lp->specWidthMode = childWidthMode;
        }

        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            RestraintLayoutParams *lp = (RestraintLayoutParams*)child->getLayoutParams();
            if (lp->isWidthMeasured && lp->isHeightMeasured)
                continue;

            int childHeight = 0;
            int childHeightMode = 0;

            if (!lp->isHeightMeasured)
            {
                getRestrainedChildHeight(
                    child, lp,
                    parentHeight, parentHeightMode,
                    &childHeight, &childHeightMode);

                lp->isHeightMeasured = true;
                lp->specHeight = childHeight;
                lp->specHeightMode = childHeightMode;
            }
            else
            {
                childHeight = lp->specHeight;
                childHeightMode = lp->specHeightMode;
            }

            child->measure(lp->specWidth, childHeight, lp->specWidthMode, childHeightMode);
        }
    }

    void RestraintLayout::measureRestrainedChild(
        View *child, RestraintLayoutParams *lp,
        int parentWidth, int parentHeight, int parentWidthMode, int parentHeightMode)
    {
        int childWidth = 0;
        int childWidthSpec = 0;

        int childHeight = 0;
        int childHeightSpec = 0;

        if (!lp->isWidthMeasured)
        {
            getRestrainedChildWidth(
                child, lp,
                parentWidth, parentWidthMode,
                &childWidth, &childWidthSpec);

            lp->isWidthMeasured = true;
            lp->specWidth = childWidth;
            lp->specWidthMode = childWidthSpec;
        }
        else
        {
            childWidth = lp->specWidth;
            childWidthSpec = lp->specWidthMode;
        }

        if (!lp->isHeightMeasured)
        {
            getRestrainedChildHeight(
                child, lp,
                parentHeight, parentHeightMode,
                &childHeight, &childHeightSpec);

            lp->isHeightMeasured = true;
            lp->specHeight = childHeight;
            lp->specHeightMode = childHeightSpec;
        }
        else
        {
            childHeight = lp->specHeight;
            childHeightSpec = lp->specHeightMode;
        }

        child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
    }

    void RestraintLayout::getRestrainedChildWidth(
        View *child, RestraintLayoutParams *lp,
        int parentWidth, int parentWidthMode, int *width, int *widthMode)
    {
        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int horizontalMargins = lp->leftMargin + lp->rightMargin;

        int childWidth = 0;
        int childWidthMode = 0;

        if (lp->hasHorizontalCouple())
        {
            int size = std::max(0,
                parentWidth - horizontalPadding - horizontalMargins);

            //child有固定的大小。
            if (lp->width > 0)
            {
                childWidth = lp->width;
                childWidthMode = EXACTLY;
                lp->horiCoupleHandlerType = RestraintLayoutParams::CH_FIXED;
            }
            //child将填充handler couple之间的区域。
            else
            {
                //handler couple绑定于父View。
                if (lp->startHandledId == this->getId()
                    && lp->endHandledId == this->getId())
                {
                    switch (parentWidthMode)
                    {
                    case FIT:
                    {
                        childWidth = size;
                        childWidthMode = FIT;
                        lp->horiCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                        break;
                    }
                    case EXACTLY:
                    {
                        if (lp->width == 0
                            || lp->width == LayoutParams::MATCH_PARENT)
                        {
                            childWidth = size;
                            childWidthMode = EXACTLY;
                            lp->horiCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->width == LayoutParams::FIT_CONTENT)
                        {
                            childWidth = size;
                            childWidthMode = FIT;
                            lp->horiCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                        }
                        break;
                    }
                    case UNKNOWN:
                    {
                        //此处，由于不知道父View的大小，无法填充handler couple之间的区域。
                        //让View自己决定大小即可。
                        childWidth = size;
                        childWidthMode = UNKNOWN;
                        lp->horiCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        break;
                    }
                    }
                }
                //handler对没有绑定于父View。这意味着它与其他子View绑定。
                //此时必须先测量绑定到的View，然后才能测量该View。将使用
                //递归完成此过程，为此需要额外的变量来记录某一View是否已测量。
                else
                {
                    //左右Handler绑定在同一个View上。
                    if (lp->startHandledId == lp->endHandledId)
                    {
                        int measuredTargetWidth = 0;
                        View *target = getChildById(lp->startHandledId);
                        RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

                        //测量此target view的宽度。
                        int targetWidth;
                        int targetWidthMode;
                        if (!targetLp->isWidthMeasured)
                        {
                            getRestrainedChildWidth(
                                target, targetLp,
                                parentWidth, parentWidthMode,
                                &targetWidth, &targetWidthMode);

                            targetLp->isWidthMeasured = true;
                            targetLp->specWidth = targetWidth;
                            targetLp->specWidthMode = targetWidthMode;
                        }
                        else
                        {
                            targetWidth = targetLp->specWidth;
                            targetWidthMode = targetLp->specWidthMode;
                        }

                        int targetHeight = 0;
                        int targetHeightMode = UNKNOWN;
                        if (targetLp->isHeightMeasured)
                        {
                            targetHeight = targetLp->specHeight;
                            targetHeightMode = targetLp->specHeightMode;
                        }

                        //让target view测量自身。
                        //这将会使target view的onMeasure()方法多调用一次。
                        target->measure(
                            targetWidth, targetHeight,
                            targetWidthMode, targetHeightMode);
                        measuredTargetWidth = target->getMeasuredWidth();

                        childWidth = std::max(0, measuredTargetWidth - horizontalMargins);

                        if (lp->width == 0
                            || lp->width == LayoutParams::MATCH_PARENT)
                        {
                            childWidthMode = EXACTLY;
                            lp->horiCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->width == LayoutParams::FIT_CONTENT)
                        {
                            childWidthMode = FIT;
                            lp->horiCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                        }
                    }
                    else
                    {
                        if (parentWidthMode == EXACTLY
                            || parentWidthMode == FIT)
                        {
                            //前向遍历。
                            int measuredStartMargin = lp->leftMargin;
                            RestraintLayoutParams *childLp = lp;
                            while (childLp->hasStart()
                                && childLp->startHandledId != this->getId())
                            {
                                View *target = getChildById(childLp->startHandledId);
                                RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

                                //测量此target view的宽度。
                                int targetWidth;
                                int targetWidthMode;
                                if (!targetLp->isWidthMeasured)
                                {
                                    getRestrainedChildWidth(
                                        target, targetLp,
                                        parentWidth, parentWidthMode,
                                        &targetWidth, &targetWidthMode);

                                    targetLp->isWidthMeasured = true;
                                    targetLp->specWidth = targetWidth;
                                    targetLp->specWidthMode = targetWidthMode;
                                }
                                else
                                {
                                    targetWidth = targetLp->specWidth;
                                    targetWidthMode = targetLp->specWidthMode;
                                }

                                int targetHeight = 0;
                                int targetHeightMode = UNKNOWN;
                                if (targetLp->isHeightMeasured)
                                {
                                    targetHeight = targetLp->specHeight;
                                    targetHeightMode = targetLp->specHeightMode;
                                }

                                //让target view测量自身。
                                //这将会使target view的onMeasure()方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->startHandledEdge
                                    == RestraintLayoutParams::END)
                                {
                                    measuredStartMargin += target->getMeasuredWidth()
                                        + (targetLp->hasStart() ? targetLp->leftMargin : 0);
                                }
                                else if (childLp->startHandledEdge
                                    == RestraintLayoutParams::START)
                                {
                                    measuredStartMargin +=
                                        (targetLp->hasStart() ? targetLp->leftMargin : 0);
                                }

                                childLp = targetLp;
                            }

                            //后向遍历。
                            int measuredEndMargin = lp->rightMargin;
                            childLp = lp;
                            while (childLp->hasEnd()
                                && childLp->endHandledId != this->getId())
                            {
                                View *target = getChildById(childLp->endHandledId);
                                RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

                                //测量此target view的宽度。
                                int targetWidth;
                                int targetWidthMode;
                                if (!targetLp->isWidthMeasured)
                                {
                                    getRestrainedChildWidth(
                                        target, targetLp,
                                        parentWidth, parentWidthMode,
                                        &targetWidth, &targetWidthMode);

                                    targetLp->isWidthMeasured = true;
                                    targetLp->specWidth = targetWidth;
                                    targetLp->specWidthMode = targetWidthMode;
                                }
                                else
                                {
                                    targetWidth = targetLp->specWidth;
                                    targetWidthMode = targetLp->specWidthMode;
                                }

                                int targetHeight = 0;
                                int targetHeightMode = UNKNOWN;
                                if (targetLp->isHeightMeasured)
                                {
                                    targetHeight = targetLp->specHeight;
                                    targetHeightMode = targetLp->specHeightMode;
                                }

                                //让target view测量自身。
                                //这将会使target view的onMeasure()方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->endHandledEdge
                                    == RestraintLayoutParams::START)
                                {
                                    measuredEndMargin += target->getMeasuredWidth()
                                        + (targetLp->hasEnd() ? targetLp->rightMargin : 0);
                                }
                                else if (childLp->endHandledEdge
                                    == RestraintLayoutParams::END)
                                {
                                    measuredEndMargin +=
                                        (targetLp->hasEnd() ? targetLp->rightMargin : 0);
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
                                    lp->horiCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                                }
                                else if (parentWidthMode == EXACTLY)
                                {
                                    childWidthMode = EXACTLY;
                                    lp->horiCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                                }
                            }
                            else if (lp->width == LayoutParams::FIT_CONTENT)
                            {
                                childWidthMode = FIT;
                                lp->horiCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                            }
                        }
                        else if (parentWidthMode == UNKNOWN)
                        {
                            childWidth = size;
                            childWidthMode = UNKNOWN;
                            lp->horiCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        }
                    }
                }
            }
        }
        else
        {
            this->getChildMeasure(
                parentWidth, parentWidthMode,
                horizontalMargins + horizontalPadding,
                lp->width, &childWidth, &childWidthMode);
        }

        *width = childWidth;
        *widthMode = childWidthMode;
    }

    void RestraintLayout::getRestrainedChildHeight(
        View *child, RestraintLayoutParams *lp,
        int parentHeight, int parentHeightMode, int *height, int *heightSpec)
    {
        int verticalPadding = getPaddingTop() + getPaddingBottom();
        int verticalMargins = lp->topMargin + lp->bottomMargin;

        int childHeight = 0;
        int childHeightMode = 0;

        if (lp->hasVerticalCouple())
        {
            int size = std::max(0,
                parentHeight - verticalPadding - verticalMargins);

            //child有固定的大小。
            if (lp->height > 0)
            {
                childHeight = lp->height;
                childHeightMode = EXACTLY;
                lp->vertCoupleHandlerType = RestraintLayoutParams::CH_FIXED;
            }
            //child将填充handler couple之间的区域。
            else
            {
                //handler couple绑定于父View。
                if (lp->topHandledId == this->getId()
                    && lp->bottomHandledId == this->getId())
                {
                    switch (parentHeightMode)
                    {
                    case FIT:
                    {
                        childHeight = size;
                        childHeightMode = FIT;
                        lp->vertCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                        break;
                    }
                    case EXACTLY:
                    {
                        if (lp->height == 0
                            || lp->height == LayoutParams::MATCH_PARENT)
                        {
                            childHeight = size;
                            childHeightMode = EXACTLY;
                            lp->vertCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->height == LayoutParams::FIT_CONTENT)
                        {
                            childHeight = size;
                            childHeightMode = FIT;
                            lp->vertCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                        }
                        break;
                    }
                    case UNKNOWN:
                    {
                        //此处，由于不知道父View的大小，无法填充handler对之间的区域。
                        //让View自己决定大小即可。
                        childHeight = size;
                        childHeightMode = UNKNOWN;
                        lp->vertCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        break;
                    }
                    }
                }
                //handler对没有绑定于父View。这意味着它与其他子View绑定。
                //此时必须先测量绑定到的View，然后才能测量该View。将使用
                //递归完成此过程，为此需要额外的变量来记录某一View是否已测量。
                else
                {
                    //上下Handler绑定在同一个View上。
                    if (lp->topHandledId == lp->bottomHandledId)
                    {
                        int measuredTargetHeight = 0;
                        View *target = getChildById(lp->topHandledId);
                        RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

                        //测量此target view的高度。
                        int targetHeight;
                        int targetHeightMode;
                        if (!targetLp->isHeightMeasured)
                        {
                            getRestrainedChildHeight(
                                target, targetLp,
                                parentHeight, parentHeightMode,
                                &targetHeight, &targetHeightMode);

                            targetLp->isHeightMeasured = true;
                            targetLp->specHeight = targetHeight;
                            targetLp->specHeightMode = targetHeightMode;
                        }
                        else
                        {
                            targetHeight = targetLp->specHeight;
                            targetHeightMode = targetLp->specHeightMode;
                        }

                        int targetWidth = 0;
                        int targetWidthMode = UNKNOWN;
                        if (targetLp->isWidthMeasured)
                        {
                            targetWidth = targetLp->specWidth;
                            targetWidthMode = targetLp->specWidthMode;
                        }

                        //让target view测量自身。
                        //这将会使target view的onMeasure()方法多调用一次。
                        target->measure(
                            targetWidth, targetHeight,
                            targetWidthMode, targetHeightMode);
                        measuredTargetHeight = target->getMeasuredHeight();

                        childHeight = std::max(0, measuredTargetHeight - verticalMargins);

                        if (lp->height == 0
                            || lp->height == LayoutParams::MATCH_PARENT)
                        {
                            childHeightMode = EXACTLY;
                            lp->vertCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        }
                        else if (lp->height == LayoutParams::FIT_CONTENT)
                        {
                            childHeightMode = FIT;
                            lp->vertCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                        }
                    }
                    else
                    {
                        if (parentHeightMode == EXACTLY
                            || parentHeightMode == FIT)
                        {
                            //上向遍历。
                            int measuredTopMargin = lp->topMargin;
                            RestraintLayoutParams *childLp = lp;
                            while (childLp->hasTop()
                                && childLp->topHandledId != this->getId())
                            {
                                View *target = getChildById(childLp->topHandledId);
                                RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

                                //测量此target view的高度。
                                int targetHeight;
                                int targetHeightMode;
                                if (!targetLp->isHeightMeasured)
                                {
                                    getRestrainedChildHeight(
                                        target, targetLp,
                                        parentHeight, parentHeightMode,
                                        &targetHeight, &targetHeightMode);

                                    targetLp->isHeightMeasured = true;
                                    targetLp->specHeight = childHeight;
                                    targetLp->specHeightMode = childHeightMode;
                                }
                                else
                                {
                                    targetHeight = targetLp->specHeight;
                                    targetHeightMode = targetLp->specHeightMode;
                                }

                                int targetWidth = 0;
                                int targetWidthMode = UNKNOWN;
                                if (targetLp->isWidthMeasured)
                                {
                                    targetWidth = targetLp->specWidth;
                                    targetWidthMode = targetLp->specWidthMode;
                                }

                                //让target view测量自身。
                                //这将会使target view的onMeasure()方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->topHandledEdge
                                    == RestraintLayoutParams::BOTTOM)
                                {
                                    measuredTopMargin += target->getMeasuredHeight()
                                        + (targetLp->hasTop() ? targetLp->topMargin : 0);
                                }
                                else if (childLp->topHandledEdge
                                    == RestraintLayoutParams::TOP)
                                {
                                    measuredTopMargin +=
                                        (targetLp->hasTop() ? targetLp->topMargin : 0);
                                }

                                childLp = targetLp;
                            }

                            //下向遍历。
                            int measuredBottomMargin = lp->bottomMargin;
                            childLp = lp;
                            while (childLp->hasBottom()
                                && childLp->bottomHandledId != this->getId())
                            {
                                View *target = getChildById(childLp->bottomHandledId);
                                RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

                                //测量此target view的高度。
                                int targetHeight;
                                int targetHeightMode;
                                if (!targetLp->isHeightMeasured)
                                {
                                    getRestrainedChildHeight(
                                        target, targetLp,
                                        parentHeight, parentHeightMode,
                                        &targetHeight, &targetHeightMode);

                                    targetLp->isHeightMeasured = true;
                                    targetLp->specHeight = targetHeight;
                                    targetLp->specHeightMode = targetHeightMode;
                                }
                                else
                                {
                                    targetHeight = targetLp->specHeight;
                                    targetHeightMode = targetLp->specHeightMode;
                                }

                                int targetWidth = 0;
                                int targetWidthMode = UNKNOWN;
                                if (targetLp->isWidthMeasured)
                                {
                                    targetWidth = targetLp->specWidth;
                                    targetWidthMode = targetLp->specWidthMode;
                                }

                                //让target view测量自身。
                                //这将会使target view的onMeasure()方法多调用一次。
                                target->measure(
                                    targetWidth, targetHeight,
                                    targetWidthMode, targetHeightMode);

                                if (childLp->bottomHandledEdge == RestraintLayoutParams::TOP)
                                {
                                    measuredBottomMargin += target->getMeasuredHeight()
                                        + (targetLp->hasBottom() ? targetLp->bottomMargin : 0);
                                }
                                else if (childLp->bottomHandledEdge == RestraintLayoutParams::BOTTOM)
                                {
                                    measuredBottomMargin +=
                                        (targetLp->hasBottom() ? targetLp->bottomMargin : 0);
                                }

                                childLp = targetLp;
                            }

                            //TODO:这里不应该用parentHeight，而应该使用最后遍历到的view
                            //作为边界来计算。此处假定最终绑定到parent边界。
                            childHeight = std::max(0, parentHeight - verticalPadding
                                - measuredTopMargin - measuredBottomMargin);

                            if (lp->height == 0
                                || lp->height == LayoutParams::MATCH_PARENT)
                            {
                                if (parentHeightMode == FIT)
                                {
                                    childHeightMode = FIT;
                                    lp->vertCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                                }
                                else if (parentHeightMode == EXACTLY)
                                {
                                    childHeightMode = EXACTLY;
                                    lp->vertCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                                }
                            }
                            else if (lp->height == LayoutParams::FIT_CONTENT)
                            {
                                childHeightMode = FIT;
                                lp->vertCoupleHandlerType = RestraintLayoutParams::CH_WRAP;
                            }
                        }
                        else if (parentHeightMode == UNKNOWN)
                        {
                            childHeight = size;
                            childHeightMode = UNKNOWN;
                            lp->vertCoupleHandlerType = RestraintLayoutParams::CH_FILL;
                        }
                    }
                }
            }
        }
        else
        {
            this->getChildMeasure(
                parentHeight, parentHeightMode,
                verticalMargins + verticalPadding,
                lp->height, &childHeight, &childHeightMode);
        }

        *height = childHeight;
        *heightSpec = childHeightMode;
    }

    int RestraintLayout::getLeftSpacing(View *child, RestraintLayoutParams *lp)
    {
        int leftSpacing = 0;
        RestraintLayoutParams *childLp = lp;
        while (childLp->hasStart()
            && childLp->startHandledId != this->getId())
        {
            View *target = getChildById(lp->startHandledId);
            RestraintLayoutParams *targetLp
                = (RestraintLayoutParams*)target->getLayoutParams();

            if (childLp->startHandledEdge
                == RestraintLayoutParams::END)
            {
                leftSpacing += target->getMeasuredWidth()
                    + (targetLp->hasStart() ? targetLp->leftMargin : 0);
            }
            else if (childLp->startHandledEdge
                == RestraintLayoutParams::START)
            {
                leftSpacing +=
                    (targetLp->hasStart() ? targetLp->leftMargin : 0);
            }

            childLp = targetLp;
        }

        return leftSpacing;
    }

    int RestraintLayout::getTopSpacing(View *child, RestraintLayoutParams *lp)
    {
        int topSpacing = 0;
        RestraintLayoutParams *childLp = lp;
        while (childLp->hasTop()
            && childLp->topHandledId != this->getId())
        {
            View *target = getChildById(lp->topHandledId);
            RestraintLayoutParams *targetLp
                = (RestraintLayoutParams*)target->getLayoutParams();

            if (childLp->topHandledEdge
                == RestraintLayoutParams::BOTTOM)
            {
                topSpacing += target->getMeasuredHeight()
                    + (targetLp->hasTop() ? targetLp->topMargin : 0);
            }
            else if (childLp->topHandledEdge
                == RestraintLayoutParams::TOP)
            {
                topSpacing +=
                    (targetLp->hasTop() ? targetLp->topMargin : 0);
            }

            childLp = targetLp;
        }

        return topSpacing;
    }

    int RestraintLayout::getRightSpacing(View *child, RestraintLayoutParams *lp)
    {
        int rightSpacing = 0;
        RestraintLayoutParams *childLp = lp;
        while (childLp->hasEnd()
            && childLp->endHandledId != this->getId())
        {
            View *target = getChildById(childLp->endHandledId);
            RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

            if (childLp->endHandledEdge
                == RestraintLayoutParams::START)
            {
                rightSpacing += target->getMeasuredWidth()
                    + (targetLp->hasEnd() ? targetLp->rightMargin : 0);
            }
            else if (childLp->endHandledEdge
                == RestraintLayoutParams::END)
            {
                rightSpacing +=
                    (targetLp->hasEnd() ? targetLp->rightMargin : 0);
            }

            childLp = targetLp;
        }

        return rightSpacing;
    }

    int RestraintLayout::getBottomSpacing(View *child, RestraintLayoutParams *lp)
    {
        int bottomSpacing = 0;
        RestraintLayoutParams *childLp = lp;
        while (childLp->hasBottom()
            && childLp->bottomHandledId != this->getId())
        {
            View *target = getChildById(childLp->bottomHandledId);
            RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

            if (childLp->bottomHandledEdge
                == RestraintLayoutParams::TOP)
            {
                bottomSpacing += target->getMeasuredHeight()
                    + (targetLp->hasBottom() ? targetLp->bottomMargin : 0);
            }
            else if (childLp->bottomHandledEdge
                == RestraintLayoutParams::BOTTOM)
            {
                bottomSpacing +=
                    (targetLp->hasBottom() ? targetLp->bottomMargin : 0);
            }

            childLp = targetLp;
        }

        return bottomSpacing;
    }


    int RestraintLayout::measureWrappedWidth()
    {
        int wrappedWidth = 0;

        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            RestraintLayoutParams *lp = (RestraintLayoutParams*)child->getLayoutParams();

            int startSpacing = getLeftSpacing(child, lp);
            int endSpacing = getRightSpacing(child, lp);

            int chainWidth =
                startSpacing + endSpacing
                + child->getMeasuredWidth()
                + (lp->hasStart() ? lp->leftMargin : 0)
                + (lp->hasEnd() ? lp->rightMargin : 0);

            wrappedWidth = std::max(wrappedWidth, chainWidth);
        }

        return wrappedWidth;
    }

    int RestraintLayout::measureWrappedHeight()
    {
        int wrappedHeight = 0;

        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            RestraintLayoutParams *lp = (RestraintLayoutParams*)child->getLayoutParams();

            int topSpacing = getTopSpacing(child, lp);
            int bottomSpacing = getBottomSpacing(child, lp);

            int chainHeight =
                topSpacing + bottomSpacing
                + child->getMeasuredHeight()
                + (lp->hasTop() ? lp->topMargin : 0)
                + (lp->hasBottom() ? lp->bottomMargin : 0);
            wrappedHeight = std::max(wrappedHeight, chainHeight);
        }

        return wrappedHeight;
    }


    void RestraintLayout::layoutChild(
        View *child, RestraintLayoutParams *lp,
        int left, int top, int right, int bottom)
    {
        if (!lp->isVertLayouted)
            layoutChildVertical(child, lp, top, bottom);
        if (!lp->isHoriLayouted)
            layoutChildHorizontal(child, lp, left, right);

        child->layout(lp->left, lp->top, lp->right, lp->bottom);
    }


    void RestraintLayout::layoutChildVertical(
        View *child, RestraintLayoutParams *lp,
        int top, int bottom)
    {
        int childTop = top + getPaddingTop();
        if (lp->hasTop()
            && lp->topHandledId != this->getId())
        {
            View *target = getChildById(lp->topHandledId);
            RestraintLayoutParams *targetLp
                = (RestraintLayoutParams*)target->getLayoutParams();

            if (!targetLp->isVertLayouted)
                layoutChildVertical(target, targetLp, top, bottom);

            if (lp->topHandledEdge
                == RestraintLayoutParams::BOTTOM)
            {
                childTop = targetLp->bottom;
            }
            else if (lp->topHandledEdge
                == RestraintLayoutParams::TOP)
            {
                childTop = targetLp->top;
            }
        }

        int childBottom = bottom - getPaddingBottom();
        if (lp->hasBottom()
            && lp->bottomHandledId != this->getId())
        {
            View *target = getChildById(lp->bottomHandledId);
            RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

            if (!targetLp->isVertLayouted)
                layoutChildVertical(target, targetLp, top, bottom);

            if (lp->bottomHandledEdge
                == RestraintLayoutParams::TOP)
            {
                childBottom = targetLp->top;
            }
            else if (lp->bottomHandledEdge
                == RestraintLayoutParams::BOTTOM)
            {
                childBottom = targetLp->bottom;
            }
        }

        if (lp->hasVerticalCouple())
        {
            childTop += lp->topMargin;
            childBottom -= lp->bottomMargin;

            switch (lp->vertCoupleHandlerType)
            {
            case RestraintLayoutParams::CH_FILL:
                break;
            case RestraintLayoutParams::CH_WRAP:
            case RestraintLayoutParams::CH_FIXED:
                childTop += static_cast<int>(
                    (childBottom - childTop - child->getMeasuredHeight())*lp->verticalPercent);
                childBottom = childTop + child->getMeasuredHeight();
                break;
            }
        }
        else
        {
            if (lp->hasTop())
            {
                childTop += lp->topMargin;
                childBottom = childTop + child->getMeasuredHeight();
            }
            else if (lp->hasBottom())
            {
                childBottom -= lp->bottomMargin;
                childTop = childBottom - child->getMeasuredHeight();
            }
            else
                childBottom = childTop + child->getMeasuredHeight();
        }

        lp->isVertLayouted = true;
        lp->top = childTop;
        lp->bottom = childBottom;
    }

    void RestraintLayout::layoutChildHorizontal(
        View *child, RestraintLayoutParams *lp,
        int left, int right)
    {
        int childLeft = left + getPaddingLeft();
        if (lp->hasStart()
            && lp->startHandledId != this->getId())
        {
            View *target = getChildById(lp->startHandledId);
            RestraintLayoutParams *targetLp
                = (RestraintLayoutParams*)target->getLayoutParams();

            if (!targetLp->isHoriLayouted)
                layoutChildHorizontal(target, targetLp, left, right);

            if (lp->startHandledEdge
                == RestraintLayoutParams::END)
            {
                childLeft = targetLp->right;
            }
            else if (lp->startHandledEdge
                == RestraintLayoutParams::START)
            {
                childLeft = targetLp->left;
            }
        }

        int childRight = right - getPaddingRight();
        if (lp->hasEnd()
            && lp->endHandledId != this->getId())
        {
            View *target = getChildById(lp->endHandledId);
            RestraintLayoutParams *targetLp = (RestraintLayoutParams*)target->getLayoutParams();

            if (!targetLp->isHoriLayouted)
                layoutChildHorizontal(target, targetLp, left, right);

            if (lp->endHandledEdge
                == RestraintLayoutParams::START)
            {
                childRight = targetLp->left;
            }
            else if (lp->endHandledEdge
                == RestraintLayoutParams::END)
            {
                childRight = targetLp->right;
            }
        }

        if (lp->hasHorizontalCouple())
        {
            childLeft += lp->leftMargin;
            childRight -= lp->rightMargin;

            switch (lp->horiCoupleHandlerType)
            {
            case RestraintLayoutParams::CH_FILL:
                break;
            case RestraintLayoutParams::CH_WRAP:
            case RestraintLayoutParams::CH_FIXED:
                childLeft += static_cast<int>(
                    (childRight - childLeft - child->getMeasuredWidth())*lp->horizontalPercent);
                childRight = childLeft + child->getMeasuredWidth();
                break;
            }
        }
        else
        {
            if (lp->hasStart())
            {
                childLeft += lp->leftMargin;
                childRight = childLeft + child->getMeasuredWidth();
            }
            else if (lp->hasEnd())
            {
                childRight -= lp->rightMargin;
                childLeft = childRight - child->getMeasuredWidth();
            }
            else
                childRight = childLeft + child->getMeasuredWidth();
        }

        lp->isHoriLayouted = true;
        lp->left = childLeft;
        lp->right = childRight;
    }


    void RestraintLayout::onMeasure(
        int width, int height,
        int widthMode, int heightMode)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        clearMeasureFlag();
        this->measureRestrainedChildren(width, height, widthMode, heightMode);

        switch (widthMode)
        {
        case FIT:
            finalWidth = measureWrappedWidth() + horizontalPadding;
            finalWidth = std::min(width, finalWidth);
            finalWidth = std::max(getMinimumWidth(), finalWidth);
            break;

        case EXACTLY:
            finalWidth = width;
            break;

        case UNKNOWN:
            finalWidth = measureWrappedWidth() + horizontalPadding;
            finalWidth = std::max(getMinimumWidth(), finalWidth);
            break;
        }

        switch (heightMode)
        {
        case FIT:
            finalHeight = measureWrappedHeight() + verticalPadding;
            finalHeight = std::min(height, finalHeight);
            finalHeight = std::max(getMinimumHeight(), finalHeight);
            break;

        case EXACTLY:
            finalHeight = height;
            break;

        case UNKNOWN:
            finalHeight = measureWrappedHeight() + verticalPadding;
            finalHeight = std::max(getMinimumHeight(), finalHeight);
            break;
        }

        this->setMeasuredDimension(finalWidth, finalHeight);
    }

    void RestraintLayout::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            RestraintLayoutParams *lp = (RestraintLayoutParams*)child->getLayoutParams();

            layoutChild(child, lp, 0, 0,
                right - left, bottom - top);
        }
    }

}