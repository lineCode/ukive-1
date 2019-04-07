#include "seek_bar.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"
#include "ukive/graphics/color.h"
#include "ukive/animation/interpolator.h"


namespace ukive {

    SeekBar::SeekBar(Window* w)
        : SeekBar(w, {}) {}

    SeekBar::SeekBar(Window* w, AttrsRef attrs)
        : View(w, attrs)
    {
        initSeekBar();
    }

    SeekBar::~SeekBar() {
    }

    void SeekBar::initSeekBar()
    {
        maximum_ = 100.f;
        seek_percent_ = 0.f;
        is_pointer_left_key_available_ = false;
        listener_ = nullptr;

        track_height_ = getWindow()->dpToPx(2);
        if (track_height_ % 2 != 0) {
            ++track_height_;
        }

        thumb_min_diameter_ = getWindow()->dpToPx(10);
        if (thumb_min_diameter_ % 2 != 0) {
            ++thumb_min_diameter_;
        }

        thumb_max_diameter_ = getWindow()->dpToPx(14);
        if (thumb_max_diameter_ % 2 != 0) {
            ++thumb_max_diameter_;
        }

        thumb_cur_diameter_ = thumb_min_diameter_;

        thumb_in_animator_.setListener(this);
        thumb_in_animator_.setDuration(100);
        thumb_in_animator_.setInterpolator(new LinearInterpolator(thumb_max_diameter_));

        thumb_out_animator_.setListener(this);
        thumb_out_animator_.setDuration(100);
        thumb_out_animator_.setInterpolator(new LinearInterpolator(thumb_min_diameter_));
    }

    void SeekBar::setMaximum(float maximum) {
        if (maximum_ > 0) {
            maximum_ = maximum;
        }
    }

    void SeekBar::setProgress(float progress, bool notify) {
        float prog = std::min(maximum_, progress);
        float percent = prog / maximum_;
        if (percent != seek_percent_) {
            seek_percent_ = percent;

            if (notify && listener_) {
                listener_->onSeekValueChanged(this, seek_percent_*maximum_);
                listener_->onSeekIntegerValueChanged(this, static_cast<int>(seek_percent_*maximum_));
            }

            invalidate();
        }
    }

    float SeekBar::getProgress() {
        return maximum_ * seek_percent_;
    }

    void SeekBar::setOnSeekValueChangedListener(OnSeekValueChangedListener* l) {
        listener_ = l;
    }

    bool SeekBar::isPointerInThumb(int x, int y) {
        float thumb_radius = thumb_max_diameter_ / 2.f;
        int track_width = getWidth() - thumb_max_diameter_ - getPaddingLeft() - getPaddingRight();

        float thumb_center_x_in_track = track_width * seek_percent_ + thumb_radius;
        float thumb_center_y_in_track = getPaddingTop() + thumb_radius;

        if (((x - getPaddingLeft()) >= (thumb_center_x_in_track - thumb_radius))
            && ((x - getPaddingLeft()) <= (thumb_center_x_in_track + thumb_radius)))
        {
            if ((y - getPaddingTop()) >= (thumb_center_y_in_track - thumb_radius)
                && (y - getPaddingTop()) <= (thumb_center_y_in_track + thumb_radius))
                return true;
        }

        return false;
    }

    bool SeekBar::isPointerInTrack(int x, int y) {
        float trackSpace = getWidth() - getPaddingLeft() - getPaddingRight();

        if ((x - getPaddingLeft()) >= 0 &&
            (x - getPaddingLeft()) <= trackSpace)
        {
            if ((y - getPaddingTop()) >= 0 &&
                (y - getPaddingTop()) <= thumb_max_diameter_)
            {
                return true;
            }
        }

        return false;
    }

    void SeekBar::computePercent(int x, int y) {
        float mouse_in_track = x - getPaddingLeft() - thumb_max_diameter_ / 2.f;
        float track_width = getWidth() - thumb_max_diameter_ - getPaddingLeft() - getPaddingRight();
        seek_percent_ = std::max(0.f, mouse_in_track / track_width);
        seek_percent_ = std::min(1.f, seek_percent_);

        if (listener_) {
            listener_->onSeekValueChanged(this, seek_percent_*maximum_);
            listener_->onSeekIntegerValueChanged(this, static_cast<int>(seek_percent_*maximum_));
        }
    }

    void SeekBar::startZoomInAnimation() {
        if (thumb_cur_diameter_ < thumb_max_diameter_) {
            thumb_in_animator_.reset();
            thumb_out_animator_.stop();

            thumb_in_animator_.setInitValue(thumb_cur_diameter_);
            thumb_in_animator_.start();
            invalidate();
        }
    }

    void SeekBar::startZoomOutAnimation() {
        if (thumb_cur_diameter_ > thumb_min_diameter_) {
            thumb_in_animator_.stop();
            thumb_out_animator_.reset();

            thumb_out_animator_.setInitValue(thumb_cur_diameter_);
            thumb_out_animator_.start();
            invalidate();
        }
    }

    void SeekBar::onMeasure(
        int width, int height, int width_mode, int height_mode)
    {
        int final_width = 0;
        int final_height = 0;

        int h_padding = getPaddingLeft() + getPaddingRight();
        int v_padding = getPaddingTop() + getPaddingBottom();

        switch (width_mode) {
        case FIT:
            final_width = std::max(width, getMinimumWidth());
            break;

        case UNKNOWN:
            final_width = std::max(0, getMinimumWidth());
            break;

        case EXACTLY:
        default:
            final_width = width;
            break;
        }

        switch (height_mode) {
        case FIT:
            final_height = std::min(height, thumb_max_diameter_ + v_padding);
            final_height = std::max(final_height, getMinimumHeight());
            break;

        case UNKNOWN:
            final_height = std::max(thumb_max_diameter_ + v_padding, getMinimumHeight());
            break;

        case EXACTLY:
        default:
            final_height = height;
            break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void SeekBar::onDraw(Canvas* canvas) {
        thumb_in_animator_.update();
        thumb_out_animator_.update();

        float left = thumb_max_diameter_ / 2.f;
        float top = (thumb_max_diameter_ - track_height_) / 2.f;
        float trackWidth = getWidth() - thumb_max_diameter_ - getPaddingLeft() - getPaddingRight();

        float cur_pos = trackWidth * seek_percent_;
        float center_x = left + cur_pos;
        float center_y = thumb_max_diameter_ / 2.f;

        if (center_x < thumb_min_diameter_) {
            canvas->fillRect(RectF(left, top, trackWidth, track_height_), Color::Grey300);
        } else {
            canvas->fillRect(RectF(left, top, cur_pos, track_height_), Color::Blue400);
            canvas->fillRect(RectF(center_x, top, trackWidth - cur_pos, track_height_), Color::Grey300);
        }

        canvas->fillCircle(center_x, center_y, thumb_cur_diameter_ / 2.f, Color::Blue400);

        if (thumb_in_animator_.isRunning() || thumb_out_animator_.isRunning()) {
            invalidate();
        }
    }

    bool SeekBar::onInputEvent(InputEvent* e) {
        View::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EV_LEAVE_VIEW:
        {
            startZoomOutAnimation();
            break;
        }

        case InputEvent::EVM_DOWN:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                if (isPointerInThumb(e->getX(), e->getY())) {
                    is_pointer_left_key_available_ = true;
                    computePercent(e->getX(), e->getY());
                    invalidate();
                } else if (isPointerInTrack(e->getX(), e->getY())) {
                    is_pointer_left_key_available_ = true;
                    computePercent(e->getX(), e->getY());
                    startZoomInAnimation();
                }
            }

            break;
        }

        case InputEvent::EVT_DOWN:
        {
            if (isPointerInThumb(e->getX(), e->getY())) {
                is_pointer_left_key_available_ = true;
                computePercent(e->getX(), e->getY());
                invalidate();
            } else if (isPointerInTrack(e->getX(), e->getY())) {
                is_pointer_left_key_available_ = true;
                computePercent(e->getX(), e->getY());
                startZoomInAnimation();
            }
            break;
        }

        case InputEvent::EVM_MOVE:
        {
            if (is_pointer_left_key_available_) {
                computePercent(e->getX(), e->getY());
                invalidate();
            } else {
                if (isPointerInThumb(e->getX(), e->getY())) {
                    startZoomInAnimation();
                } else {
                    startZoomOutAnimation();
                }
            }
            break;
        }

        case InputEvent::EVT_MOVE:
        {
            if (is_pointer_left_key_available_) {
                computePercent(e->getX(), e->getY());
                invalidate();
            }
            break;
        }

        case InputEvent::EVM_UP:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                if (is_pointer_left_key_available_) {
                    is_pointer_left_key_available_ = false;
                }
            }
            break;
        }

        case InputEvent::EVT_UP:
        {
            if (is_pointer_left_key_available_) {
                is_pointer_left_key_available_ = false;
            }
            break;
        }
        default:
            break;
        }

        return true;
    }

    void SeekBar::onAnimationProgress(Animator2* animator) {
        thumb_cur_diameter_ = animator->getCurValue();
    }

}
