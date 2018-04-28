#include "view.h"

#include "ukive/log.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/drawable/drawable.h"
#include "ukive/animation/view_animator.h"
#include "ukive/text/input_connection.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/application.h"
#include "ukive/message/cycler.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/graphics/direct3d/effects/shadow_effect.h"


namespace ukive {

    View::View(Window* w)
        :window_(w),
        id_(Application::getViewID()),
        flags_(0),
        min_width_(0),
        min_height_(0),
        measured_width_(0),
        measured_height_(0),
        old_pm_width_(0),
        old_pm_height_(0),
        old_pm_width_mode_(UNKNOWN),
        old_pm_height_mode_(UNKNOWN),
        scroll_x_(0),
        scroll_y_(0),
        elevation_(0.f),
        visibility_(VISIBLE),
        has_focus_(false),
        is_enabled_(true),
        is_pressed_(false),
        is_focusable_(false),
        is_attached_to_window_(false),
        is_input_event_at_last_(false),
        is_receive_outside_input_event_(false),
        can_consume_mouse_event_(true),
        parent_(nullptr),
        input_connection_(nullptr),
        click_listener_(nullptr),
        click_performer_(new ClickPerformer(this)),
        mAlpha(1.0),
        mScaleX(1.0),
        mScaleY(1.0),
        mTranslateX(0.0),
        mTranslateY(0.0),
        mPivotX(0.0),
        mPivotY(0.0),
        mRevealType(ViewAnimator::REVEAL_CIRCULE),
        mHasReveal(false),
        mRevealRadius(0.0),
        mRevealCenterX(0.0),
        mRevealCenterY(0.0),
        mRevealWidthRadius(0.0),
        mRevealHeightRadius(0.0) {}


    View::~View() {
        delete click_performer_;

        if (input_connection_) {
            input_connection_->popEditor();
            delete input_connection_;
        }
    }


    ViewAnimator* View::animate() {
        if (!animator_) {
            animator_ = std::make_unique<ViewAnimator>(this);
        }

        return animator_.get();
    }

    void View::setId(int id) {
        id_ = id;
    }

    void View::setX(double x) {
        setTranslateX(x - bounds_.left);
    }

    void View::setY(double y) {
        setTranslateY(y - bounds_.top);
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
        if (scroll_x_ != x) {
            scroll_x_ = x;
            invalidate();
        }
    }

    void View::setScrollY(int y) {
        if (scroll_y_ != y) {
            scroll_y_ = y;
            invalidate();
        }
    }

    void View::setVisibility(int visibility) {
        if (visibility == visibility_) {
            return;
        }

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
        if (enable == is_enabled_) {
            return;
        }

        is_enabled_ = enable;

        if (!is_enabled_) {
            discardFocus();
            discardPendingOperations();
        }

        invalidate();
    }

    void View::setBackground(Drawable* drawable) {
        bg_drawable_.reset(drawable);
        invalidate();
    }

    void View::setForeground(Drawable* drawable) {
        fg_drawable_.reset(drawable);
        invalidate();
    }

    void View::setPadding(int left, int top, int right, int bottom) {
        Rect new_padding(left, top, right - left, bottom - top);
        if (padding_ == new_padding) {
            return;
        }

        padding_ = new_padding;

        requestLayout();
        invalidate();
    }

    void View::setLayoutParams(LayoutParams* params) {
        if (!params) {
            DLOG(Log::WARNING) << "null param";
            return;
        }

        layout_params_.reset(params);

        requestLayout();
        invalidate();
    }

    void View::setIsInputEventAtLast(bool isInput) {
        is_input_event_at_last_ = isInput;
    }

    void View::setPressed(bool pressed) {
        if (is_pressed_ == pressed) {
            return;
        }

        is_pressed_ = pressed;

        invalidate();
    }

    void View::setCurrentCursor(Cursor cursor) {
        window_->setCurrentCursor(cursor);
    }

    void View::setFocusable(bool focusable) {
        if (is_focusable_ == focusable) {
            return;
        }

        is_focusable_ = focusable;

        if (!focusable) {
            discardFocus();
        }
    }

    void View::setElevation(float elevation) {
        if (elevation == elevation_) {
            return;
        }

        if (!shadow_effect_) {
            shadow_effect_ = std::make_unique<ShadowEffect>();
        }

        elevation_ = elevation;
        shadow_effect_->setRadius(elevation_ * 2);

        requestLayout();
        invalidate();
    }

    void View::setReceiveOutsideInputEvent(bool receive) {
        is_receive_outside_input_event_ = receive;
    }

    void View::setCanConsumeMouseEvent(bool enable) {
        can_consume_mouse_event_ = enable;

        if (!enable) {
            window_->releaseMouse();
        }
    }

    void View::setParent(View* parent) {
        parent_ = parent;
    }

    void View::setMeasuredDimension(int width, int height) {
        measured_width_ = width;
        measured_height_ = height;

        flags_ |= Flags::MEASURED_DIMENSION_SET;
    }

    void View::offsetTopAndBottom(int dy) {
        bounds_.top += dy;
        bounds_.bottom += dy;

        invalidate();
    }

    void View::offsetLeftAndRight(int dx) {
        bounds_.left += dx;
        bounds_.right += dx;

        invalidate();
    }

    void View::setMinimumWidth(int width) {
        min_width_ = width;
    }

    void View::setMinimumHeight(int height) {
        min_height_ = height;
    }

    void View::setOnClickListener(OnClickListener* l) {
        click_listener_ = l;
    }


    int View::getId() const {
        return id_;
    }

    double View::getX() const {
        return bounds_.left + getTranslateX();
    }

    double View::getY() const {
        return bounds_.top + getTranslateY();
    }

    double View::getAlpha() const {
        return mAlpha;
    }

    double View::getScaleX() const {
        return mScaleX;
    }

    double View::getScaleY() const {
        return mScaleY;
    }

    double View::getTranslateX() const {
        return mTranslateX;
    }

    double View::getTranslateY() const {
        return mTranslateY;
    }

    double View::getPivotX() const {
        return mPivotX;
    }

    double View::getPivotY() const {
        return mPivotY;
    }

    int View::getScrollX() const {
        return scroll_x_;
    }

    int View::getScrollY() const {
        return scroll_y_;
    }

    int View::getLeft() const {
        return bounds_.left;
    }

    int View::getTop() const {
        return bounds_.top;
    }

    int View::getRight() const {
        return bounds_.right;
    }

    int View::getBottom() const {
        return bounds_.bottom;
    }

    int View::getWidth() const {
        return bounds_.width();
    }

    int View::getHeight() const {
        return bounds_.height();
    }

    int View::getMeasuredWidth() const {
        return measured_width_;
    }

    int View::getMeasuredHeight() const {
        return measured_height_;
    }

    int View::getMinimumWidth() const {
        return min_width_;
    }

    int View::getMinimumHeight() const {
        return min_height_;
    }

    float View::getElevation() const {
        return elevation_;
    }

    int View::getVisibility() const {
        return visibility_;
    }


    int View::getPaddingLeft() const {
        return padding_.left;
    }

    int View::getPaddingTop() const {
        return padding_.top;
    }

    int View::getPaddingRight() const {
        return padding_.right;
    }

    int View::getPaddingBottom() const {
        return padding_.bottom;
    }


    LayoutParams* View::getLayoutParams() const {
        return layout_params_.get();
    }

    View* View::getParent() const {
        return parent_;
    }


    Window* View::getWindow() const {
        return window_;
    }


    Drawable* View::getBackground() const {
        return bg_drawable_.get();
    }

    Drawable* View::getForeground() const {
        return fg_drawable_.get();
    }


    Rect View::getBounds() const {
        return bounds_;
    }

    Rect View::getBoundsInWindow() const {
        Rect bound = getBounds();

        View* parent = parent_;
        while (parent) {
            Rect parentBound = parent->getBounds();
            bound.left += parentBound.left;
            bound.top += parentBound.top;
            bound.right += parentBound.left;
            bound.bottom += parentBound.top;

            parent = parent->getParent();
        }

        return bound;
    }

    Rect View::getBoundsInScreen() const {
        Rect bound = getBoundsInWindow();

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

    Rect View::getContentBounds() const {
        Rect content_bounds = bounds_;
        content_bounds.insets(padding_);
        return content_bounds;
    }

    Rect View::getContentBoundsInThis() const {
        int content_width = bounds_.width() - padding_.width();
        int content_height = bounds_.height() - padding_.height();
        return Rect(
            padding_.left, padding_.top,
            content_width, content_height);
    }


    View* View::findViewById(int id) {
        return nullptr;
    }


    bool View::isEnabled() const {
        return is_enabled_;
    }

    bool View::isAttachedToWindow() const {
        return is_attached_to_window_;
    }

    bool View::isInputEventAtLast() const {
        return is_input_event_at_last_;
    }

    bool View::isPressed() const {
        return is_pressed_;
    }

    bool View::hasFocus() const {
        return has_focus_;
    }

    bool View::isFocusable() const {
        return is_focusable_;
    }

    bool View::isLayouted() const {
        return flags_ & Flags::BOUNDS_SET;
    }

    bool View::isLocalMouseInThis(InputEvent* e) const {
        return (e->getMouseX() >= 0 && e->getMouseX() <= getWidth()
            && e->getMouseY() >= 0 && e->getMouseY() <= getHeight());
    }

    bool View::isParentMouseInThis(InputEvent* e) const {
        return bounds_.hit(e->getMouseX(), e->getMouseY());
    }

    bool View::isReceiveOutsideInputEvent() const {
        return is_receive_outside_input_event_;
    }

    bool View::canConsumeMouseEvent() const {
        return can_consume_mouse_event_;
    }


    void View::scrollTo(int x, int y) {
        if (scroll_x_ != x || scroll_y_ != y) {
            int oldScrollX = scroll_x_;
            int oldScrollY = scroll_y_;

            scroll_x_ = x;
            scroll_y_ = y;

            invalidate();
            onScrollChanged(x, y, oldScrollX, oldScrollY);
        }
    }

    void View::scrollBy(int dx, int dy) {
        scrollTo(scroll_x_ + dx, scroll_y_ + dy);
    }

    void View::performClick() {
        click_listener_->onClick(this);
    }


    void View::draw(Canvas* canvas) {
        // 应用动画变量
        canvas->save();
        canvas->setOpacity(mAlpha*canvas->getOpacity());
        canvas->scale(mScaleX, mScaleY, bounds_.left + mPivotX, bounds_.top + mPivotY);
        canvas->translate(mTranslateX, mTranslateY);

        bool hasBg = needDrawBackground();
        bool hasShadow = (hasBg && (elevation_ > 0.f) && shadow_effect_);

        std::shared_ptr<Bitmap> bg_bmp;
        ComPtr<ID3D11Texture2D> bg_texture;
        if (hasShadow) {
            // 将背景绘制到 bg_bmp 上
            Canvas offscreen(getWidth(), getHeight());
            offscreen.beginDraw();
            offscreen.clear();
            offscreen.setOpacity(canvas->getOpacity());
            drawBackground(&offscreen);
            offscreen.endDraw();
            bg_bmp = offscreen.extractBitmap();
            bg_texture = offscreen.getTexture();
        }

        // 若有，使用 layer 应用 reveal 动画
        // 若某一 view 正在进行 reveal 动画，则其 child 无法应用 reveal 动画
        if (mHasReveal) {
            // 圆形 reveal 动画。
            if (mRevealType == ViewAnimator::REVEAL_CIRCULE) {
                ComPtr<ID2D1EllipseGeometry> circleGeo;
                Application::getGraphicDeviceManager()->getD2DFactory()->CreateEllipseGeometry(
                    D2D1::Ellipse(
                        D2D1::Point2F(mRevealCenterX, mRevealCenterY),
                        mRevealRadius, mRevealRadius), &circleGeo);

                // 在 pushLayer 之前绘制阴影
                if (hasShadow) {
                    ComPtr<ID2D1BitmapBrush> bmp_brush;
                    canvas->getRT()->CreateBitmapBrush(bg_bmp->getNative().get(), &bmp_brush);

                    Canvas offscreen(getWidth(), getHeight());
                    offscreen.beginDraw();
                    offscreen.clear();
                    offscreen.fillGeometry(circleGeo.get(), bmp_brush.get());
                    offscreen.endDraw();
                    auto revealed_bg_bmp = offscreen.extractBitmap();
                    auto revealed_bg_texture = offscreen.getTexture();

                    shadow_effect_->setContent(revealed_bg_texture.get());
                    shadow_effect_->draw(canvas);

                    canvas->drawBitmap(revealed_bg_bmp.get());
                }

                canvas->pushLayer(circleGeo.get());

                if (hasBg && !hasShadow) {
                    drawBackground(canvas);
                }
            } else if (mRevealType == ViewAnimator::REVEAL_RECT) {
                // 矩形 reveal 动画
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
                    canvas->getRT()->CreateBitmapBrush(bg_bmp->getNative().get(), &bmp_brush);

                    Canvas offscreen(getWidth(), getHeight());
                    offscreen.beginDraw();
                    offscreen.clear();
                    offscreen.fillGeometry(rectGeo.get(), bmp_brush.get());
                    offscreen.endDraw();
                    auto revealed_bg_bmp = offscreen.extractBitmap();
                    auto revealed_bg_texture = offscreen.getTexture();

                    shadow_effect_->setContent(revealed_bg_texture.get());
                    shadow_effect_->draw(canvas);

                    canvas->drawBitmap(revealed_bg_bmp.get());
                }

                canvas->pushLayer(rectGeo.get());

                if (hasBg && !hasShadow) {
                    drawBackground(canvas);
                }
            }
        } else {
            // 没有 reveal 动画，直接绘制背景和阴影
            if (hasBg) {
                if (hasShadow) {
                    shadow_effect_->setContent(bg_texture.get());
                    shadow_effect_->draw(canvas);

                    canvas->drawBitmap(bg_bmp.get());
                } else {
                    drawBackground(canvas);
                }
            }
        }

        // 应用 padding
        canvas->save();
        canvas->translate(padding_.left, padding_.top);

        // 裁剪
        canvas->pushClip(RectF(0, 0,
            measured_width_ - padding_.width(),
            measured_height_ - padding_.height()));
        canvas->translate(-scroll_x_, -scroll_y_);

        // 绘制自身
        onDraw(canvas);
        // 绘制孩子
        dispatchDraw(canvas);

        canvas->popClip();
        canvas->restore();

        // 绘制前景
        drawForeground(canvas);

        if (mHasReveal) {
            canvas->popLayer();
        }

        canvas->restore();
    }

    bool View::needDrawBackground() {
        return (bg_drawable_ != nullptr
            && bg_drawable_->getOpacity() > 0.f);
    }

    bool View::needDrawForeground() {
        return (fg_drawable_ != nullptr
            && fg_drawable_->getOpacity() > 0.f);
    }

    void View::drawBackground(Canvas* canvas) {
        if (needDrawBackground()) {
            bg_drawable_->setBounds(0, 0, bounds_.width(), bounds_.height());
            bg_drawable_->draw(canvas);
        }
    }

    void View::drawForeground(Canvas* canvas) {
        if (needDrawForeground()) {
            fg_drawable_->setBounds(0, 0, bounds_.width(), bounds_.height());
            fg_drawable_->draw(canvas);
        }
    }

    void View::measure(int width, int height, int width_mode, int height_mode) {
        if (flags_ & Flags::MEASURED_DIMENSION_SET) {
            bool is_force_layout = (flags_ & Flags::FORCE_LAYOUT);
            bool is_exactly_mode = (width_mode == EXACTLY && height_mode == EXACTLY);
            bool is_spec_not_change =
                (width == old_pm_width_ && height == old_pm_height_
                    && width_mode == old_pm_width_mode_ && height_mode == old_pm_height_mode_);

            if (!is_force_layout && is_exactly_mode && is_spec_not_change) {
                return;
            }
        }

        flags_ &= ~Flags::MEASURED_DIMENSION_SET;

        onMeasure(width, height, width_mode, height_mode);

        if (!(flags_ & Flags::MEASURED_DIMENSION_SET)) {
            LOG(Log::FATAL) << "You must invoke setMeasuredDimension() in onMeasure()!";
        }

        flags_ |= Flags::NEED_LAYOUT;

        old_pm_width_ = width;
        old_pm_height_ = height;
        old_pm_width_mode_ = width_mode;
        old_pm_height_mode_ = height_mode;
    }

    void View::layout(int left, int top, int right, int bottom) {
        bool size_changed = false;

        Rect new_bounds(left, top, right - left, bottom - top);

        bool changed = bounds_ != new_bounds;
        if (changed) {
            int width = new_bounds.width();
            int height = new_bounds.height();
            int old_width = bounds_.width();
            int old_height = bounds_.height();

            size_changed = old_width != width || old_height != height;
            bounds_ = new_bounds;
            if (size_changed) {
                onSizeChanged(width, height, old_width, old_height);
            }

            invalidate();
        }

        if (changed || (flags_ & Flags::NEED_LAYOUT)) {
            onLayout(changed, size_changed,
                left, top, right, bottom);
        }

        flags_ |= Flags::BOUNDS_SET;
        flags_ &= ~Flags::FORCE_LAYOUT;
        flags_ &= ~Flags::NEED_LAYOUT;
    }

    void View::invalidate() {
        invalidate(bounds_);
    }

    void View::invalidate(const Rect& rect) {
        invalidate(rect.left, rect.top, rect.right, rect.bottom);
    }

    void View::invalidate(int left, int top, int right, int bottom) {
        flags_ |= Flags::INVALIDATED;

        // TODO: we only need to refresh dirty region.
        // add to queue
        getWindow()->invalidate();
    }

    void View::requestLayout() {
        flags_ |= Flags::FORCE_LAYOUT;

        if (parent_) {
            parent_->requestLayout();
        }
    }

    void View::requestFocus() {
        if (!is_focusable_ || has_focus_) {
            return;
        }

        // 先取消其他 view 的焦点。
        View* prev_holder = window_->getKeyboardHolder();
        if (prev_holder) {
            prev_holder->discardFocus();
        }

        // 获取焦点。
        window_->captureKeyboard(this);

        has_focus_ = true;
        onFocusChanged(true);
    }

    void View::discardFocus() {
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

    void View::dispatchDraw(Canvas* canvas) {}

    void View::dispatchDiscardFocus() {}

    void View::dispatchDiscardPendingOperations() {}

    bool View::dispatchInputEvent(InputEvent* e) {
        e->setMouseX(e->getMouseX() - bounds_.left);
        e->setMouseY(e->getMouseY() - bounds_.top);

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

        if (input_connection_) {
            input_connection_->pushEditor();
        }
    }

    void View::onDetachedFromWindow() {
        is_attached_to_window_ = false;

        if (input_connection_) {
            input_connection_->popEditor();
        }

        if (animator_) {
            animator_->cancel();
        }
    }

    void View::onDraw(Canvas* canvas) {}

    bool View::onInputEvent(InputEvent* e) {
        bool should_refresh = false;

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
                    should_refresh = fg_drawable_->setState(Drawable::STATE_PRESSED);
                }
                if (bg_drawable_) {
                    bg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    should_refresh = bg_drawable_->setState(Drawable::STATE_PRESSED);
                }
            }
            if (should_refresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_MOVE:
            if (!isPressed()) {
                if (fg_drawable_) {
                    //fg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    should_refresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                }
                if (bg_drawable_) {
                    //bg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                    should_refresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                }
            }
            if (should_refresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_SCROLL_ENTER:
            if (fg_drawable_) {
                should_refresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
            }
            if (bg_drawable_) {
                should_refresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
            }
            if (should_refresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EVM_LEAVE_VIEW:
            if (fg_drawable_) {
                should_refresh = fg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (bg_drawable_) {
                should_refresh = bg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (should_refresh) {
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

                if (isLocalMouseInThis(e)) {
                    if (fg_drawable_) {
                        fg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                        should_refresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                    }
                    if (bg_drawable_) {
                        bg_drawable_->setHotspot(e->getMouseX(), e->getMouseY());
                        should_refresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                    }

                    if (pressed && click_listener_) {
                        window_->getCycler()->removeCallbacks(click_performer_);
                        window_->getCycler()->post(click_performer_);
                    }
                }
                else {
                    if (fg_drawable_) {
                        should_refresh = fg_drawable_->setState(Drawable::STATE_NONE);
                    }
                    if (bg_drawable_) {
                        should_refresh = bg_drawable_->setState(Drawable::STATE_NONE);
                    }
                }
            }
            if (should_refresh) {
                invalidate();
            }
            return can_consume_mouse_event_;

        case InputEvent::EV_CANCEL:
            if (can_consume_mouse_event_) {
                window_->releaseMouse();
            }
            setPressed(false);
            if (fg_drawable_) {
                should_refresh = fg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (bg_drawable_) {
                should_refresh = bg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (should_refresh) {
                invalidate();
            }
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

    InputConnection* View::onCreateInputConnection() {
        return nullptr;
    }

    void View::onSizeChanged(int width, int height, int old_width, int old_height) {
    }

    void View::onVisibilityChanged(int visibility) {
    }

    void View::onFocusChanged(bool get_focus) {
        if (get_focus) {
            if (onCheckIsTextEditor()) {
                if (!input_connection_) {
                    input_connection_ = onCreateInputConnection();
                }

                if (input_connection_) {
                    input_connection_->initialization();
                    input_connection_->pushEditor();
                    input_connection_->mount();
                }
            }
        } else {
            if (input_connection_) {
                input_connection_->unmount();
                input_connection_->terminateComposition();
            }
        }

        bool shouldRefresh = false;
        if (bg_drawable_) {
            shouldRefresh = bg_drawable_->setParentFocus(get_focus);
        }
        if (fg_drawable_) {
            shouldRefresh = fg_drawable_->setParentFocus(get_focus);
        }

        if (shouldRefresh) {
            invalidate();
        }
    }

    void View::onWindowFocusChanged(bool window_focus) {
        if (!hasFocus()) {
            DCHECK(getWindow()->getKeyboardHolder() != this);
            return;
        }

        if (window_focus) {
            if (onCheckIsTextEditor()) {
                if (!input_connection_) {
                    input_connection_ = onCreateInputConnection();
                }

                if (input_connection_) {
                    HRESULT hr = input_connection_->initialization();
                    DCHECK(SUCCEEDED(hr));
                    input_connection_->pushEditor();
                    input_connection_->mount();
                }
            }
        } else {
            if (input_connection_) {
                bool ret = input_connection_->unmount();
                DCHECK(ret);
                ret = input_connection_->terminateComposition();
                DCHECK(ret);
            }
        }
    }

    void View::onWindowDpiChanged(int dpi_x, int dpi_y) {
    }

    void View::onScrollChanged(
        int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) {
    }

}