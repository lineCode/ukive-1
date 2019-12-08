#include "view.h"

#include "utils/log.h"

#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/animation/view_animator.h"
#include "ukive/text/tsf_input_connection.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/window/window.h"
#include "ukive/application.h"
#include "ukive/message/cycler.h"
#include "ukive/graphics/point.h"
#include "ukive/graphics/effects/shadow_effect.h"
#include "ukive/resources/dimension_utils.h"

#include "oigka/layout_constants.h"


namespace ukive {

    View::View(Window* w)
        : View(w, {}) {}

    View::View(Window* w, AttrsRef attrs)
        : cur_ev_(std::make_unique<InputEvent>()),
          id_(Application::getViewID()),
          outline_(OUTLINE_RECT),
          flags_(0),
          scroll_x_(0),
          scroll_y_(0),
          measured_width_(0),
          measured_height_(0),
          old_pm_width_(0),
          old_pm_height_(0),
          old_pm_width_mode_(UNKNOWN),
          old_pm_height_mode_(UNKNOWN),
          min_width_(0),
          min_height_(0),
          visibility_(VISIBLE),
          elevation_(0.f),
          has_focus_(false),
          is_enabled_(true),
          is_attached_to_window_(false),
          is_input_event_at_last_(false),
          is_pressed_(false),
          is_focusable_(false),
          is_receive_outside_input_event_(false),
          is_mouse_down_(false),
          is_touch_down_(false),
          window_(w),
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
          mRevealHeightRadius(0.0),
          parent_(nullptr),
          click_listener_(nullptr),
          click_performer_(new ClickPerformer(this)),
          input_connection_(nullptr)
    {
        auto it = attrs.find(oigka::kAttrViewId);
        if (it != attrs.end()) {
            if (!utl::stringToNumber(it->second, &id_)) {
                LOG(Log::WARNING) << "Cannot convert View id: " << it->second;
            }
        }

        Rect padding;
        padding.left = padding.right = padding.top = padding.bottom
            = resolveAttrDimension(getWindow(), attrs, oigka::kAttrViewPadding, 0);
        padding.left = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrViewPaddingStart, padding.left);
        padding.right = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrViewPaddingEnd, padding.right);
        padding.top = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrViewPaddingTop, padding.top);
        padding.bottom = resolveAttrDimension(
            getWindow(), attrs, oigka::kAttrViewPaddingBottom, padding.bottom);

        setPadding(padding.left, padding.top, padding.right, padding.bottom);

        float elevation = resolveAttrDimension(getWindow(), attrs, oigka::kAttrViewElevation, 0);
        setElevation(elevation);
    }

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
            discardMouseCapture();
            discardTouchCapture();
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
            discardMouseCapture();
            discardTouchCapture();
            discardPendingOperations();
        }

        updateDrawableState();
        invalidate();
    }

    void View::setBackground(Drawable* drawable) {
        bg_drawable_.reset(drawable);
        bg_drawable_->setCallback(this);
        updateBackgroundState();
        invalidate();
    }

    void View::setForeground(Drawable* drawable) {
        fg_drawable_.reset(drawable);
        fg_drawable_->setCallback(this);
        updateForegroundState();
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

        if (layout_params_.get() != params) {
            layout_params_.reset(params);
        }

        requestLayout();
        invalidate();
    }

    void View::setIsInputEventAtLast(bool is_last) {
        is_input_event_at_last_ = is_last;
        if (is_last) {
            window_->setLastInputView(this);
        } else if (!is_last && window_->getLastInputView() == this) {
            window_->setLastInputView(nullptr);
        }
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

    void View::setClickable(bool clickable) {
        is_clickable_ = clickable;
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

    void View::setTouchCapturable(bool capturable) {
        if (is_touch_capturable_ == capturable) {
            return;
        }

        is_touch_capturable_ = capturable;
    }

    void View::setElevation(float elevation) {
        if (elevation == elevation_) {
            return;
        }

        if (!shadow_effect_) {
            shadow_effect_.reset(ShadowEffect::createShadowEffect());
        }

        elevation_ = elevation;
        shadow_effect_->setRadius(elevation_ * 2);

        requestLayout();
        invalidate();
    }

    void View::setReceiveOutsideInputEvent(bool receive) {
        is_receive_outside_input_event_ = receive;
    }

    void View::setParent(View* parent) {
        parent_ = parent;
    }

    void View::setMeasuredSize(int width, int height) {
        measured_width_ = width;
        measured_height_ = height;

        flags_ |= Flags::MEASURED_SIZE_SET;
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

    void View::setOutline(Outline outline) {
        if (outline == outline_) {
            return;
        }

        outline_ = outline;
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

    int View::getOutline() const {
        return outline_;
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

    Drawable* View::getReleasedBackground() {
        return bg_drawable_.release();
    }

    Drawable* View::getReleasedForeground() {
        return fg_drawable_.release();
    }

    Rect View::getBounds() const {
        return bounds_;
    }

    Rect View::getBoundsInWindow() const {
        auto bounds = getBounds();

        auto parent = parent_;
        while (parent) {
            auto p_bounds = parent->getBounds();
            bounds.offset(
                p_bounds.left - parent->getScrollX(),
                p_bounds.top - parent->getScrollY());
            parent = parent->getParent();
        }

        return bounds;
    }

    Rect View::getBoundsInScreen() const {
        auto bound = getBoundsInWindow();

        Point pt;
        pt.x = bound.left;
        pt.y = bound.top;

        getWindow()->convClientToScreen(&pt);

        int dx = pt.x - bound.left;
        int dy = pt.y - bound.top;

        bound.offset(dx, dy);

        return bound;
    }

    Rect View::getContentBounds() const {
        int content_width = bounds_.width() - padding_.width();
        int content_height = bounds_.height() - padding_.height();
        return Rect(
            padding_.left, padding_.top,
            content_width, content_height);
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

    bool View::isClickable() const {
        return is_clickable_;
    }

    bool View::isFocusable() const {
        return is_focusable_;
    }

    bool View::isTouchCapturable() const {
        return is_touch_capturable_;
    }

    bool View::isLayouted() const {
        return flags_ & Flags::BOUNDS_SET;
    }

    bool View::isLocalPointerInThis(InputEvent* e) const {
        switch (outline_) {
        case OUTLINE_OVAL:
        {
            float a = getWidth() / 2.f;
            float b = getHeight() / 2.f;
            return std::pow(e->getX() / a - 1, 2) + std::pow(e->getY() / b - 1, 2) <= 1;
        }

        case OUTLINE_RECT:
        default:
            return (e->getX() >= 0 && e->getX() < getWidth()
                && e->getY() >= 0 && e->getY() < getHeight());
        }
    }

    bool View::isParentPointerInThis(InputEvent* e) const {
        switch (outline_) {
        case OUTLINE_OVAL:
        {
            float a = getWidth() / 2.f;
            float b = getHeight() / 2.f;
            return std::pow((e->getX() - bounds_.left) / a - 1, 2)
                + std::pow((e->getY() - bounds_.top) / b - 1, 2) <= 1;
        }

        case OUTLINE_RECT:
        default:
            return bounds_.hit(e->getX(), e->getY());
        }
    }

    bool View::isReceiveOutsideInputEvent() const {
        return is_receive_outside_input_event_;
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
        if (animator_) {
            animator_->onPreViewDraw();
        }
        onComputeScroll();

        // 应用动画变量
        canvas->save();
        canvas->setOpacity(mAlpha*canvas->getOpacity());
        canvas->scale(mScaleX, mScaleY, bounds_.left + mPivotX, bounds_.top + mPivotY);
        canvas->translate(mTranslateX, mTranslateY);

        bool hasBg = needDrawBackground();
        bool hasShadow = (hasBg && (elevation_ > 0.f) && shadow_effect_);

        std::shared_ptr<Bitmap> bg_bmp;
        std::shared_ptr<Canvas> bg_off;
        if (hasShadow) {
            // 将背景绘制到 bg_bmp 上
            bg_off = std::make_shared<Canvas>(getWidth(), getHeight());
            bg_off->beginDraw();
            bg_off->clear();
            bg_off->setOpacity(canvas->getOpacity());
            drawBackground(bg_off.get());
            bg_off->endDraw();
            bg_bmp = bg_off->extractBitmap();
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
                    Canvas offscreen(getWidth(), getHeight());
                    offscreen.beginDraw();
                    offscreen.clear();
                    offscreen.fillCircle(mRevealCenterX, mRevealCenterY, mRevealRadius, bg_bmp.get());
                    offscreen.endDraw();
                    auto revealed_bg_bmp = offscreen.extractBitmap();

                    shadow_effect_->setContent(reinterpret_cast<OffscreenBuffer*>(offscreen.getBuffer()));
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
                    Canvas offscreen(getWidth(), getHeight());
                    offscreen.beginDraw();
                    offscreen.clear();
                    offscreen.fillGeometry(rectGeo.get(), bg_bmp.get());
                    offscreen.endDraw();
                    auto revealed_bg_bmp = offscreen.extractBitmap();

                    shadow_effect_->setContent(reinterpret_cast<OffscreenBuffer*>(offscreen.getBuffer()));
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
                    shadow_effect_->setContent(reinterpret_cast<OffscreenBuffer*>(bg_off->getBuffer()));
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
        canvas->pushClip(Rect(0, 0,
            measured_width_ - padding_.width(),
            measured_height_ - padding_.height()));
        canvas->translate(-scroll_x_, -scroll_y_);

        // 绘制自身
        onDraw(canvas);
        // 绘制孩子
        dispatchDraw(canvas);

        onDrawOverChildren(canvas);

        canvas->popClip();
        canvas->restore();

        // 绘制前景
        drawForeground(canvas);

        if (mHasReveal) {
            canvas->popLayer();
        }

        canvas->restore();

        if (animator_) {
            animator_->onPostViewDraw();
        }
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

    void View::onDrawableInvalidate(Drawable* d) {
        invalidate();
    }

    bool View::dispatchInputEventToThis(InputEvent* e) {
        bool consumed = false;
        if (e->getEvent() == InputEvent::EV_CANCEL ||
            e->getEvent() == InputEvent::EV_LEAVE_VIEW)
        {
            if (isInputEventAtLast()) {
                processInputEvent(e);
                setIsInputEventAtLast(false);
            }
        } else {
            consumed = processInputEvent(e);
            // 防止对 EVM_SCROLL_ENTER 消息的分发产生影响。
            // 主要影响由该消息转变的 EVM_LEAVE_VIEW 消息的分发
            if (!isInputEventAtLast() || e->getEvent() != InputEvent::EVM_WHEEL) {
                setIsInputEventAtLast(consumed);
            }
        }
        return consumed;
    }

    void View::updateDrawableState() {
        updateBackgroundState();
        updateForegroundState();
    }

    void View::updateBackgroundState() {
        bool need_redraw = false;
        if (isEnabled()) {
            if (bg_drawable_) {
                need_redraw = bg_drawable_->setState(Drawable::STATE_NONE);
            }
        } else {
            if (bg_drawable_) {
                need_redraw = bg_drawable_->setState(Drawable::STATE_DISABLED);
            }
        }

        if (need_redraw) {
            invalidate();
        }
    }

    void View::updateForegroundState() {
        bool need_redraw = false;
        if (isEnabled()) {
            if (fg_drawable_) {
                need_redraw = fg_drawable_->setState(Drawable::STATE_NONE);
            }
        } else {
            if (fg_drawable_) {
                need_redraw = fg_drawable_->setState(Drawable::STATE_DISABLED);
            }
        }

        if (need_redraw) {
            invalidate();
        }
    }

    bool View::processInputEvent(InputEvent* e) {
        if (is_mouse_down_ && e->isTouchEvent()) {
            return true;
        }
        if (is_touch_down_ && e->isMouseEvent()) {
            return true;
        }

        // 对于鼠标事件，仅 UP 事件需要过滤
        if (e->getEvent() == InputEvent::EVM_UP && !is_mouse_down_) {
            return true;
        }

        // 对于触摸事件，所有事件都需要过滤
        if (e->getEvent() != InputEvent::EVT_DOWN) {
            if (e->isTouchEvent() && !is_touch_down_) {
                return false;
            }
        }

        if ((e->getEvent() == InputEvent::EVT_MOVE ||
            e->getEvent() == InputEvent::EVT_UP) &&
            !cur_ev_->hasTouchEvent(e))
        {
            return true;
        }

        InputEvent* ev;
        if (e->isTouchEvent()) {
            cur_ev_->combineTouchEvent(e);
            ev = cur_ev_.get();
        } else {
            ev = e;
        }

        bool consumed = onInputEvent(ev);

        if (e->isTouchEvent()) {
            cur_ev_->clearTouchUp();
        }

        bool should_refresh = false;
        switch (ev->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (consumed) {
                is_mouse_down_ = true;
                window_->captureMouse(this);
                if (is_focusable_) {
                    requestFocus();
                }

                if (e->getMouseKey() == InputEvent::MK_LEFT) {
                    setPressed(true);
                    if (fg_drawable_) {
                        fg_drawable_->setHotspot(e->getX(), e->getY());
                        should_refresh = fg_drawable_->setState(Drawable::STATE_PRESSED);
                    }
                    if (bg_drawable_) {
                        bg_drawable_->setHotspot(e->getX(), e->getY());
                        should_refresh = bg_drawable_->setState(Drawable::STATE_PRESSED);
                    }
                    if (should_refresh) {
                        invalidate();
                    }
                }
            }
            return consumed;

        case InputEvent::EVT_DOWN:
            if (consumed) {
                is_touch_down_ = true;
                if (is_focusable_) {
                    requestFocus();
                }
                if (is_touch_capturable_) {
                    window_->captureTouch(this);
                }

                setPressed(true);
                if (fg_drawable_) {
                    fg_drawable_->setHotspot(e->getX(), e->getY());
                    should_refresh = fg_drawable_->setState(Drawable::STATE_PRESSED);
                }
                if (bg_drawable_) {
                    bg_drawable_->setHotspot(e->getX(), e->getY());
                    should_refresh = bg_drawable_->setState(Drawable::STATE_PRESSED);
                }

                if (should_refresh) {
                    invalidate();
                }
            } else {
                cur_ev_->clearTouch();
            }
            return consumed;

        case InputEvent::EVT_MULTI_DOWN:
            return true;

        case InputEvent::EVM_MOVE:
            if (consumed) {
                if (!isPressed()) {
                    if (fg_drawable_) {
                        //fg_drawable_->setHotspot(e->getX(), e->getY());
                        should_refresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                    }
                    if (bg_drawable_) {
                        //bg_drawable_->setHotspot(e->getX(), e->getY());
                        should_refresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                    }
                }
                if (should_refresh) {
                    invalidate();
                }
            }
            return consumed;

        case InputEvent::EVT_MOVE:
            return consumed;

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
            return consumed;

        case InputEvent::EV_LEAVE_VIEW:
            if (e->isMouseEvent()) {
                is_mouse_down_ = false;
            }
            if (e->isTouchEvent()) {
                is_touch_down_ = false;
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
            return consumed;

        case InputEvent::EVM_UP:
            window_->releaseMouse();
            if (window_->getMouseHolderRef() == 0) {
                is_mouse_down_ = false;
            }
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                bool pressed = isPressed();
                setPressed(false);

                if (isLocalPointerInThis(e)) {
                    if (fg_drawable_) {
                        fg_drawable_->setHotspot(e->getX(), e->getY());
                        should_refresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                    }
                    if (bg_drawable_) {
                        bg_drawable_->setHotspot(e->getX(), e->getY());
                        should_refresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                    }

                    if (pressed && click_listener_) {
                        window_->getCycler()->removeCallbacks(click_performer_);
                        window_->getCycler()->post(click_performer_);
                    }
                } else {
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
            return consumed;

        case InputEvent::EVT_MULTI_UP:
            return true;

        case InputEvent::EVT_UP:
        {
            is_touch_down_ = false;
            bool pressed = isPressed();
            setPressed(false);

            if (window_->getTouchHolder() == this) {
                window_->releaseTouch();
            }

            if (fg_drawable_) {
                fg_drawable_->setHotspot(e->getX(), e->getY());
                should_refresh = fg_drawable_->setState(Drawable::STATE_HOVERED);
                should_refresh |= fg_drawable_->setState(Drawable::STATE_NONE);
            }
            if (bg_drawable_) {
                bg_drawable_->setHotspot(e->getX(), e->getY());
                should_refresh = bg_drawable_->setState(Drawable::STATE_HOVERED);
                should_refresh |= bg_drawable_->setState(Drawable::STATE_NONE);
            }

            if (isLocalPointerInThis(e)) {
                if (pressed && click_listener_) {
                    window_->getCycler()->removeCallbacks(click_performer_);
                    window_->getCycler()->post(click_performer_);
                }
            }

            if (should_refresh) {
                invalidate();
            }
            return true;
        }

        case InputEvent::EV_CANCEL:
            if (e->isMouseEvent() && is_mouse_down_) {
                window_->releaseMouse();
                is_mouse_down_ = false;
            }
            if (e->isTouchEvent()) {
                is_touch_down_ = false;
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
            return consumed;

        default:
            break;
        }

        return consumed;
    }

    void View::measure(int width, int height, int width_mode, int height_mode) {
        if (flags_ & MEASURED_SIZE_SET) {
            bool is_force_layout = (flags_ & FORCE_LAYOUT);
            bool is_exactly_mode = (width_mode == EXACTLY && height_mode == EXACTLY);
            bool is_spec_not_change =
                (width == old_pm_width_ && height == old_pm_height_
                    && width_mode == old_pm_width_mode_ && height_mode == old_pm_height_mode_);

            if (!is_force_layout && is_exactly_mode && is_spec_not_change) {
                return;
            }
        }

        flags_ &= ~MEASURED_SIZE_SET;

        onMeasure(width, height, width_mode, height_mode);

        if (!(flags_ & MEASURED_SIZE_SET)) {
            LOG(Log::FATAL) << "You must invoke setMeasuredSize() in onMeasure()!";
        }

        flags_ |= NEED_LAYOUT;

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
            Rect old_bounds(bounds_);

            int width = new_bounds.width();
            int height = new_bounds.height();
            int old_width = bounds_.width();
            int old_height = bounds_.height();

            size_changed = old_width != width || old_height != height;
            bounds_ = new_bounds;
            if (size_changed) {
                onSizeChanged(width, height, old_width, old_height);
            }

            // 将新老 bounds 合并刷新
            old_bounds.join(new_bounds);
            invalidate(old_bounds);
        }

        if (changed || (flags_ & NEED_LAYOUT)) {
            onLayout(changed, size_changed,
                left, top, right, bottom);
        }

        flags_ |= BOUNDS_SET;
        flags_ &= ~FORCE_LAYOUT;
        flags_ &= ~NEED_LAYOUT;
    }

    void View::invalidate() {
        invalidate(bounds_);
    }

    void View::invalidate(const Rect& rect) {
        flags_ |= INVALIDATED;

        if (parent_) {
            int extend = 0;
            if (shadow_effect_) {
                extend = shadow_effect_->getRadius();
                if (extend) {
                    ++extend;
                }
            }

            int off_x = parent_->getLeft() + mTranslateX - parent_->getScrollX();
            int off_y = parent_->getTop() + mTranslateY - parent_->getScrollY();

            Rect p_rect(rect);
            p_rect.insets(-extend, -extend, -extend, -extend);
            p_rect.offset(off_x, off_y);

            parent_->invalidate(p_rect);
        }
    }

    void View::requestLayout() {
        flags_ |= FORCE_LAYOUT;

        if (parent_) {
            parent_->requestLayout();
        }
    }

    void View::requestFocus() {
        if (!is_focusable_ || has_focus_) {
            return;
        }

        // 先取消其他 View 的焦点。
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

    void View::discardMouseCapture() {
        if (window_->getMouseHolder() == this) {
            window_->releaseMouse(true);
        }
    }

    void View::discardTouchCapture() {
        if (window_->getTouchHolder() == this) {
            window_->releaseTouch(true);
        }
    }

    void View::discardPendingOperations() {
        window_->getCycler()->removeCallbacks(click_performer_);
        dispatchDiscardPendingOperations();
    }

    View* View::findViewById(int id) const {
        return nullptr;
    }

    bool View::dispatchInputEvent(InputEvent* e) {
        e->offsetInputPos(-bounds_.left, -bounds_.top);
        return dispatchInputEventToThis(e);
    }

    void View::dispatchWindowFocusChanged(bool focus) {
        onWindowFocusChanged(focus);
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
            input_connection_->unmount();
        }

        if (animator_) {
            animator_->cancel();
        }

        discardFocus();
        discardMouseCapture();
        discardTouchCapture();
        discardPendingOperations();
        updateDrawableState();
    }

    bool View::isViewGroup() const {
        return false;
    }

    bool View::onInputEvent(InputEvent* e) {
        return is_clickable_;
    }

    void View::onMeasure(int width, int height, int width_mode, int height_mode) {
        setMeasuredSize(width > min_width_ ? width : min_width_,
            height > min_height_ ? height : min_height_);
    }

    bool View::onCheckIsTextEditor() {
        return false;
    }

    InputConnection* View::onCreateInputConnection() {
        return nullptr;
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

        bool should_refresh = false;
        if (bg_drawable_) {
            should_refresh = bg_drawable_->setParentFocus(get_focus);
        }
        if (fg_drawable_) {
            should_refresh = fg_drawable_->setParentFocus(get_focus);
        }

        if (should_refresh) {
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

}
