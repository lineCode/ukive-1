#include "view_group.h"

#include <queue>

#include "ukive/event/input_event.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/graphics/canvas.h"
#include "ukive/log.h"
#include "ukive/utils/stl_utils.h"


namespace ukive {

    ViewGroup::ViewGroup(Window* w)
        :View(w) {
    }

    ViewGroup::~ViewGroup() {
        STLDeleteElements(&view_list_);
    }


    bool ViewGroup::checkLayoutParams(LayoutParams* lp) {
        return lp != nullptr;
    }

    LayoutParams* ViewGroup::generateDefaultLayoutParams() {
        return new LayoutParams(
            LayoutParams::FIT_CONTENT,
            LayoutParams::FIT_CONTENT);
    }

    LayoutParams* ViewGroup::generateLayoutParams(const LayoutParams &lp) {
        return new LayoutParams(lp);
    }

    int ViewGroup::getWrappedWidth() {
        int wrapped_width = 0;
        for (auto view : view_list_) {
            if (view->getVisibility() != View::VANISHED) {
                auto lp = view->getLayoutParams();

                int childWidth = view->getMeasuredWidth() + lp->leftMargin + lp->rightMargin;
                if (childWidth > wrapped_width) {
                    wrapped_width = childWidth;
                }
            }
        }

        return wrapped_width;
    }

    int ViewGroup::getWrappedHeight() {
        int wrapped_height = 0;
        for (auto view : view_list_) {
            if (view->getVisibility() != View::VANISHED) {
                auto lp = view->getLayoutParams();

                int childHeight = view->getMeasuredHeight() + lp->topMargin + lp->bottomMargin;
                if (childHeight > wrapped_height) {
                    wrapped_height = childHeight;
                }
            }
        }

        return wrapped_height;
    }


    void ViewGroup::onAttachedToWindow() {
        View::onAttachedToWindow();

        for (auto view : view_list_) {
            if (!view->isAttachedToWindow()) {
                view->onAttachedToWindow();
            }
        }
    }

    void ViewGroup::onDetachedFromWindow() {
        View::onDetachedFromWindow();

        for (auto view : view_list_) {
            if (view->isAttachedToWindow()) {
                view->onDetachedFromWindow();
            }
        }
    }


    void ViewGroup::addView(View* v, LayoutParams* params) {
        addView(view_list_.size(), v, params);
    }

    void ViewGroup::addView(size_t index, View* v, LayoutParams* params) {
        if (!v) {
            DCHECK(false) << "You cannot add a null View to ViewGroup.";
            return;
        }
        if (index > view_list_.size()) {
            DCHECK(false) << "ViewGroup-addView(): Invalid index";
            return;
        }

        for (auto view : view_list_) {
            if (view->getId() == v->getId()) {
                return;
            }
        }

        if (params == nullptr) {
            params = v->getLayoutParams();
            if (params == nullptr) {
                params = generateDefaultLayoutParams();
                if (params == nullptr) {
                    DLOG(Log::WARNING) << "Cannot generate default LayoutParams.";
                    return;
                }
            }
        }

        if (!checkLayoutParams(params)) {
            params = generateLayoutParams(*params);
        }

        v->setParent(this);
        if (params != v->getLayoutParams()) {
            v->setLayoutParams(params);
        }

        if (index == view_list_.size()) {
            view_list_.push_back(v);
        } else {
            view_list_.insert(view_list_.begin() + index, v);
        }

        if (!v->isAttachedToWindow() && isAttachedToWindow()) {
            v->onAttachedToWindow();
        }

        requestLayout();
        invalidate();
    }

    void ViewGroup::removeView(View* v, bool del) {
        if (v == nullptr) {
            DLOG(Log::WARNING) << "You cannot remove a null view from ViewGroup.";
            return;
        }

        for (auto it = view_list_.begin();
            it != view_list_.end(); ++it) {

            if ((*it)->getId() == v->getId()) {
                v->discardFocus();
                v->discardPendingOperations();

                if (v->isAttachedToWindow() && isAttachedToWindow()) {
                    v->onDetachedFromWindow();
                }

                v->setParent(nullptr);
                view_list_.erase(it);

                if (del) {
                    delete v;
                }

                requestLayout();
                invalidate();
                return;
            }
        }
    }

    void ViewGroup::removeAllViews(bool del) {
        if (!view_list_.empty()) {
            for (auto child : view_list_) {
                child->discardFocus();
                child->discardPendingOperations();
                if (child->isAttachedToWindow() && isAttachedToWindow()) {
                    child->onDetachedFromWindow();
                }
                child->setParent(nullptr);

                if (del) {
                    delete child;
                }
            }

            view_list_.clear();
            requestLayout();
            invalidate();
        }
    }

    size_t ViewGroup::getChildCount() {
        return view_list_.size();
    }

    View* ViewGroup::getChildById(int id) {
        for (auto view : view_list_) {
            if (view->getId() == id) {
                return view;
            }
        }

        return nullptr;
    }

    View* ViewGroup::getChildAt(size_t index) {
        return view_list_.at(index);
    }

    View* ViewGroup::findViewById(int id) {
        std::queue<View*> curQueue;
        std::queue<View*> nextQueue;

        for (auto view : view_list_) {
            if (view->getId() == id) {
                return view;
            } else {
                View* child = view->findViewById(id);
                if (child) {
                    return child;
                }
            }
        }

        return nullptr;
    }

    void ViewGroup::dispatchDraw(Canvas* canvas) {
        drawChildren(canvas);
    }

    void ViewGroup::dispatchDiscardFocus() {
        for (auto view : view_list_) {
            view->discardFocus();
        }
    }

    void ViewGroup::dispatchDiscardPendingOperations() {
        for (auto view : view_list_) {
            view->discardPendingOperations();
        }
    }


    bool ViewGroup::dispatchMouseEvent(InputEvent* e) {
        bool consumed = false;
        bool intercepted = false;

        e->setMouseX(e->getMouseX() - getLeft() + getScrollX());
        e->setMouseY(e->getMouseY() - getTop() + getScrollY());

        if (onInterceptInputEvent(e) || onInterceptMouseEvent(e)) {
            consumed = onInputEvent(e);
            intercepted = true;
            e->setEvent(InputEvent::EV_CANCEL);
        }

        // 从 View 列表的尾部开始遍历。因为最近添加的 View 在列表尾部，
        // 而最近添加的 View 可能会处于其他之前添加的 View 的上面（在绘制
        //  View 是从列表头开始的），这样一来与坐标相关的事件就可能发生在
        // 多个 View 交叠的区域，此时应该将该事件先发送至最上层的 View ，
        // 为此从 View 列表的尾部开始遍历。
        // 随后根据子 View 的dispatchInputEvent()方法的返回值来决定是否将
        // 该事件传递给下层的 View。
        for (auto it = view_list_.rbegin();
            it != view_list_.rend(); ++it) {

            View* child = (*it);
            if (child->getVisibility() != View::VISIBLE
                || !child->isEnabled()) {
                continue;
            }

            int mx = e->getMouseX();
            int my = e->getMouseY();

            if (child->isParentMouseInThis(e)
                && e->getEvent() != InputEvent::EVM_LEAVE_WIN
                && e->getEvent() != InputEvent::EV_CANCEL) {

                if (!consumed) {
                    consumed = child->dispatchInputEvent(e);
                    child->setIsInputEventAtLast(true);
                }
            } else if (child->isReceiveOutsideInputEvent()
                && e->getEvent() != InputEvent::EVM_LEAVE_WIN
                && e->getEvent() != InputEvent::EV_CANCEL) {

                if (!consumed) {
                    e->setOutside(true);

                    consumed = child->dispatchInputEvent(e);
                    child->setIsInputEventAtLast(false);

                    e->setOutside(false);
                }
            } else {
                if (child->isInputEventAtLast()) {
                    int savedEvent = e->getEvent();

                    if (savedEvent == InputEvent::EV_CANCEL) {
                        e->setEvent(InputEvent::EV_CANCEL);
                    }
                    else {
                        e->setEvent(InputEvent::EVM_LEAVE_VIEW);
                    }

                    child->dispatchInputEvent(e);
                    child->setIsInputEventAtLast(false);
                    e->setEvent(savedEvent);
                }
            }

            e->setMouseX(mx);
            e->setMouseY(my);
        }

        if (!intercepted && !consumed) {
            consumed = onInputEvent(e);
        }

        return consumed;
    }

    bool ViewGroup::dispatchKeyboardEvent(InputEvent* e) {
        if (onInterceptInputEvent(e)) {
            return onInputEvent(e);
        }

        if (onInterceptKeyboardEvent(e)) {
            return onInputEvent(e);
        }

        // 键盘事件不参与分发。
        return false;
    }

    bool ViewGroup::dispatchInputEvent(InputEvent* e) {
        bool consumed = false;

        if (e->isMouseEvent() && !e->isMouseCaptured()) {
            consumed = dispatchMouseEvent(e);
        } else if (e->isKeyboardEvent()) {
            consumed = dispatchKeyboardEvent(e);
        } else {
            consumed = onInputEvent(e);
        }

        return consumed;
    }

    void ViewGroup::dispatchWindowFocusChanged(bool windowFocus) {
        onWindowFocusChanged(windowFocus);

        for (auto view : view_list_) {
            view->dispatchWindowFocusChanged(windowFocus);
        }
    }

    void ViewGroup::dispatchWindowDpiChanged(int dpi_x, int dpi_y) {
        onWindowDpiChanged(dpi_x, dpi_y);

        for (auto view : view_list_) {
            view->dispatchWindowDpiChanged(dpi_x, dpi_y);
        }
    }

    bool ViewGroup::onInterceptInputEvent(InputEvent* e) {
        return false;
    }

    bool ViewGroup::onInterceptMouseEvent(InputEvent* e) {
        return false;
    }

    bool ViewGroup::onInterceptKeyboardEvent(InputEvent* e) {
        return false;
    }


    void ViewGroup::drawChild(Canvas* canvas, View* child) {
        if (child->isLayouted()
            && child->getVisibility() == View::VISIBLE
            && child->getWidth() > 0 && child->getHeight() > 0) {

            canvas->save();
            canvas->translate(child->getLeft(), child->getTop());
            child->draw(canvas);
            canvas->restore();
        }
    }

    void ViewGroup::drawChildren(Canvas* canvas) {
        for (auto view : view_list_) {
            drawChild(canvas, view);
        }
    }

    void ViewGroup::measureChild(
        View* child,
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode) {

        LayoutParams* child_lp = child->getLayoutParams();

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        int childWidth;
        int childWidthSpec;
        int childHeight;
        int childHeightSpec;

        getChildMeasure(
            parent_width, parent_width_mode,
            hori_padding,
            child_lp->width, &childWidth, &childWidthSpec);

        getChildMeasure(
            parent_height, parent_height_mode,
            vert_padding,
            child_lp->height, &childHeight, &childHeightSpec);

        child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
    }

    void ViewGroup::measureChildWithMargins(
        View* child,
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode) {

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
            parent_width, parent_width_mode,
            hori_margin + hori_padding,
            child_lp->width, &childWidth, &childWidthSpec);

        getChildMeasure(
            parent_height, parent_height_mode,
            vert_margin + vert_padding,
            child_lp->height, &childHeight, &childHeightSpec);

        child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
    }

    void ViewGroup::measureChildren(
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode) {

        for (auto child : view_list_) {
            if (child->getVisibility() != View::VANISHED) {
                measureChild(
                    child, parent_width, parent_height, parent_width_mode, parent_height_mode);
            }
        }
    }

    void ViewGroup::measureChildrenWithMargins(
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode) {

        for (auto child : view_list_) {
            if (child->getVisibility() != View::VANISHED) {
                measureChildWithMargins(
                    child, parent_width, parent_height, parent_width_mode, parent_height_mode);
            }
        }
    }

    void ViewGroup::getChildMeasure(
        int parent_size, int parent_size_mode,
        int padding, int child_dimension,
        int* child_size, int* child_size_mode) {

        int size = std::max(0, parent_size - padding);

        int result_size = 0;
        int result_spec = 0;

        switch (parent_size_mode) {
            // 父 ViewGroup 以其子 View 决定大小。
            // 此时，若子 View 的大小未给出，应测量自身。
        case FIT:
            if (child_dimension >= 0) {
                result_size = child_dimension;
                result_spec = EXACTLY;
            } else if (child_dimension == LayoutParams::MATCH_PARENT) {
                result_size = size;
                result_spec = FIT;
            } else if (child_dimension == LayoutParams::FIT_CONTENT) {
                result_size = size;
                result_spec = FIT;
            }
            break;

            // 父 ViewGroup 的大小已确定。
            // 此时，子 View 的度量模式不变。
        case EXACTLY:
            if (child_dimension >= 0) {
                result_size = child_dimension;
                result_spec = EXACTLY;
            } else if (child_dimension == LayoutParams::MATCH_PARENT) {
                result_size = size;
                result_spec = EXACTLY;
            } else if (child_dimension == LayoutParams::FIT_CONTENT) {
                result_size = size;
                result_spec = FIT;
            }
            break;

            // 父 ViewGroup 的大小未知。
            // 此时，除非子 View 的大小已给出，否则子 View 的度量模式也为未知。
        case UNKNOWN:
            if (child_dimension >= 0) {
                result_size = child_dimension;
                result_spec = EXACTLY;
            } else if (child_dimension == LayoutParams::MATCH_PARENT) {
                result_size = size;
                result_spec = UNKNOWN;
            } else if (child_dimension == LayoutParams::FIT_CONTENT) {
                result_size = size;
                result_spec = UNKNOWN;
            }
            break;
        }

        *child_size = result_size;
        *child_size_mode = result_spec;
    }

}