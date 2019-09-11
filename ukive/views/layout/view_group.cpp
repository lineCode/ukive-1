#include "view_group.h"

#include <queue>

#include "ukive/event/input_event.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/graphics/canvas.h"
#include "ukive/log.h"
#include "ukive/utils/stl_utils.h"
#include "ukive/resources/dimension_utils.h"

#include "oigka/layout_constants.h"


namespace ukive {

    ViewGroup::ViewGroup(Window* w)
        : ViewGroup(w, {}) {
    }

    ViewGroup::ViewGroup(Window* w, AttrsRef attrs)
        : View(w, attrs) {

    }

    ViewGroup::~ViewGroup() {
        STLDeleteElements(&views_);
    }

    bool ViewGroup::checkLayoutParams(LayoutParams* lp) const {
        return lp != nullptr;
    }

    LayoutParams* ViewGroup::generateDefaultLayoutParams() const {
        return new LayoutParams(
            LayoutParams::FIT_CONTENT,
            LayoutParams::FIT_CONTENT);
    }

    LayoutParams* ViewGroup::generateLayoutParams(const LayoutParams &lp) const {
        return new LayoutParams(lp);
    }

    LayoutParams* ViewGroup::generateLayoutParamsByAttrs(AttrsRef attrs) const {
        int width = LayoutParams::FIT_CONTENT;
        int height = LayoutParams::FIT_CONTENT;

        auto width_attr = resolveAttrString(
            attrs, oigka::kAttrLayoutWidth, oigka::kAttrValLayoutFit);
        if (isEqual(width_attr, oigka::kAttrValLayoutFit, false)) {
            width = LayoutParams::FIT_CONTENT;
        } else if (isEqual(width_attr, oigka::kAttrValLayoutMatch, false)) {
            width = LayoutParams::MATCH_PARENT;
        }

        auto height_attr = resolveAttrString(
            attrs, oigka::kAttrLayoutHeight, oigka::kAttrValLayoutFit);
        if (isEqual(height_attr, oigka::kAttrValLayoutFit, false)) {
            height = LayoutParams::FIT_CONTENT;
        } else if (isEqual(height_attr, oigka::kAttrValLayoutMatch, false)) {
            height = LayoutParams::MATCH_PARENT;
        }

        auto lp = new LayoutParams(width, height);
        lp->left_margin = lp->right_margin = lp->top_margin = lp->bottom_margin
            = resolveAttrDimension(getWindow(), attrs, oigka::kAttrLayoutMargin, 0);

        lp->left_margin = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrLayoutMarginStart, lp->left_margin);
        lp->right_margin = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrLayoutMarginEnd, lp->right_margin);
        lp->top_margin = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrLayoutMarginTop, lp->top_margin);
        lp->bottom_margin = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrLayoutMarginBottom, lp->bottom_margin);

        return lp;
    }

    int ViewGroup::getWrappedWidth() {
        int wrapped_width = 0;
        for (auto view : views_) {
            if (view->getVisibility() != View::VANISHED) {
                auto lp = view->getLayoutParams();

                int childWidth = view->getMeasuredWidth() + lp->left_margin + lp->right_margin;
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

                int childHeight = view->getMeasuredHeight() + lp->top_margin + lp->bottom_margin;
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

    bool ViewGroup::isViewGroup() const {
        return true;
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

    bool ViewGroup::dispatchPointerEvent(InputEvent* e) {
        bool consumed = false;
        bool intercepted = false;

        e->offsetInputPos(-getLeft() + getScrollX(), -getTop() + getScrollY());

        if (is_intercepted_ || onInterceptInputEvent(e)) {
            if (e->getEvent() == InputEvent::EVM_DOWN ||
                e->getEvent() == InputEvent::EVT_DOWN)
            {
                is_intercepted_ = true;
            }

            if (e->isNoActiveEvent() ||
                e->getEvent() == InputEvent::EVM_UP ||
                e->getEvent() == InputEvent::EVT_UP)
            {
                is_intercepted_ = false;
            }

            intercepted = true;
            consumed = dispatchInputEventToThis(e);
            if (e->getEvent() == InputEvent::EVT_DOWN) {
                return consumed;
            }
            e->setEvent(InputEvent::EV_CANCEL);
        }

        // 从 View 列表的尾部开始遍历。因为最近添加的 View 在列表尾部，
        // 而最近添加的 View 可能会处于其他之前添加的 View 的上面（在绘制
        //  View 是从列表头开始的），这样一来与坐标相关的事件就可能发生在
        // 多个 View 交叠的区域，此时应该将该事件先发送至最上层的 View ，
        // 为此从 View 列表的尾部开始遍历。
        // 随后根据子 View 的 dispatchInputEvent() 方法的返回值来决定是否将
        // 该事件传递给下层的 View。
        for (auto it = views_.rbegin(); it != views_.rend(); ++it) {
            auto child = (*it);
            if (child->getVisibility() != View::VISIBLE || !child->isEnabled()) {
                continue;
            }

            int mx = e->getX();
            int my = e->getY();

            if (child->isParentPointerInThis(e) &&
                e->getEvent() != InputEvent::EV_CANCEL)
            {
                if (!consumed) {
                    consumed = child->dispatchInputEvent(e);
                }
            } else if (child->isReceiveOutsideInputEvent() &&
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
                    e->setEvent(InputEvent::EV_LEAVE_VIEW);
                }
                child->dispatchInputEvent(e);
                e->setEvent(saved_event);
            }

            e->setX(mx);
            e->setY(my);
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
        // 键盘事件不参与分发。
        return false;
    }

    bool ViewGroup::dispatchInputEvent(InputEvent* e) {
        bool consumed;
        if ((e->isMouseEvent() || e->isTouchEvent()) && !e->isNoDispatch()) {
            consumed = dispatchPointerEvent(e);
        } else if (e->isKeyboardEvent()) {
            consumed = dispatchKeyboardEvent(e);
        } else {
            // ViewGroup 拦截事件后如果获得了鼠标焦点，则
            // 鼠标事件将走到这里而不是 dispatchPointerEvent()，
            // 所以需要在这里进行拦截变量的重置
            if (is_intercepted_ &&
                (e->isNoActiveEvent() ||
                e->getEvent() == InputEvent::EVM_UP ||
                e->getEvent() == InputEvent::EVT_UP))
            {
                is_intercepted_ = false;
            }

            consumed = View::dispatchInputEvent(e);
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

        int child_w;
        int child_w_mode;
        int child_h;
        int child_h_mode;

        getChildMeasure(
            parent_width, parent_width_mode,
            hori_padding,
            child_lp->width, &child_w, &child_w_mode);

        getChildMeasure(
            parent_height, parent_height_mode,
            vert_padding,
            child_lp->height, &child_h, &child_h_mode);

        child->measure(child_w, child_h, child_w_mode, child_h_mode);
    }

    void ViewGroup::measureChildWithMargins(
        View* child,
        int parent_width, int parent_height,
        int parent_width_mode, int parent_height_mode)
    {
        auto child_lp = child->getLayoutParams();

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        int hori_margin = child_lp->left_margin + child_lp->right_margin;
        int vert_margin = child_lp->top_margin + child_lp->bottom_margin;

        int child_w;
        int child_w_mode;
        int child_h;
        int child_h_mode;

        getChildMeasure(
            parent_width, parent_width_mode,
            hori_margin + hori_padding,
            child_lp->width, &child_w, &child_w_mode);

        getChildMeasure(
            parent_height, parent_height_mode,
            vert_margin + vert_padding,
            child_lp->height, &child_h, &child_h_mode);

        child->measure(child_w, child_h, child_w_mode, child_h_mode);
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