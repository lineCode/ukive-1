#include "text_blink.h"

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/message/cycler.h"
#include "ukive/views/view.h"
#include "ukive/window/window.h"


namespace ukive {

    TextBlink::TextBlink(View* v)
        :target_view_(v),
        is_located_(false),
        is_cancelled_(true),
        blink_mask_(false),
        color_(Color::Black) {

        blink_cycler_ = new Cycler();
        thickness_ = v->getWindow()->dpToPxX(2);
    }

    TextBlink::~TextBlink() {
        is_cancelled_ = true;
        blink_cycler_->removeCallbacks(this);
        delete blink_cycler_;
    }


    void TextBlink::draw(Canvas* canvas) {
        if (blink_mask_ && is_located_) {
            canvas->fillRect(blink_rect_.toRectF(), color_);
        }
    }


    void TextBlink::show() {
        if (!is_located_) {
            return;
        }

        is_cancelled_ = false;
        blink_mask_ = false;

        blink_cycler_->removeCallbacks(this);
        blink_cycler_->post(this);
    }

    void TextBlink::hide() {
        is_cancelled_ = true;
        blink_cycler_->removeCallbacks(this);

        blink_mask_ = false;
        target_view_->invalidate();
    }

    void TextBlink::locate(float xCenter, float top, float bottom) {
        int topInt = std::floor(top);
        int bottomInt = std::ceil(bottom);

        int leftInt = std::round(xCenter - thickness_ / 2.f);
        int rightInt = leftInt + thickness_;

        // 防止blink被截断。
        if (leftInt < 0) {
            rightInt += -leftInt;
            leftInt = 0;
        }

        if (topInt == blink_rect_.top
            && bottomInt == blink_rect_.bottom
            && leftInt == blink_rect_.left
            && rightInt == blink_rect_.right) {
            return;
        }

        bool shouldShow = false;

        if (!is_cancelled_) {
            hide();
            shouldShow = true;
        }

        blink_rect_.left = leftInt;
        blink_rect_.top = topInt;
        blink_rect_.right = rightInt;
        blink_rect_.bottom = bottomInt;

        is_located_ = true;

        if (shouldShow) {
            show();
        }
    }


    void TextBlink::setColor(const Color& color) {
        color_ = color;
    }

    void TextBlink::setThickness(float thickness) {
        thickness_ = thickness;
    }

    bool TextBlink::isShowing() const {
        return !is_cancelled_;
    }

    Color TextBlink::getColor() const {
        return color_;
    }

    float TextBlink::getThickness() const {
        return thickness_;
    }

    void TextBlink::run() {
        if (!is_cancelled_) {
            blink_mask_ = !blink_mask_;

            target_view_->invalidate();

            blink_cycler_->removeCallbacks(this);
            blink_cycler_->postDelayed(this, 500);
        }
    }

}