#include "scroll_view.h"

#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/views/layout/layout_params.h"


namespace ukive {

    ScrollView::ScrollView(Window* wnd)
        :ViewGroup(wnd) {
    }


    bool ScrollView::canScroll() {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != View::VANISHED) {
            if (getMeasuredHeight() < child->getMeasuredHeight()) {
                return true;
            }
        }

        return false;
    }

    int ScrollView::computeScrollRange() {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != View::VANISHED) {
            return child->getMeasuredHeight();
        }
        return 0;
    }

    int ScrollView::computeScrollExtend() {
        View* child = getChildAt(0);
        if (child && child->getVisibility() != View::VANISHED) {
            return (child->getMeasuredHeight() - getMeasuredHeight());
        }
        return 0;
    }

    void ScrollView::processVerticalScroll(int dy) {
        int final_dy = 0;
        if (dy > 0) {
            int scroll_y = getScrollY();
            final_dy = std::max(scroll_y - dy, 0) - scroll_y;
        } else if (dy < 0) {
            int scroll_y = getScrollY();
            int extend = computeScrollExtend();
            if (extend > 0) {
                final_dy = std::min(scroll_y - dy, extend) - scroll_y;
            }
        }

        if (final_dy != 0) {
            scrollBy(0, final_dy);
        }
    }


    void ScrollView::onMeasure(
        int width, int height, int widthSpec, int heightSpec) {

        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        if (getChildCount() > 1) {
            throw std::logic_error("UScrollView-onMeasure(): UScrollView can only have one child.");
        }

        measureChildrenWithMargins(width, 0, widthSpec, UNKNOWN);

        switch (widthSpec) {
        case FIT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int childWidth = child->getMeasuredWidth();
                final_width = std::min(childWidth + hori_padding, width);
                final_width = std::max(getMinimumWidth(), final_width);
            }
            break;
        }

        case UNKNOWN: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int childWidth = child->getMeasuredWidth();
                final_width = std::max(getMinimumWidth(), childWidth);
            }
            break;
        }

        case EXACTLY:
            final_width = width;
            break;
        }

        switch (heightSpec) {
        case FIT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int childHeight = child->getMeasuredHeight();
                final_height = std::min(childHeight + vert_padding, height);
                final_height = std::max(getMinimumHeight(), final_height);
            }
            break;
        }

        case UNKNOWN: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int childHeight = child->getMeasuredHeight();
                final_height = std::max(getMinimumHeight(), childHeight);
            }
            break;
        }

        case EXACTLY:
            final_height = height;
            break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void ScrollView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom) {

        auto child = getChildAt(0);
        if (child && child->getVisibility() != View::VANISHED) {
            auto lp = child->getLayoutParams();

            int width = child->getMeasuredWidth();
            int height = child->getMeasuredHeight();

            int childleft = getPaddingLeft() + lp->leftMargin;
            int childTop = getPaddingTop() + lp->topMargin;

            child->layout(
                childleft, childTop,
                width + childleft, height + childTop);
        }
    }

    void ScrollView::onSizeChanged(
        int width, int height, int oldWidth, int oldHeight) {

        if (width > 0 && height > 0
            && oldWidth > 0 && oldHeight > 0) {
            int changed = 0;

            if (canScroll()) {
                if (getScrollY() < 0) {
                    changed = -getScrollY();
                }

                int extend = computeScrollExtend();
                if (getScrollY() > extend) {
                    changed = extend - getScrollY();
                }
            } else {
                if (getScrollY() != 0) {
                    changed = -getScrollY();
                }
            }

            if (changed != 0) {
                scrollBy(0, changed);
            }
        }
    }

    void ScrollView::onScrollChanged(
        int scrollX, int scrollY, int oldScrollX, int oldScrollY) {
        ViewGroup::onScrollChanged(scrollX, scrollY, oldScrollX, oldScrollY);

        InputEvent e;
        e.setEvent(InputEvent::EVM_SCROLL_ENTER);
        e.setMouseX(mMouseXCache + getLeft() - getScrollX() - (oldScrollX - scrollX));
        e.setMouseY(mMouseYCache + getTop() - getScrollY() - (oldScrollY - scrollY));
        dispatchInputEvent(&e);
    }


    bool ScrollView::onInputEvent(InputEvent* e) {
        bool consumed = ViewGroup::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
            mMouseXCache = e->getMouseX();
            mMouseYCache = e->getMouseY();
            processVerticalScroll(30 * e->getMouseWheel());
            consumed |= true;
        }

        return consumed;
    }

    bool ScrollView::onInterceptMouseEvent(InputEvent* e) {
        return false;
    }

}