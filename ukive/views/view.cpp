#include "view.h"

#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/drawable/drawable.h"
#include "ukive/animation/view_animator.h"
#include "ukive/text/input_connection.h"
#include "ukive/views/click_listener.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/application.h"
#include "ukive/message/cycler.h"
#include "ukive/graphics/bitmap_factory.h"


namespace ukive {

    View::View(Window *wnd)
        :View(wnd, Application::getViewUID()) {
    }

    View::View(Window *wnd, int id) {
        mAlpha = 1;
        mScaleX = 1;
        mScaleY = 1;
        mTranslateX = 0;
        mTranslateY = 0;
        mPivotX = mPivotY = 0;

        mRevealType = ViewAnimator::REVEAL_CIRCULE;
        mHasReveal = false;
        mRevealRadius = 0;
        mRevealCenterX = mRevealCenterY = 0;
        mRevealWidthRadius = mRevealHeightRadius = 0;

        id_ = id;
        mLeft = mRight = mTop = mBottom = 0;
        measured_width_ = measured_height_ = 0;
        min_width_ = min_height_ = 0;

        mScrollX = mScrollY = 0;
        mPaddingLeft = mPaddingRight = mPaddingTop = mPaddingBottom = 0;

        elevation_ = 0.f;

        visibility_ = VISIBLE;
        is_enabled_ = true;
        is_attached_to_window_ = false;
        is_input_event_at_last_ = false;
        is_pressed_ = false;
        has_focus_ = false;
        is_focusable_ = false;
        is_layouted_ = false;
        is_receive_outside_input_event_ = false;
        can_consume_mouse_event_ = true;

        window_ = wnd;
        layout_params_ = 0;
        parent_ = 0;
        input_connection_ = 0;

        bg_drawable_ = nullptr;
        fg_drawable_ = nullptr;
        click_listener_ = nullptr;
        animator_ = nullptr;

        click_performer_ = new ClickPerformer(this);
    }


    View::~View() {
        delete click_performer_;

        if (input_connection_ != nullptr)
            delete input_connection_;
    }


    ViewAnimator *View::animate() {
        if (animator_ == nullptr)
            animator_ = new ViewAnimator(this);

        return animator_;
    }

    void View::setX(double x) {
        setTranslateX(x - mLeft);
    }

    void View::setY(double y) {
        setTranslateY(y - mTop);
    }

    void View::setAlpha(double alpha) {
        mAlpha = alpha;
    }

    void View::setScaleX(double sx) {
        mScaleX = sx;
    }

    void View::setScaleY(double sy) {
        mScaleY = sy;
    }

    void View::setTranslateX(double tx) {
        mTranslateX = tx;
    }

    void View::setTranslateY(double ty) {
        mTranslateY = ty;
    }

    void View::setPivotX(double px) {
        mPivotX = px;
    }

    void View::setPivotY(double py) {
        mPivotY = py;
    }

    void View::setRevealType(int type) {
        mRevealType = type;
    }

    void View::setHasReveal(bool reveal) {
        mHasReveal = reveal;
    }

    void View::setRevealRadius(double radius) {
        mRevealRadius = radius;
    }

    void View::setRevealCenterX(double cx) {
        mRevealCenterX = cx;
    }

    void View::setRevealCenterY(double cy) {
        mRevealCenterY = cy;
    }

    void View::setRevealWidthRadius(double widthRadius) {
        mRevealWidthRadius = widthRadius;
    }

    void View::setRevealHeightRadius(double heightRadius) {
        mRevealHeightRadius = heightRadius;
    }

    void View::setScrollX(int x) {
        if (mScrollX != x) {
            mScrollX = x;
            invalidate();
        }
    }

    void View::setScrollY(int y) {
        if (mScrollY != y) {
            mScrollY = y;
            invalidate();
        }
    }

    void View::setMeasuredDimension(int width, int height) {
        measured_width_ = width;
        measured_height_ = height;
    }

    void View::setVisibility(int visibility) {
        if (visibility == visibility_)
            return;

        visibility_ = visibility;

        if (visibility_ != VISIBLE) {
            discardFocus();
            discardPendingOperations();
        }

        requestLayout();
        invalidate();
        onVisibilityChanged(visibility);
    }

    void View::setEnabled(bool enable) {
        if (enable == is_enabled_)
            return;

        is_enabled_ = enable;

        if (!is_enabled_) {
            discardFocus();
            discardPendingOperations();
        }

        invalidate();
    }

    void View::setBackground(Drawable *drawable) {
        bg_drawable_.reset(drawable);
        invalidate();
    }

    void View::setForeground(Drawable *drawable) {
        fg_drawable_.reset(drawable);
        invalidate();
    }

    void View::setPadding(int left, int top, int right, int bottom) {
        if (mPaddingLeft == left
            && mPaddingTop == top
            && mPaddingRight == right
            && mPaddingBottom == bottom)
            return;

        mPaddingLeft = left;
        mPaddingTop = top;
        mPaddingRight = right;
        mPaddingBottom = bottom;

        requestLayout();
        invalidate();
    }

    void View::setLayoutParams(LayoutParams *params) {
        if (params == nullptr)
            throw std::invalid_argument("setLayoutParams: null param");

        layout_params_ = params;

        requestLayout();
        invalidate();
    }

    void View::setIsInputEventAtLast(bool isInput) {
        is_input_event_at_last_ = isInput;
    }

    void View::setPressed(bool pressed) {
        if (is_pressed_ == pressed)
            return;

        is_pressed_ = pressed;

        invalidate();
    }

    void View::setCurrentCursor(const string16 &cursor) {
        window_->setCurrentCursor(cursor);
    }

    void View::setFocusable(bool focusable) {
        if (is_focusable_ == focusable)
            return;

        is_focusable_ = focusable;

        if (!focusable) {
            discardFocus();
        }
    }

    void View::setElevation(float elevation) {
        if (elevation == elevation_) {
            return;
        }

        elevation_ = elevation;

        requestLayout();
        invalidate();
    }

    void View::setReceiveOutsideInputEvent(bool receive) {
        is_receive_outside_input_event_ = receive;
    }

    void View::setCanConsumeMouseEvent(bool enable) {
        can_consume_mouse_event_ = enable;

        if (!enable)
            window_->releaseMouse();
    }

    void View::setParent(View *parent) {
        parent_ = parent;
    }

    void View::offsetTopAndBottom(int dy) {
        mTop += dy;
        mBottom += dy;

        invalidate();
    }

    void View::offsetLeftAndRight(int dx) {
        mLeft += dx;
        mRight += dx;

        invalidate();
    }

    void View::setMinimumWidth(int width) {
        min_width_ = width;
    }

    void View::setMinimumHeight(int height) {
        min_height_ = height;
    }

    void View::setOnClickListener(OnClickListener *l) {
        click_listener_ = l;
    }


    int View::getId() {
        return id_;
    }

    double View::getX() {
        return mLeft + getTranslateX();
    }

    double View::getY() {
        return mTop + getTranslateY();
    }

    double View::getAlpha() {
        return mAlpha;
    }

    double View::getScaleX() {
        return mScaleX;
    }

    double View::getScaleY() {
        return mScaleY;
    }

    double View::getTranslateX() {
        return mTranslateX;
    }

    double View::getTranslateY() {
        return mTranslateY;
    }

    double View::getPivotX() {
        return mPivotX;
    }

    double View::getPivotY() {
        return mPivotY;
    }

    int View::getScrollX() {
        return mScrollX;
    }

    int View::getScrollY() {
        return mScrollY;
    }

    int View::getLeft() {
        return mLeft;
    }

    int View::getTop() {
        return mTop;
    }

    int View::getRight() {
        return mRight;
    }

    int View::getBottom() {
        return mBottom;
    }

    int View::getWidth() {
        return mRight - mLeft;
    }

    int View::getHeight() {
        return mBottom - mTop;
    }

    int View::getMeasuredWidth() {
        return measured_width_;
    }

    int View::getMeasuredHeight() {
        return measured_height_;
    }

    int View::getMinimumWidth() {
        return min_width_;
    }

    int View::getMinimumHeight() {
        return min_height_;
    }

    float View::getElevation() {
        return elevation_;
    }

    int View::getVisibility() {
        return visibility_;
    }


    int View::getPaddingLeft() {
        return mPaddingLeft;
    }

    int View::getPaddingTop() {
        return mPaddingTop;
    }

    int View::getPaddingRight() {
        return mPaddingRight;
    }

    int View::getPaddingBottom() {
        return mPaddingBottom;
    }


    LayoutParams *View::getLayoutParams() {
        return layout_params_;
    }

    View *View::getParent() {
        return parent_;
    }


    Window *View::getWindow() {
        return window_;
    }


    Drawable *View::getBackground() {
        return bg_drawable_.get();
    }

    Drawable *View::getForeground() {
        return fg_drawable_.get();
    }


    Rect View::getBound() {
        return Rect(mLeft, mTop, mRight - mLeft, mBottom - mTop);
    }

    Rect View::getBoundInWindow() {
        Rect bound = getBound();

        View *parent = parent_;
        while (parent) {
            Rect parentBound = parent->getBound();
            bound.left += parentBound.left;
            bound.top += parentBound.top;
            bound.right += parentBound.left;
            bound.bottom += parentBound.top;

            parent = parent->getParent();
        }

        return bound;
    }

    Rect View::getBoundInScreen() {
        Rect bound = getBoundInWindow();

        POINT pt;
        pt.x = bound.left;
        pt.y = bound.top;

        ::ClientToScreen(window_->getHandle(), &pt);

        int diffX = pt.x - bound.left;
        int diffY = pt.y - bound.top;

        bound.left += diffX;
        bound.top += diffY;
        bound.right += diffX;
        bound.bottom += diffY;

        return bound;
    }

    Rect View::getContentBound() {
        int content_width = mRight - mLeft - mPaddingLeft - mPaddingRight;
        int content_height = mBottom - mTop - mPaddingTop - mPaddingBottom;
        return Rect(
            mLeft + mPaddingLeft, mTop + mPaddingTop,
            content_width, content_height);
    }

    Rect View::getContentBoundInThis() {
        int content_width = mRight - mLeft - mPaddingLeft - mPaddingRight;
        int content_height = mBottom - mTop - mPaddingTop - mPaddingBottom;
        return Rect(
            mPaddingLeft, mPaddingTop,
            content_width, content_height);
    }


    View *View::findViewById(int id) {
        return nullptr;
    }


    bool View::isEnabled() {
        return is_enabled_;
    }

    bool View::isAttachedToWindow() {
        return is_attached_to_window_;
    }

    bool View::isInputEventAtLast() {
        return is_input_event_at_last_;
    }

    bool View::isPressed() {
        return is_pressed_;
    }

    bool View::hasFocus() {
        return has_focus_;
    }

    bool View::isFocusable() {
        return is_focusable_;
    }

    bool View::isLayouted() {
        return is_layouted_;
    }

    bool View::isMouseInThis(InputEvent *e) {
        return (e->getMouseX() >= 0 && e->getMouseX() <= getWidth()
            && e->getMouseY() >= 0 && e->getMouseY() <= getHeight());
    }

    bool View::isGroupMouseInThis(InputEvent *e) {
        return (e->getMouseX() >= mLeft && e->getMouseX() <= mRight - 1
            && e->getMouseY() >= mTop && e->getMouseY() <= mBottom - 1);
    }

    bool View::isReceiveOutsideInputEvent() {
        return is_receive_outside_input_event_;
    }

    bool View::canConsumeMouseEvent() {
        return can_consume_mouse_event_;
    }


    void View::scrollTo(int x, int y) {
        if (mScrollX != x || mScrollY != y) {
            int oldScrollX = mScrollX;
            int oldScrollY = mScrollY;

            mScrollX = x;
            mScrollY = y;

            invalidate();
            onScrollChanged(x, y, oldScrollX, oldScrollY);
        }
    }

    void View::scrollBy(int dx, int dy) {
        scrollTo(mScrollX + dx, mScrollY + dy);
    }

    void View::performClick() {
        click_listener_->onClick(this);
    }


    void View::draw(Canvas *canvas) {
        // 应用动画变量
        canvas->save();
        canvas->setOpacity(mAlpha*canvas->getOpacity());
        canvas->scale(mScaleX, mScaleY, mLeft + mPivotX, mTop + mPivotY);
        canvas->translate(
            static_cast<float>(mTranslateX),
            static_cast<float>(mTranslateY));

        // 将背景绘制到 bgBitmap 上
        ComPtr<ID2D1Bitmap> bgBitmap;
        if (bg_drawable_ != nullptr
            && bg_drawable_->getOpacity() != 0.f)
        {
            window_->getRenderer()->drawOnBitmap(
                mRight - mLeft, mBottom - mTop, &bgBitmap,
                [this, canvas](ComPtr<ID2D1RenderTarget> rt)
            {
                Canvas bgCanvas(rt);
                bgCanvas.setOpacity(canvas->getOpacity());
                drawBackground(&bgCanvas);
            });
        }

        bool hasBg = (bgBitmap != nullptr);
        bool hasShadow = (hasBg && (elevation_ > 0.f));

        // 若有，使用 layer 应用 reveal 动画
        // 若某一 view 正在进行 reveal 动画，则其 child 无法应用 reveal 动画
        if (mHasReveal) {
            //圆形 reveal 动画。
            if (mRevealType == ViewAnimator::REVEAL_CIRCULE) {
                ComPtr<ID2D1EllipseGeometry> circleGeo;
                Application::getGraphicDeviceManager()->getD2DFactory()->CreateEllipseGeometry(
                    D2D1::Ellipse(
                        D2D1::Point2F(mRevealCenterX, mRevealCenterY),
                        mRevealRadius, mRevealRadius), &circleGeo);

                // 在 pushLayer 之前绘制阴影
                // TODO: 修改离屏缓冲机制
                if (hasShadow) {
                    ComPtr<ID2D1BitmapBrush> bmp_brush;
                    canvas->getRT()->CreateBitmapBrush(bgBitmap.get(), &bmp_brush);

                    ComPtr<ID2D1Bitmap> bgRevealedBitmap;
                    window_->getRenderer()->drawOnBitmap(
                        mRight - mLeft, mBottom - mTop, &bgRevealedBitmap,
                        [this, circleGeo, bmp_brush](ComPtr<ID2D1RenderTarget> rt)
                    {
                        rt->FillGeometry(circleGeo.get(), bmp_brush.get());
                    });

                    window_->getRenderer()->drawShadow(elevation_, canvas->getOpacity(), bgRevealedBitmap.get());
                    canvas->drawBitmap(bgRevealedBitmap.get());
                }

                canvas->pushLayer(circleGeo.get());

                if (hasBg && !hasShadow) {
                    canvas->drawBitmap(bgBitmap.get());
                }
            }
            // 矩形 reveal 动画
            else if (mRevealType == ViewAnimator::REVEAL_RECT) {
                ComPtr<ID2D1RectangleGeometry> rectGeo;
                Application::getGraphicDeviceManager()->getD2DFactory()->CreateRectangleGeometry(
                    D2D1::RectF(
                        mRevealCenterX - mRevealWidthRadius,
                        mRevealCenterY - mRevealHeightRadius,
                        mRevealCenterX + mRevealWidthRadius,
                        mRevealCenterY + mRevealHeightRadius), &rectGeo);

                // 在 pushLayer 之前绘制阴影
                if (hasShadow) {
                    ComPtr<ID2D1BitmapBrush> bmp_brush;
                    canvas->getRT()->CreateBitmapBrush(bgBitmap.get(), &bmp_brush);

                    ComPtr<ID2D1Bitmap> bgRevealedBitmap;
                    window_->getRenderer()->drawOnBitmap(
                        mRight - mLeft, mBottom - mTop, &bgRevealedBitmap,
                        [this, rectGeo, bmp_brush](ComPtr<ID2D1RenderTarget> rt)
                    {
                        rt->FillGeometry(rectGeo.get(), bmp_brush.get());
                    });

                    window_->getRenderer()->drawShadow(elevation_, canvas->getOpacity(), bgRevealedBitmap.get());
                    canvas->drawBitmap(bgRevealedBitmap.get());
                }

                canvas->pushLayer(rectGeo.get());

                if (hasBg && !hasShadow) {
                    canvas->drawBitmap(bgBitmap.get());
                }
            }
        }
        // 没有 reveal 动画，直接绘制背景和阴影
        else {
            if (hasBg) {
                if (hasShadow) {
                    window_->getRenderer()->drawShadow(
                        elevation_, canvas->getOpacity(), bgBitmap.get());
                }
                canvas->drawBitmap(bgBitmap.get());
            }
        }

        // 应用 padding
        canvas->save();
        canvas->translate(mPaddingLeft, mPaddingTop);

        // 裁剪
        canvas->pushClip(D2D1::RectF(0, 0,
            measured_width_ - mPaddingLeft - mPaddingRight,
            measured_height_ - mPaddingTop - mPaddingBottom));
        canvas->translate(-mScrollX, -mScrollY);

        // 绘制自身
        onDraw(canvas);
        // 绘制孩子
        dispatchDraw(canvas);

        canvas->popClip();
        canvas->restore();

        // 绘制前景
        drawForeground(canvas);

        if (mHasReveal)
            canvas->popLayer();

        canvas->restore();
    }

    void View::drawBackgroundWithShadow(Canvas *canvas) {
        if (bg_drawable_ != nullptr
            && bg_drawable_->getOpacity() != 0.f) {
            if (elevation_ == 0.f) {
                drawBackground(canvas);
            }
            else
            {
                bg_drawable_->setBound(
                    0, 0, mRight - mLeft, mBottom - mTop);
                D2D1_RECT_F bkRect = bg_drawable_->getBound();

                /*window_->getRenderer()->drawWithShadow(
                    elevation_, bkRect.right - bkRect.left, bkRect.bottom - bkRect.top,
                    [this, canvas](ComPtr<ID2D1RenderTarget> rt)
                {
                    Canvas bkCanvas(rt);
                    bkCanvas.setOpacity(canvas->getOpacity());
                    drawBackground(&bkCanvas);
                });*/
            }
        }
    }

    void View::drawBackground(Canvas *canvas) {
        if (bg_drawable_ != nullptr
            && bg_drawable_->getOpacity() != 0.f) {
            bg_drawable_->setBound(0, 0, mRight - mLeft, mBottom - mTop);
            bg_drawable_->draw(canvas);
        }
    }

    void View::drawForeground(Canvas *canvas) {
        if (fg_drawable_ != nullptr
            && fg_drawable_->getOpacity() != 0.f) {
            fg_drawable_->setBound(0.f, 0.f, mRight - mLeft, mBottom - mTop);
            fg_drawable_->draw(canvas);
        }
    }


    void View::measure(int width, int height, int widthMode, int heightMode) {
        onMeasure(width, height, widthMode, heightMode);
    }


    void View::layout(int left, int top, int right, int bottom) {
        bool sizeChanged = false;

        int width = right - left;
        int height = bottom - top;
        int oldWidth = mRight - mLeft;
        int oldHeight = mBottom - mTop;

        bool changed =
            left != mLeft
            || top != mTop
            || right != mRight
            || bottom != mBottom;
        if (changed) {
            sizeChanged =
                oldWidth != width
                || oldHeight != height;

            mLeft = left;
            mTop = top;
            mRight = right;
            mBottom = bottom;

            if (sizeChanged) {
                onSizeChanged(width, height, oldWidth, oldHeight);
            }
        }

        onLayout(
            changed, sizeChanged,
            left, top, right, bottom);

        is_layouted_ = true;
    }


    void View::invalidate() {
        invalidate(mLeft, mTop, mRight, mBottom);
    }

    void View::invalidate(Rect rect) {
        invalidate(rect.left, rect.top, rect.right, rect.bottom);
    }

    void View::invalidate(int left, int top, int right, int bottom) {
        //TODO:应只刷新脏区域。
        //this->getWindow()->performRefresh(left, top, right, bottom);

        //直接绘制。(实时)
        //this->getWindow()->performRefresh();

        //加入消息队列，等待下一帧。(非实时)
        getWindow()->invalidate();
    }

    void View::requestLayout() {
        //直接布局。(实时)
        //this->getWindow()->performLayout();

        //加入消息队列，等待下一帧。(非实时)
        getWindow()->requestLayout();
    }

    void View::requestFocus() {
        if (!is_focusable_ || has_focus_) {
            return;
        }

        //先取消其他widget的焦点。
        View *prevHolder = window_->getKeyboardHolder();
        if (prevHolder != nullptr) {
            prevHolder->discardFocus();
        }

        //获取焦点。
        window_->captureKeyboard(this);

        has_focus_ = true;
        onFocusChanged(true);
    }

    void View::discardFocus() {
        //舍弃焦点(如果有的话)。
        if (has_focus_) {
            has_focus_ = false;
            window_->releaseKeyboard();
            onFocusChanged(false);
        }

        dispatchDiscardFocus();
    }

    void View::discardPendingOperations() {
        window_->getCycler()->removeCallbacks(click_performer_);
        dispatchDiscardPendingOperations();
    }


    void View::dispatchDraw(Canvas *canvas) {
    }

    void View::dispatchDiscardFocus() {
    }

    void View::dispatchDiscardPendingOperations() {
    }


    bool View::dispatchInputEvent(InputEvent *e) {
        e->setMouseX(e->getMouseX() - mLeft);
        e->setMouseY(e->getMouseY() - mTop);

        return onInputEvent(e);
    }

    void View::dispatchWindowFocusChanged(bool windowFocus) {
        onWindowFocusChanged(windowFocus);
    }

    void View::dispatchWindowDpiChanged(int dpi_x, int dpi_y) {
        onWindowDpiChanged(dpi_x, dpi_y);
    }


    void View::onAttachedToWindow() {
        is_attached_to_window_ = true;

        if (input_connection_ != nullptr) {
            input_connection_->pushEditor();
        }
    }

    void View::onDetachedFromWindow() {
        is_attached_to_window_ = false;

        if (input_connection_ != nullptr) {
            input_connection_->popEditor();
        }

        if (animator_ != nullptr) {
            animator_->cancel();
        }
    }


    void View::onDraw(Canvas *canvas) {
    }

    bool View::onInputEvent(InputEvent *e) {
        bool shouldRefresh = false;

        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (can_consume_mouse_event_) {
                window_->captureMouse(this);
            }
            if (is_focusable_) {
                requestFocus();
            }
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                setPressed(true);
                if (fg_drawable_) {
                    fg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    shouldRefresh = fg_drawable_->setState(Drawable::STATE_PRESSED);
                }
                if (bg_drawable_) {
                    bg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    shouldRefresh = bg_drawable_->setState(Drawable::STATE_PRESSED);
                }
            }
            if (shouldRefresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_MOVE:
            if (!isPressed()) {
                if (fg_drawable_) {
                    //fg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    shouldRefresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                }
                if (bg_drawable_) {
                    //bg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    shouldRefresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                }
            }
            if (shouldRefresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_SCROLL_ENTER:
            if (fg_drawable_) {
                shouldRefresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
            }
            if (bg_drawable_) {
                shouldRefresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
            }
            if (shouldRefresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_LEAVE_OBJ:
            if (fg_drawable_) {
                shouldRefresh = fg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (bg_drawable_) {
                shouldRefresh = bg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (shouldRefresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_UP:
            if (can_consume_mouse_event_) {
                window_->releaseMouse();
            }
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                bool pressed = false;
                if (isPressed()) {
                    pressed = true;
                    setPressed(false);
                }

                if (isMouseInThis(e)) {
                    if (fg_drawable_) {
                        fg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                        shouldRefresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                    }
                    if (bg_drawable_) {
                        bg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                        shouldRefresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                    }

                    if (pressed && click_listener_) {
                        window_->getCycler()->removeCallbacks(click_performer_);
                        window_->getCycler()->post(click_performer_);
                    }
                }
                else {
                    if (fg_drawable_) {
                        shouldRefresh = fg_drawable_->setState(Drawable::STATE_NONE);
                    }
                    if (bg_drawable_) {
                        shouldRefresh = bg_drawable_->setState(Drawable::STATE_NONE);
                    }
                }
            }
            if (shouldRefresh)
                invalidate();
            return can_consume_mouse_event_;

        case InputEvent::EV_CANCEL:
            if (can_consume_mouse_event_)
                window_->releaseMouse();
            setPressed(false);
            if (fg_drawable_)
                shouldRefresh = fg_drawable_->setState(Drawable::STATE_NONE);
            if (bg_drawable_)
                shouldRefresh = bg_drawable_->setState(Drawable::STATE_NONE);
            if (shouldRefresh)
                invalidate();
            return can_consume_mouse_event_;
        }

        return false;
    }

    void View::onMeasure(int width, int height, int widthMode, int heightMode) {
        setMeasuredDimension(width > min_width_ ? width : min_width_,
            height > min_height_ ? height : min_height_);
    }

    void View::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom) {
    }


    bool View::onCheckIsTextEditor() {
        return false;
    }

    InputConnection *View::onCreateInputConnection() {
        return nullptr;
    }


    void View::onSizeChanged(int width, int height, int oldWidth, int oldHeight) {
    }

    void View::onVisibilityChanged(int visibility) {
    }

    void View::onFocusChanged(bool getFocus) {
        if (getFocus) {
            if (this->onCheckIsTextEditor()) {
                if (input_connection_ == nullptr)
                    input_connection_ = this->onCreateInputConnection();

                if (input_connection_ != nullptr) {
                    TsfManager *tsfMgr = Application::getTsfManager();
                    input_connection_->initialization(tsfMgr);
                    input_connection_->pushEditor();
                    input_connection_->mount(tsfMgr);
                }
            }
        }
        else {
            if (input_connection_ != nullptr) {
                TsfManager *tsfMgr = Application::getTsfManager();
                input_connection_->unmount(tsfMgr);
                input_connection_->terminateComposition();
            }
        }

        bool shouldRefresh = false;
        if (bg_drawable_) {
            shouldRefresh = bg_drawable_->setParentFocus(getFocus);
        }
        if (fg_drawable_) {
            shouldRefresh = fg_drawable_->setParentFocus(getFocus);
        }

        if (shouldRefresh) {
            invalidate();
        }
    }

    void View::onWindowFocusChanged(bool windowFocus) {
        if (!hasFocus()) {
            return;
        }

        if (windowFocus) {
            if (onCheckIsTextEditor()) {
                if (input_connection_ == nullptr) {
                    input_connection_ = this->onCreateInputConnection();
                }

                if (input_connection_ != nullptr) {
                    TsfManager *tsfMgr = Application::getTsfManager();
                    input_connection_->initialization(tsfMgr);
                    input_connection_->mount(tsfMgr);
                }
            }
        }
        else {
            if (input_connection_ != nullptr) {
                TsfManager *tsfMgr = Application::getTsfManager();
                input_connection_->unmount(tsfMgr);
                input_connection_->terminateComposition();
            }
        }
    }

    void View::onWindowDpiChanged(int dpi_x, int dpi_y) {
    }

    void View::onScrollChanged(int scrollX, int scrollY, int oldScrollX, int oldScrollY) {
    }

}