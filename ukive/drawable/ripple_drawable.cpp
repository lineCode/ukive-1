#include "ripple_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/bitmap.h"


namespace ukive {

    RippleDrawable::RippleDrawable(Window* win)
        :LayerDrawable(),
        alpha_(0),
        owner_win_(win) {

        down_animator_ = new Animator(win->getAnimationManager());
        up_animator_ = new Animator(win->getAnimationManager());

        hover_animator_ = new Animator(win->getAnimationManager());
        leave_animator_ = new Animator(win->getAnimationManager());

        ripple_animator_ = new Animator(win->getAnimationManager());
    }

    RippleDrawable::~RippleDrawable() {
        delete ripple_animator_;
        delete hover_animator_;
        delete leave_animator_;
        delete up_animator_;
        delete down_animator_;
    }


    void RippleDrawable::setTintColor(Color tint) {
        tint_color_ = tint;
    }

    void RippleDrawable::draw(Canvas *canvas) {
        auto bound = getBounds();
        Color color(0.f, 0.f, 0.f, alpha_);

        // 绘制底色、alpha 和 ripple。
        content_off_->beginDraw();
        content_off_->clear();
        content_off_->setOpacity(canvas->getOpacity());
        content_off_->fillRect(bound.toRectF(), tint_color_);
        content_off_->fillRect(bound.toRectF(), color);

        if ((getState() == STATE_HOVERED && getPrevState() == STATE_PRESSED)
            || (getState() == STATE_NONE && getPrevState() == STATE_HOVERED)) {

            Color rippleColor = Color::ofRGB(0, ripple_animator_->getValue(1));

            content_off_->fillCircle(
                start_x_, start_y_,
                ripple_animator_->getValue(0), rippleColor);
        }
        content_off_->endDraw();
        auto contentBitmap = content_off_->extractBitmap();

        if (drawable_list_.empty()) {
            canvas->drawBitmap(contentBitmap.get());
        } else {
            // 绘制 mask，以该 mask 确定背景形状以及 ripple 的扩散边界。
            mask_off_->beginDraw();
            mask_off_->clear();
            mask_off_->setOpacity(canvas->getOpacity());
            LayerDrawable::draw(mask_off_.get());
            mask_off_->endDraw();
            auto maskBitmap = mask_off_->extractBitmap();

            canvas->drawBitmap(maskBitmap.get());
            canvas->fillOpacityMask(
                bound.width(), bound.height(),
                maskBitmap.get(), contentBitmap.get());
        }
    }

    void RippleDrawable::onBoundChanged(const Rect& new_bound) {
        LayerDrawable::onBoundChanged(new_bound);

        mask_off_.reset();
        mask_off_ = std::make_unique<Canvas>(
            new_bound.width(), new_bound.height());

        content_off_.reset();
        content_off_ = std::make_unique<Canvas>(
            new_bound.width(), new_bound.height());
    }

    bool RippleDrawable::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = LayerDrawable::onStateChanged(new_state, prev_state);

        switch (new_state) {
        case STATE_NONE:
        {
            if (prev_state == STATE_HOVERED) {
                hover_animator_->stop();
                alpha_ = hover_animator_->getValue(0);

                leave_animator_->addVariable(0, alpha_, 0, 1);
                leave_animator_->setOnValueChangedListener(0, this);
                leave_animator_->startTransition(0, Transition::linearTransition(HOVER_LEAVE_SEC, 0));

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_->stop();
                alpha_ = down_animator_->getValue(0);

                up_animator_->addVariable(0, alpha_, 0, 1);
                up_animator_->setOnValueChangedListener(0, this);
                up_animator_->startTransition(0, Transition::linearTransition(DOWN_UP_SEC, 0));

                need_redraw |= true;
            }
            break;
        }

        case STATE_HOVERED:
            if (prev_state == STATE_NONE) {
                alpha_ = 0;
                hover_animator_->addVariable(0, 0, 0, 1);
                hover_animator_->setOnValueChangedListener(0, this);
                hover_animator_->startTransition(0, Transition::linearTransition(HOVER_LEAVE_SEC, HOVER_ALPHA));

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_->stop();
                alpha_ = down_animator_->getValue(0);

                up_animator_->addVariable(0, alpha_, 0, 1);
                up_animator_->setOnValueChangedListener(0, this);
                up_animator_->startTransition(0, Transition::linearTransition(DOWN_UP_SEC, HOVER_ALPHA));

                ripple_animator_->reset();
                ripple_animator_->addVariable(0, 0, 0, 100);
                ripple_animator_->addVariable(1, 0.1, 0, 1);
                ripple_animator_->addTransition(0, Transition::linearTransition(0.5, 100));
                ripple_animator_->addTransition(1, Transition::linearTransition(0.5, 0));
                ripple_animator_->start();

                need_redraw |= true;
            }
            break;

        case STATE_PRESSED:
            hover_animator_->stop();
            alpha_ = hover_animator_->getValue(0);

            down_animator_->addVariable(0, alpha_, 0, 1);
            down_animator_->setOnValueChangedListener(0, this);
            down_animator_->startTransition(0, Transition::linearTransition(DOWN_UP_SEC, DOWN_ALPHA));

            need_redraw |= true;
            break;

        case STATE_DISABLED:
            up_animator_->stop();
            up_animator_->setOnValueChangedListener(0, nullptr);
            down_animator_->stop();
            down_animator_->setOnValueChangedListener(0, nullptr);
            hover_animator_->stop();
            hover_animator_->setOnValueChangedListener(0, nullptr);
            leave_animator_->stop();
            leave_animator_->setOnValueChangedListener(0, nullptr);
            ripple_animator_->stop();
            ripple_animator_->setOnValueChangedListener(0, nullptr);
            alpha_ = 0;

            need_redraw |= true;
            break;

        default:
            break;
        }

        return need_redraw;
    }

    float RippleDrawable::getOpacity() {
        return 1.0f;
    }


    void RippleDrawable::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        double newValue, double previousValue)
    {
        alpha_ = newValue;
    }

}