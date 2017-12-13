#include "scroll_view.h"

#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/views/layout/layout_params.h"


namespace ukive {

    ScrollView::ScrollView(Window *wnd)
        :ViewGroup(wnd)
    {
    }

    ScrollView::ScrollView(Window *wnd, int id)
        : ViewGroup(wnd, id)
    {
    }

    ScrollView::~ScrollView()
    {
    }


    bool ScrollView::canScroll()
    {
        View *widget = getChildAt(0);
        if (widget && widget->getVisibility() != View::VANISHED)
        {
            if (getMeasuredHeight() < widget->getMeasuredHeight())
                return true;
        }

        return false;
    }

    int ScrollView::computeScrollRange()
    {
        View *widget = getChildAt(0);
        if (widget && widget->getVisibility() != View::VANISHED)
            return widget->getMeasuredHeight();
        return 0;
    }

    int ScrollView::computeScrollExtend()
    {
        View *widget = getChildAt(0);
        if (widget && widget->getVisibility() != View::VANISHED)
            return (widget->getMeasuredHeight() - getMeasuredHeight());
        return 0;
    }

    void ScrollView::processVerticalScroll(int dy)
    {
        int finalDy = 0;
        if (dy > 0)
        {
            int scrollY = getScrollY();
            finalDy = std::max(scrollY - dy, 0) - scrollY;
        }
        else if (dy < 0)
        {
            int scrollY = getScrollY();
            int extend = this->computeScrollExtend();
            if (extend > 0)
                finalDy = std::min(scrollY - dy, extend) - scrollY;
        }

        if (finalDy != 0)
            scrollBy(0, finalDy);
    }


    void ScrollView::onMeasure(
        int width, int height,
        int widthSpec, int heightSpec)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        if (getChildCount() > 1)
            throw std::logic_error("UScrollView-onMeasure(): UScrollView can only have one child.");

        this->measureChildrenWithMargins(width, 0, widthSpec, UNKNOWN);

        switch (widthSpec)
        {
        case FIT:
        {
            View *widget = this->getChildAt(0);
            if (widget && widget->getVisibility() != View::VANISHED)
            {
                int childWidth = widget->getMeasuredWidth();
                finalWidth = std::min(childWidth + horizontalPadding, width);
                finalWidth = std::max(getMinimumWidth(), finalWidth);
            }
            break;
        }

        case UNKNOWN:
        {
            View *widget = this->getChildAt(0);
            if (widget && widget->getVisibility() != View::VANISHED)
            {
                int childWidth = widget->getMeasuredWidth();
                finalWidth = std::max(getMinimumWidth(), childWidth);
            }
            break;
        }

        case EXACTLY:
            finalWidth = width;
            break;
        }

        switch (heightSpec)
        {
        case FIT:
        {
            View *widget = this->getChildAt(0);
            if (widget && widget->getVisibility() != View::VANISHED)
            {
                int childHeight = widget->getMeasuredHeight();
                finalHeight = std::min(childHeight + verticalPadding, height);
                finalHeight = std::max(getMinimumHeight(), finalHeight);
            }
            break;
        }

        case UNKNOWN:
        {
            View *widget = this->getChildAt(0);
            if (widget && widget->getVisibility() != View::VANISHED)
            {
                int childHeight = widget->getMeasuredHeight();
                finalHeight = std::max(getMinimumHeight(), childHeight);
            }
            break;
        }

        case EXACTLY:
            finalHeight = height;
            break;
        }

        this->setMeasuredDimension(finalWidth, finalHeight);
    }

    void ScrollView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        View *widget = this->getChildAt(0);
        if (widget && widget->getVisibility() != View::VANISHED)
        {
            LayoutParams *lp = widget->getLayoutParams();

            int width = widget->getMeasuredWidth();
            int height = widget->getMeasuredHeight();

            int childleft = getPaddingLeft() + lp->leftMargin;
            int childTop = getPaddingTop() + lp->topMargin;

            widget->layout(
                childleft,
                childTop,
                width + childleft,
                height + childTop);
        }
    }

    void ScrollView::onSizeChanged(
        int width, int height, int oldWidth, int oldHeight)
    {
        if (width > 0 && height > 0
            && oldWidth > 0 && oldHeight > 0)
        {
            int changed = 0;

            if (canScroll())
            {
                if (getScrollY() < 0)
                    changed = -getScrollY();

                int extend = this->computeScrollExtend();
                if (getScrollY() > extend)
                    changed = extend - getScrollY();
            }
            else
            {
                if (getScrollY() != 0)
                    changed = -getScrollY();
            }

            if (changed != 0)
                scrollBy(0, changed);
        }
    }

    void ScrollView::onScrollChanged(
        int scrollX, int scrollY, int oldScrollX, int oldScrollY)
    {
        ViewGroup::onScrollChanged(scrollX, scrollY, oldScrollX, oldScrollY);

        InputEvent e;
        e.setEvent(InputEvent::EVM_SCROLL_ENTER);
        e.setMouseX(mMouseXCache + getLeft() - getScrollX() - (oldScrollX - scrollX));
        e.setMouseY(mMouseYCache + getTop() - getScrollY() - (oldScrollY - scrollY));
        this->dispatchInputEvent(&e);
    }


    bool ScrollView::onInputEvent(InputEvent *e)
    {
        ViewGroup::onInputEvent(e);

        switch (e->getEvent())
        {
        case InputEvent::EVM_WHEEL:
            mMouseXCache = e->getMouseX();
            mMouseYCache = e->getMouseY();
            processVerticalScroll(30 * e->getMouseWheel());
            return true;
        }

        return false;
    }

    bool ScrollView::onInterceptMouseEvent(InputEvent *e)
    {
        return false;
    }

}