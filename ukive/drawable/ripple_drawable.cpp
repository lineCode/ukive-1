#include "ripple_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/animation/interpolator.h"

#define DOWN_UP_SEC      200
#define HOVER_LEAVE_SEC  200

#define HOVER_ALPHA  0.07
#define DOWN_ALPHA   0.13


namespace ukive {

    RippleDrawable::RippleDrawable()
        : LayerDrawable(),
          alpha_(0)
    {
        ripple_animator_.setDuration(500);
        ripple_animator_.setInterpolator(new LinearInterpolator(1));

        hover_animator_.setListener(this);
        hover_animator_.setDuration(HOVER_LEAVE_SEC);
        hover_animator_.setInterpolator(new LinearInterpolator(HOVER_ALPHA));

        down_animator_.setListener(this);
        down_animator_.setDuration(DOWN_UP_SEC);
        down_animator_.setInterpolator(new LinearInterpolator(DOWN_ALPHA));

        up_animator_.setListener(this);
        up_animator_.setDuration(DOWN_UP_SEC);

        leave_animator_.setListener(this);
        leave_animator_.setDuration(HOVER_LEAVE_SEC);
        leave_animator_.setInterpolator(new LinearInterpolator(0));
    }

    RippleDrawable::~RippleDrawable() {
    }

    void RippleDrawable::setTintColor(Color tint) {
        tint_color_ = tint;
    }

    void RippleDrawable::draw(Canvas *canvas) {
        hover_animator_.update();
        leave_animator_.update();
        down_animator_.update();
        up_animator_.update();
        ripple_animator_.update();

        auto bound = getBounds();
        Color color(0.f, 0.f, 0.f, float(alpha_));

        bool has_content = tint_color_.a > 0.f || color.a > 0.f || ripple_animator_.isRunning();
        if (has_content) {
            // 绘制底色、alpha 和 ripple。
            content_off_->beginDraw();
            content_off_->clear();
            content_off_->setOpacity(canvas->getOpacity());
            if (tint_color_.a > 0.f) {
                content_off_->fillRect(bound.toRectF(), tint_color_);
            }
            if (color.a > 0.f) {
                content_off_->fillRect(bound.toRectF(), color);
            }

            if (ripple_animator_.isRunning()) {
                Color rippleColor = Color::ofRGB(0, (1 - ripple_animator_.getCurValue())*0.1f);
                content_off_->fillCircle(
                    start_x_, start_y_,
                    ripple_animator_.getCurValue() * 100, rippleColor);
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
        } else {
            LayerDrawable::draw(canvas);
        }

        if (hover_animator_.isRunning() ||
            leave_animator_.isRunning() ||
            down_animator_.isRunning() ||
            up_animator_.isRunning() ||
            ripple_animator_.isRunning())
        {
            invalidate();
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
                if (hover_animator_.isRunning()) {
                    hover_animator_.stop();
                    alpha_ = hover_animator_.getCurValue();
                } else if (up_animator_.isRunning()) {
                    up_animator_.stop();
                    alpha_ = up_animator_.getCurValue();
                }

                leave_animator_.setInitValue(alpha_);
                leave_animator_.reset();
                leave_animator_.start();

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_.stop();
                alpha_ = down_animator_.getCurValue();

                up_animator_.setInitValue(alpha_);
                up_animator_.setInterpolator(new LinearInterpolator(0));
                up_animator_.reset();
                up_animator_.start();

                need_redraw |= true;
            }
            break;
        }

        case STATE_HOVERED:
            if (prev_state == STATE_NONE) {
                alpha_ = 0;
                hover_animator_.reset();
                hover_animator_.start();

                need_redraw |= true;
            } else if (prev_state == STATE_PRESSED) {
                down_animator_.stop();
                alpha_ = down_animator_.getCurValue();

                up_animator_.setInitValue(alpha_);
                up_animator_.setInterpolator(new LinearInterpolator(HOVER_ALPHA));
                up_animator_.reset();
                up_animator_.start();

                ripple_animator_.reset();
                ripple_animator_.start();

                need_redraw |= true;
            }
            break;

        case STATE_PRESSED:
            hover_animator_.stop();
            alpha_ = hover_animator_.getCurValue();

            down_animator_.setInitValue(alpha_);
            down_animator_.reset();
            down_animator_.start();

            need_redraw |= true;
            break;

        case STATE_DISABLED:
            up_animator_.stop();
            down_animator_.stop();
            hover_animator_.stop();
            leave_animator_.stop();
            ripple_animator_.stop();
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

    void RippleDrawable::onAnimationProgress(Animator* animator) {
        alpha_ = animator->getCurValue();
    }

}
