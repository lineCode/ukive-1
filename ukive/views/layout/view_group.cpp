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
        STLDeleteElements(&views_);
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
        for (auto view : views_) {
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
        for (auto view : views_) {
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

        for (auto view : views_) {
            if (!view->isAttachedToWindow()) {
                view->onAttachedToWindow();
            }
        }
    }

    void ViewGroup::onDetachedFromWindow() {
        View::onDetachedFromWindow();

        for (auto view : views_) {
            if (view->isAttachedToWindow()) {
                view->onDetachedFromWindow();
            }
        }
    }


    void ViewGroup::addView(View* v, LayoutParams* params) {
        addView(STLCInt(views_.size()), v, params);
    }

    void ViewGroup::addView(int index, View* v, LayoutParams* params) {
        if (!v) {
            DCHECK(false) << "You cannot add a null View to ViewGroup.";
            return;
        }

        if (index < 0 || STLCST(views_, index) > views_.size()) {
            DCHECK(false) << "ViewGroup-addView(): Invalid index";
            return;
        }

        for (auto view : views_) {
            if (view == v) {
                return;
            }
        }

        if (!params) {
            params = v->getLayoutParams();
            if (!params) {
                params = generateDefaultLayoutParams();
                if (!params) {
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

        if (STLCST(views_, index) == views_.size()) {
            views_.push_back(v);
        } else {
            views_.insert(views_.begin() + index, v);
        }

        if (!v->isAttachedToWindow() && isAttachedToWindow()) {
            v->onAttachedToWindow();
        }

        requestLayout();
        invalidate();
    }

    void ViewGroup::removeView(View* v, bool del) {
        if (!v) {
            DLOG(Log::WARNING) << "You cannot remove a null view from ViewGroup.";
            return;
        }

        for (auto it = views_.begin(); it != views_.end(); ++it) {
            if ((*it) == v) {
                v->discardFocus();
                v->discardPendingOperations();

                if (v->isAttachedToWindow() && isAttachedToWindow()) {
                    v->onDetachedFromWindow();
                }

                v->setParent(nullptr);
                views_.erase(it);

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
        if (!views_.empty()) {
            for (auto child : views_) {
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

            views_.clear();
            requestLayout();
            invalidate();
        }
    }

    int ViewGroup::getChildCount() const {
        return STLCInt(views_.size());
    }

    View* ViewGroup::getChildById(int id) const {
        for (auto view : views_) {
            if (view->getId() == id) {
                return view;
            }
        }

        return nullptr;
    }

    View* ViewGroup::getChildAt(int index) const {
        return views_.at(STLCST(views_, index));
    }

    View* ViewGroup::findViewById(int id) const {
        std::queue<View*> curQueue;
        std::queue<View*> nextQueue;

        for (auto view : views_) {
            if (view->getId() == id) {
                return view;
            }
            auto child = view->findViewById(id);
            if (child) {
                return child;
            }
        }

        return nullptr;
    }

    void ViewGroup::dispatchDraw(Canvas* canvas) {
        drawChildren(canvas);
    }

    void ViewGroup::dispatchDiscardFocus() {
        for (auto view : views_) {
            view->discardFocus();
        }
    }

    void ViewGroup::dispatchDiscardPendingOperations() {
        for (auto view : views_) {
            view->discardPendingOperations();
        }
    }


    bool ViewGroup::dispatchMouseEvent(InputEvent* e) {
        bool consumed = false;
        bool intercepted = false;

        e->setMouseX(e->getMouseX() - getLeft() + getScrollX());
        e->setMouseY(e->getMouseY() - getTop() + getScrollY());

        if (onInterceptInputEvent(e) || onInterceptMouseEvent(e)) {
            intercepted = true;
            consumed = dispatchInputEventToThis(e);
            e->setEvent(InputEvent::EV_CANCEL);
        }

        // 从 View 列表的尾部开始遍历。因为最近添加的 View 在列表尾部，
        // 而最近添加的 View 可能会处于其他之前添加的 View 的上面（在绘制
        //  View 是从列表头开始的），这样一来与坐标相关的事件就可能发生在
        // 多个 View 交叠的区域，此时应该将该事件先发送至最上层的 View ，
        // 为此从 View 列表的尾部开始遍历。
        // 随后根据子 View 的dispatchInputEvent()方法的返回值来决定是否将
        // 该事件传递给下层的 View。
        for (auto it = views_.rbegin(); it != views_.rend(); ++it) {
            auto child = (*it);
            if (child->getVisibility() != View::VISIBLE || !child->isEnabled()) {
                continue;
            }

            int mx = e->getMouseX();
            int my = e->getMouseY();

            if (child->isParentMouseInThis(e) &&
                e->getEvent() != InputEvent::EVM_LEAVE_WIN &&
                e->getEvent() != InputEvent::EV_CANCEL)
            {
                if (!consumed) {
                    consumed = child->dispatchInputEvent(e);
                }
            } else if (child->isReceiveOutsideInputEvent() &&
                e->getEvent() != InputEvent::EVM_LEAVE_WIN &&
                e->getEvent() != InputEvent::EV_CANCEL)
            {
                if (!consumed) {
                    e->setOutside(true);

                    consumed = child->dispatchInputEvent(e);
                    child->setIsInputEventAtLast(false);

                    e->setOutside(false);
                }
            } else {
                int saved_event = e->getEvent();
                if (saved_event != InputEvent::EV_CANCEL) {
                    e->setEvent(InputEvent::EVM_LEAVE_VIEW);
                }
                child->dispatchInputEvent(e);
                e->setEvent(saved_event);
            }

            e->setMouseX(mx);
            e->setMouseY(my);
        }

        if (!intercepted && !consumed) {
            consumed = dispatchInputEventToThis(e);
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

    void ViewGroup::dispatchWindowFocusChanged(bool focus) {
        onWindowFocusChanged(focus);

        for (auto view : views_) {
            view->dispatchWindowFocusChanged(focus);
        }
    }

    void ViewGroup::dispatchWindowDpiChanged(int dpi_x, int dpi_y) {
        onWindowDpiChanged(dpi_x, dpi_y);

        for (auto view : views_) {
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
        if (child->isLayouted() &&
            child->getVisibility() == View::VISIBLE &&
            child->getWidth() > 0 && child->getHeight() > 0)
        {
            canvas->save();
            canvas->translate(child->getLeft(), child->getTop());
            child->draw(canvas);
            canvas->restore();
        }
    }

    void ViewGroup::drawChildren(Canvas* canvas) {
        for (auto view : views_) {
            drawChild(canvas, view);
        }
    }

    void ViewGroup::measureChild(
        View* child,
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode)
    {
        auto child_lp = child->getLayoutParams();

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
        int parent_width_mode, int parent_height_mode)
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
        int parent_width_mode, int parent_height_mode)
    {
        for (auto child : views_) {
            if (child->getVisibility() != View::VANISHED) {
                measureChild(
                    child, parent_width, parent_height, parent_width_mode, parent_height_mode);
            }
        }
    }

    void ViewGroup::measureChildrenWithMargins(
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode)
    {
        for (auto child : views_) {
            if (child->getVisibility() != View::VANISHED) {
                measureChildWithMargins(
                    child, parent_width, parent_height, parent_width_mode, parent_height_mode);
            }
        }
    }

    void ViewGroup::getChildMeasure(
        int parent_size, int parent_size_mode,
        int padding, int child_dimension,
        int* child_size, int* child_size_mode)
    {
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

        default:
            break;
        }

        *child_size = result_size;
        *child_size_mode = result_spec;
    }

}