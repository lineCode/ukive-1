#include "debug_view.h"

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"


namespace ukive {

    DebugView::DebugView(Window* w)
        :View(w),
        mode_(Mode::FRAME),
        strip_width_(w->dpToPx(4)),
        screen_width_(Application::getScreenWidth()) {
    }


    void DebugView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        int width = getContentBounds().width();
        int height = getContentBounds().height();

        int cur_x = width;
        int base_height = getWindow()->dpToPx(64);
        float base_time = 0.f;
        if (mode_ == Mode::LAYOUT) {
            base_time = 4;
        } else {
            base_time = 100.f / 6.f;
        }

        for (auto it = durations_.rbegin(); it != durations_.rend(); ++it) {
            int y = height - it->duration / base_time * base_height;
            canvas->fillRect(
                RectF(cur_x - strip_width_, y, strip_width_, height - y),
                mode_ == Mode::FRAME ? Color::Orange400 : Color::Pink200);

            cur_x -= strip_width_;
            if (width - cur_x > screen_width_) {
                durations_.erase(durations_.begin(), it.base());
                break;
            }
        }

        canvas->fillRect(
            RectF(0, std::round(height - base_height), width, getWindow()->dpToPx(2)),
            Color::Red400);
    }

    void DebugView::toggleMode() {
        if (mode_ == Mode::FRAME) {
            mode_ = Mode::LAYOUT;
        } else if (mode_ == Mode::LAYOUT) {
            mode_ = Mode::FRAME;
        }

        durations_.clear();
        invalidate();
    }

    void DebugView::addDuration(uint64_t duration) {
        durations_.push_back(FrameDuration(duration));
    }

    DebugView::Mode DebugView::getMode() {
        return mode_;
    }
}