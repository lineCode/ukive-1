#include "view_group.h"

#include <queue>

#include "ukive/event/input_event.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/graphics/canvas.h"
#include "ukive/log.h"
#include "ukive/utils/stl_utils.h"


namespace ukive {

    ViewGroup::ViewGroup(Window *wnd)
        :View(wnd) {
        initViewGroup();
    }

    ViewGroup::ViewGroup(Window *wnd, int id)
        : View(wnd, id) {
        initViewGroup();
    }


    ViewGroup::~ViewGroup() {
        STLDeleteElements(&view_list_);
    }


    void ViewGroup::initViewGroup() {
    }


    LayoutParams *ViewGroup::generateLayoutParams(const LayoutParams &lp) {
        return new LayoutParams(lp);
    }

    LayoutParams *ViewGroup::generateDefaultLayoutParams() {
        return new LayoutParams(
            LayoutParams::FIT_CONTENT,
            LayoutParams::FIT_CONTENT);
    }

    bool ViewGroup::checkLayoutParams(LayoutParams *lp) {
        return lp != nullptr;
    }


    int ViewGroup::getWrappedWidth() {
        int wrappedWidth = 0;
        for (std::size_t i = 0; i < getChildCount(); ++i) {
            View *widget = getChildAt(i);
            if (widget->getVisibility() != View::VANISHED) {
                auto lp = widget->getLayoutParams();

                int childWidth = widget->getMeasuredWidth() + lp->leftMargin + lp->rightMargin;
                if (childWidth > wrappedWidth) {
                    wrappedWidth = childWidth;
                }
            }
        }

        return wrappedWidth;
    }

    int ViewGroup::getWrappedHeight() {
        int wrappedHeight = 0;
        for (std::size_t i = 0; i < getChildCount(); ++i) {
            View *widget = getChildAt(i);
            if (widget->getVisibility() != View::VANISHED) {
                auto lp = widget->getLayoutParams();

                int childHeight = widget->getMeasuredHeight() + lp->topMargin + lp->bottomMargin;
                if (childHeight > wrappedHeight) {
                    wrappedHeight = childHeight;
                }
            }
        }

        return wrappedHeight;
    }


    void ViewGroup::onAttachedToWindow() {
        View::onAttachedToWindow();

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            if (!(*it)->isAttachedToWindow()) {
                (*it)->onAttachedToWindow();
            }
        }
    }

    void ViewGroup::onDetachedFromWindow() {
        View::onDetachedFromWindow();

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            if ((*it)->isAttachedToWindow()) {
                (*it)->onDetachedFromWindow();
            }
        }
    }


    void ViewGroup::addView(View *v, LayoutParams *params) {
        addView(view_list_.size(), v, params);
    }

    void ViewGroup::addView(std::size_t index, View *v, LayoutParams *params)
    {
        if (v == nullptr) {
            throw std::invalid_argument(
                "ViewGroup-addView(): You cannot add a null view to ViewGroup.");
        }
        if (index > view_list_.size()) {
            throw std::invalid_argument(
                "ViewGroup-addView(): invalid index");
        }

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            if ((*it)->getId() == v->getId()) {
                return;
            }
        }

        if (params == nullptr) {
            params = v->getLayoutParams();
            if (params == nullptr) {
                params = generateDefaultLayoutParams();
                if (params == nullptr) {
                    throw std::logic_error(
                        "ViewGroup-addView(): You cannot add a null view to ViewGroup.");
                }
            }
        }

        if (!checkLayoutParams(params)) {
            params = generateLayoutParams(*params);
        }

        v->setParent(this);
        v->setLayoutParams(params);

        if (index == view_list_.size()) {
            view_list_.push_back(v);
        }
        else {
            view_list_.insert(view_list_.begin() + index, v);
        }

        if (!v->isAttachedToWindow() && isAttachedToWindow()) {
            v->onAttachedToWindow();
        }

        requestLayout();
        invalidate();
    }

    void ViewGroup::removeView(View *v, bool del) {
        if (v == nullptr) {
            Log::e(L"ViewGroup-removeView(): You cannot remove a null view from ViewGroup.");
            return;
        }

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it)
        {
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
            if (del) {
                for (auto it = view_list_.begin();
                    it != view_list_.end(); ++it) {
                    delete (*it);
                }
            }

            view_list_.clear();
            requestLayout();
            invalidate();
        }
    }


    View *ViewGroup::findViewById(int id) {
        std::queue<View*> curQueue;
        std::queue<View*> nextQueue;

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {

            if ((*it)->getId() == id) {
                return (*it);
            }
            else {
                View *widget = (*it)->findViewById(id);
                if (widget) {
                    return widget;
                }
            }
        }

        return nullptr;
    }


    std::size_t ViewGroup::getChildCount() {
        return view_list_.size();
    }

    View *ViewGroup::getChildById(int id) {
        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            if ((*it)->getId() == id) {
                return (*it);
            }
        }

        return nullptr;
    }

    View *ViewGroup::getChildAt(std::size_t index) {
        return view_list_.at(index);
    }


    void ViewGroup::dispatchDraw(Canvas *canvas) {
        drawChildren(canvas);
    }

    void ViewGroup::dispatchDiscardFocus() {
        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            (*it)->discardFocus();
        }
    }

    void ViewGroup::dispatchDiscardPendingOperations() {
        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            (*it)->discardPendingOperations();
        }
    }


    bool ViewGroup::dispatchMouseEvent(InputEvent *e) {
        bool consumed = false;
        bool isIntercepted = false;

        e->setMouseX(e->getMouseX() - getLeft() + getScrollX());
        e->setMouseY(e->getMouseY() - getTop() + getScrollY());

        if (this->onInterceptInputEvent(e)
            || this->onInterceptMouseEvent(e)) {

            consumed = this->onInputEvent(e);
            isIntercepted = true;
            e->setEvent(InputEvent::EV_CANCEL);
        }

        //从Widget列表的尾部开始遍历。因为最近添加的Widget在列表尾部，
        //而最近添加的Widget可能会处于其他之前添加的Widget的上面（在绘制
        //Widget是从列表头开始的），这样一来与坐标相关的事件就可能发生在
        //多个Widget交叠的区域，此时应该将该事件先发送至最上层的Widget，
        //为此从Widget列表的尾部开始遍历。
        //随后根据子Widget的dispatchInputEvent()方法的返回值来决定是否将
        //该事件传递给下层的Widget。
        for (auto it = view_list_.rbegin();
            it != view_list_.rend();
            ++it) {

            View *child = (*it);
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
            }
            else if (child->isReceiveOutsideInputEvent()
                && e->getEvent() != InputEvent::EVM_LEAVE_WIN
                && e->getEvent() != InputEvent::EV_CANCEL) {

                if (!consumed) {
                    e->setOutside(true);

                    consumed = child->dispatchInputEvent(e);
                    child->setIsInputEventAtLast(false);

                    e->setOutside(false);
                }
            }
            else {
                if (child->isInputEventAtLast()) {
                    int savedEvent = e->getEvent();

                    if (savedEvent == InputEvent::EV_CANCEL) {
                        e->setEvent(InputEvent::EV_CANCEL);
                    }
                    else {
                        e->setEvent(InputEvent::EVM_LEAVE_OBJ);
                    }

                    child->dispatchInputEvent(e);
                    child->setIsInputEventAtLast(false);
                    e->setEvent(savedEvent);
                }
            }

            e->setMouseX(mx);
            e->setMouseY(my);
        }

        if (!isIntercepted && !consumed) {
            consumed = this->onInputEvent(e);
        }

        return consumed;
    }

    bool ViewGroup::dispatchKeyboardEvent(InputEvent *e) {
        if (this->onInterceptInputEvent(e)) {
            return this->onInputEvent(e);
        }

        if (this->onInterceptKeyboardEvent(e)) {
            return this->onInputEvent(e);
        }

        //键盘事件不参与分发。
        return false;
    }

    bool ViewGroup::dispatchInputEvent(InputEvent *e) {
        bool consumed = false;

        if (e->isMouseEvent() && !e->isMouseCaptured()) {
            consumed = this->dispatchMouseEvent(e);
        }
        else if (e->isKeyboardEvent()) {
            consumed = this->dispatchKeyboardEvent(e);
        }
        else {
            consumed = this->onInputEvent(e);
        }

        return consumed;
    }

    void ViewGroup::dispatchWindowFocusChanged(bool windowFocus) {
        onWindowFocusChanged(windowFocus);

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            (*it)->dispatchWindowFocusChanged(windowFocus);
        }
    }

    void ViewGroup::dispatchWindowDpiChanged(int dpi_x, int dpi_y) {
        onWindowDpiChanged(dpi_x, dpi_y);

        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            (*it)->dispatchWindowDpiChanged(dpi_x, dpi_y);
        }
    }

    bool ViewGroup::onInterceptInputEvent(InputEvent *e) {
        return false;
    }

    bool ViewGroup::onInterceptMouseEvent(InputEvent *e) {
        return false;
    }

    bool ViewGroup::onInterceptKeyboardEvent(InputEvent *e) {
        return false;
    }


    void ViewGroup::drawChild(Canvas *canvas, View *child) {
        if (child->isLayouted()
            && child->getVisibility() == View::VISIBLE
            && child->getWidth() > 0 && child->getHeight() > 0) {

            canvas->save();
            canvas->translate(child->getLeft(), child->getTop());
            child->draw(canvas);
            canvas->restore();
        }
    }

    void ViewGroup::drawChildren(Canvas *canvas)
    {
        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {
            drawChild(canvas, *it);
        }
    }


    void ViewGroup::getChildMeasure(
        int parentSize, int parentSizeMode,
        int padding, int childDimension,
        int *childSize, int *childSizeMode)
    {
        int size = std::max(0, parentSize - padding);

        int resultSize = 0;
        int resultSpec = 0;

        switch (parentSizeMode) {
            //父ViewGroup以其子View决定大小。
            //此时，若子View的大小未给出，应测量自身。
        case FIT:
            if (childDimension >= 0) {
                resultSize = childDimension;
                resultSpec = EXACTLY;
            }
            else if (childDimension == LayoutParams::MATCH_PARENT) {
                resultSize = size;
                resultSpec = FIT;
            }
            else if (childDimension == LayoutParams::FIT_CONTENT) {
                resultSize = size;
                resultSpec = FIT;
            }
            break;

            //父ViewGroup的大小已确定。
            //此时，子View的度量模式不变。
        case EXACTLY:
            if (childDimension >= 0) {
                resultSize = childDimension;
                resultSpec = EXACTLY;
            }
            else if (childDimension == LayoutParams::MATCH_PARENT) {
                resultSize = size;
                resultSpec = EXACTLY;
            }
            else if (childDimension == LayoutParams::FIT_CONTENT) {
                resultSize = size;
                resultSpec = FIT;
            }
            break;

            //父ViewGroup的大小未知。
            //此时，除非子View的大小已给出，否则子View的度量模式也为未知。
        case UNKNOWN:
            if (childDimension >= 0) {
                resultSize = childDimension;
                resultSpec = EXACTLY;
            }
            else if (childDimension == LayoutParams::MATCH_PARENT) {
                resultSize = size;
                resultSpec = UNKNOWN;
            }
            else if (childDimension == LayoutParams::FIT_CONTENT) {
                resultSize = size;
                resultSpec = UNKNOWN;
            }
            break;
        }

        *childSize = resultSize;
        *childSizeMode = resultSpec;
    }


    void ViewGroup::measureChild(
        View *child,
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        LayoutParams *childParams = child->getLayoutParams();

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        int childWidth;
        int childWidthSpec;
        int childHeight;
        int childHeightSpec;

        getChildMeasure(
            parentWidth, parentWidthMode,
            horizontalPadding,
            childParams->width, &childWidth, &childWidthSpec);

        getChildMeasure(
            parentHeight, parentHeightMode,
            verticalPadding,
            childParams->height, &childHeight, &childHeightSpec);

        child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
    }


    void ViewGroup::measureChildWithMargins(
        View *child,
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        LayoutParams *childParams = child->getLayoutParams();

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        int horizontalMargins = childParams->leftMargin + childParams->rightMargin;
        int verticalMargins = childParams->topMargin + childParams->bottomMargin;

        int childWidth;
        int childWidthSpec;
        int childHeight;
        int childHeightSpec;

        getChildMeasure(
            parentWidth, parentWidthMode,
            horizontalMargins + horizontalPadding,
            childParams->width, &childWidth, &childWidthSpec);

        getChildMeasure(
            parentHeight, parentHeightMode,
            verticalMargins + verticalPadding,
            childParams->height, &childHeight, &childHeightSpec);

        child->measure(childWidth, childHeight, childWidthSpec, childHeightSpec);
    }


    void ViewGroup::measureChildren(
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        for (auto it = view_list_.begin();
            it != view_list_.end();
            ++it) {

            if ((*it)->getVisibility() != View::VANISHED) {
                measureChild(
                    *it, parentWidth, parentHeight, parentWidthMode, parentHeightMode);
            }
        }
    }

    void ViewGroup::measureChildrenWithMargins(
        int parentWidth, int parentHeight,
        int parentWidthMode, int parentHeightMode)
    {
        for (auto it = view_list_.begin();
            it != view_list_.end(); ++it) {

            if ((*it)->getVisibility() != View::VANISHED) {
                measureChildWithMargins(
                    *it, parentWidth, parentHeight, parentWidthMode, parentHeightMode);
            }
        }
    }

}