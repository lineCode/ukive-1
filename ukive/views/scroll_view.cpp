#include "scroll_view.h"

#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/log.h"
#include "ukive/views/layout/layout_params.h"


namespace ukive {

    ScrollView::ScrollView(Window* w)
        : ScrollView(w, {}) {}

    ScrollView::ScrollView(Window* w, AttrsRef attrs)
        : ViewGroup(w, attrs),
          mouse_x_cache_(0),
          mouse_y_cache_(0),
          saved_pointer_type_(InputEvent::PT_NONE),
          scroller_(nullptr) {}

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


    void ScrollView::onMeasure(int width, int height, int width_mode, int height_mode) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        DCHECK(getChildCount() <= 1) << "ScrollView can only have one child.";

        measureChildrenWithMargins(width, 0, width_mode, UNKNOWN);

        switch (width_mode) {
        case FIT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int child_width = child->getMeasuredWidth();
                final_width = std::min(child_width + hori_padding, width);
                final_width = std::max(getMinimumWidth(), final_width);
            }
            break;
        }

        case UNKNOWN: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int child_width = child->getMeasuredWidth();
                final_width = std::max(getMinimumWidth(), child_width);
            }
            break;
        }

        case EXACTLY:
        default:
            final_width = width;
            break;
        }

        switch (height_mode) {
        case FIT: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int child_height = child->getMeasuredHeight();
                final_height = std::min(child_height + vert_padding, height);
                final_height = std::max(getMinimumHeight(), final_height);
            }
            break;
        }

        case UNKNOWN: {
            View* child = getChildAt(0);
            if (child && child->getVisibility() != View::VANISHED) {
                int child_height = child->getMeasuredHeight();
                final_height = std::max(getMinimumHeight(), child_height);
            }
            break;
        }

        case EXACTLY:
        default:
            final_height = height;
            break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void ScrollView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        auto child = getChildAt(0);
        if (child && child->getVisibility() != View::VANISHED) {
            auto lp = child->getLayoutParams();

            int width = child->getMeasuredWidth();
            int height = child->getMeasuredHeight();

            int child_left = getPaddingLeft() + lp->left_margin;
            int child_top = getPaddingTop() + lp->top_margin;

            child->layout(
                child_left, child_top,
                width + child_left, height + child_top);
        }
    }

    void ScrollView::onSizeChanged(
        int width, int height, int old_w, int old_h)
    {
        if (width > 0 && height > 0 &&
            old_w > 0 && old_h > 0)
        {
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
        int scrollX, int scrollY, int oldScrollX, int oldScrollY)
    {
        ViewGroup::onScrollChanged(scrollX, scrollY, oldScrollX, oldScrollY);

        InputEvent e;
        e.setEvent(InputEvent::EVM_SCROLL_ENTER);
        e.setPointerType(saved_pointer_type_);
        e.setX(mouse_x_cache_ + getLeft() - getScrollX() - (oldScrollX - scrollX));
        e.setY(mouse_y_cache_ + getTop() - getScrollY() - (oldScrollY - scrollY));
        dispatchInputEvent(&e);
    }


    bool ScrollView::onInputEvent(InputEvent* e) {
        bool consumed = ViewGroup::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
            mouse_x_cache_ = e->getX();
            mouse_y_cache_ = e->getY();
            saved_pointer_type_ = e->getPointerType();
            processVerticalScroll(30 * e->getMouseWheel());
            consumed |= true;

        default:
            break;
        }

        return consumed;
    }

    bool ScrollView::onInterceptInputEvent(InputEvent* e) {
        return false;
    }

}
