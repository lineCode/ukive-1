#include "ukive/drawable/check_drawable.h"

#include <algorithm>

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/point.h"
#include "ukive/window/window.h"


namespace ukive {

    CheckDrawable::CheckDrawable(Window* w)
        :win_(w),
        checked_(false) {

        anim_ = std::make_unique<Animator>(w->getAnimationManager());
    }

    void CheckDrawable::draw(Canvas* canvas) {
        auto bound = getBounds();

        float hp_offset = 0.f;
        float total_offset = 0.f;
        int length = std::ceil(win_->dpToPx(16));
        int stroke_width = win_->dpToPx(2.5f);
        if (stroke_width % 2) {
            hp_offset = 0.5f;
        }

        total_offset = hp_offset + std::floor(stroke_width / 2.f);

        RectF check_bounds(
            bound.left + total_offset,
            bound.top + (bound.height() - length) / 2.f + hp_offset,
            length, length);

        canvas->drawRoundRect(
            check_bounds, stroke_width, win_->dpToPx(1), Color::Blue400);

        if (checked_) {
            auto value = static_cast<float>(anim_->getValue(0));

            auto line1s = PointF(check_bounds.left + stroke_width, check_bounds.top + length / 2.f);
            auto line1e = PointF(check_bounds.left + length / 2.f, check_bounds.top + length - stroke_width);
            auto line2s = PointF(check_bounds.left + length / 2.f, check_bounds.top + length - stroke_width);
            auto line2e = PointF(check_bounds.left + length - stroke_width, check_bounds.top + stroke_width);

            auto vec1 = line1e - line1s;
            auto vec2 = line2e - line2s;

            canvas->drawLine(line1s, line1s + vec1 * (std::min(value, 0.5f) * 2), std::floor(win_->dpToPx(3)), Color::Blue800);
            canvas->drawLine(line2s, line2s + vec2 * (std::max(value - 0.5f, 0.f) * 2), std::floor(win_->dpToPx(3)), Color::Blue800);
        }
    }

    void CheckDrawable::setChecked(bool checked) {
        if (checked_) {
            anim_->addVariable(0, 0, 0, 1);
            anim_->startTransition(0, Transition::linearTransition(0.24, 1));
        } else {
            anim_->stop();
        }
    }

    bool CheckDrawable::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = false;

        switch (new_state) {
        case STATE_NONE:
            break;

        case STATE_HOVERED:
            if (prev_state == STATE_PRESSED) {
                checked_ = !checked_;
                if (checked_) {
                    anim_->addVariable(0, 0, 0, 1);
                    anim_->startTransition(0, Transition::linearTransition(0.24, 1));
                } else {
                    anim_->stop();
                }
                need_redraw |= true;
            }
            break;

        case STATE_PRESSED:
            break;

        case STATE_DISABLED:
            break;

        default:
            break;
        }

        return need_redraw;
    }

}