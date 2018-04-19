#include "inner_window.h"

#include "ukive/event/input_event.h"
#include "ukive/views/layout/root_layout.h"
#include "ukive/views/layout/root_layout_params.h"
#include "ukive/window/window.h"


namespace ukive {

    InnerWindow::InnerWindow(Window* wnd)
        :parent_(wnd),
        elevation_(0.f),
        is_showing_(false),
        outside_touchable_(false),
        dismiss_by_touch_outside_(false)
    {
        decor_view_ = nullptr;
        content_view_ = nullptr;

        background_drawable_ = nullptr;
        width_ = LayoutParams::FIT_CONTENT;
        height_ = LayoutParams::FIT_CONTENT;
    }

    InnerWindow::~InnerWindow() {
        if (decor_view_ && !is_showing_) {
            delete decor_view_;
        }
    }


    void InnerWindow::createDecorView() {
        decor_view_ = new InnerDecorView(this);
        decor_view_->addView(
            content_view_, new LayoutParams(width_, height_));

        decor_view_->setElevation(elevation_);
        decor_view_->setBackground(background_drawable_);
        decor_view_->setReceiveOutsideInputEvent(!outside_touchable_);
    }


    void InnerWindow::setWidth(int width) {
        width_ = width;
    }

    void InnerWindow::setHeight(int height) {
        height_ = height;
    }

    void InnerWindow::setSize(int width, int height) {
        width_ = width;
        height_ = height;
    }

    void InnerWindow::setElevation(float elevation) {
        elevation_ = elevation;
    }

    void InnerWindow::setBackground(Drawable* drawable) {
        background_drawable_ = drawable;
    }

    void InnerWindow::setOutsideTouchable(bool touchable) {
        outside_touchable_ = touchable;
    }

    void InnerWindow::setDismissByTouchOutside(bool enable) {
        dismiss_by_touch_outside_ = enable;
    }

    void InnerWindow::setContentView(View* contentView) {
        if (contentView == nullptr) {
            throw std::invalid_argument("setContentView: null param");
        }

        content_view_ = contentView;
    }


    int InnerWindow::getWidth() {
        return width_;
    }

    int InnerWindow::getHeight() {
        return height_;
    }

    float InnerWindow::getElevation() {
        return elevation_;
    }

    Drawable* InnerWindow::getBackground() {
        return background_drawable_;
    }

    bool InnerWindow::isOutsideTouchable() {
        return outside_touchable_;
    }

    bool InnerWindow::isDismissByTouchOutside() {
        return dismiss_by_touch_outside_;
    }

    Window* InnerWindow::getParent() {
        return parent_;
    }

    View* InnerWindow::getContentView() {
        return content_view_;
    }

    View* InnerWindow::getDecorView() {
        return decor_view_;
    }

    bool InnerWindow::isShowing() {
        return is_showing_;
    }


    void InnerWindow::show(int x, int y) {
        if (!content_view_ || is_showing_) {
            return;
        }

        if (decor_view_) {
            delete decor_view_;
        }

        createDecorView();

        RootLayoutParams* baselp
            = new RootLayoutParams(width_, height_);
        baselp->leftMargin = x;
        baselp->topMargin = y;

        decor_view_->setLayoutParams(baselp);

        parent_->getRootLayout()->addShade(decor_view_);

        is_showing_ = true;
    }

    void InnerWindow::show(View* anchor, View::Gravity gravity) {
        if (!content_view_ || !anchor || is_showing_) {
            return;
        }

        createDecorView();

        Rect rect = anchor->getBoundsInWindow();
    }

    void InnerWindow::update(int x, int y) {
        if (!decor_view_ || !is_showing_) {
            return;
        }

        RootLayoutParams* baselp
            = (RootLayoutParams*)decor_view_->getLayoutParams();
        baselp->leftMargin = x;
        baselp->topMargin = y;

        decor_view_->setLayoutParams(baselp);
    }

    void InnerWindow::update(View* anchor, View::Gravity gravity) {
    }

    void InnerWindow::dismiss() {
        if (decor_view_ && is_showing_) {
            parent_->getRootLayout()->removeShade(decor_view_);
        }
        is_showing_ = false;
    }


    InnerWindow::InnerDecorView::InnerDecorView(InnerWindow* inner)
        :FrameLayout(inner->getParent()),
        inner_window_(inner) {}

    InnerWindow::InnerDecorView::~InnerDecorView() {
    }


    bool InnerWindow::InnerDecorView::onInterceptInputEvent(InputEvent* e) {
        return false;
    }

    bool InnerWindow::InnerDecorView::onInputEvent(InputEvent* e) {
        if (e->isOutside()) {
            if (e->getEvent() == InputEvent::EVM_DOWN
                || e->getEvent() == InputEvent::EVM_UP)
            {
                if (inner_window_->dismiss_by_touch_outside_) {
                    inner_window_->dismiss();
                }
            }
            return true;
        }

        return false;
    }

}