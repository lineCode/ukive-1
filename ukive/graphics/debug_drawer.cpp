#include "debug_drawer.h"

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"


namespace ukive {

    DebugDrawer::DebugDrawer(Window* w)
        : window_(w),
          strip_width_(w->dpToPxX(4)),
          screen_width_(Application::getScreenWidth()),
          mode_(RENDER) {
    }

    void DebugDrawer::draw(int width, int height, Canvas* canvas) {
        int cur_x = width;
        int base_height = window_->dpToPxX(64);
        float base_time = 0.f;
        if (mode_ == LAYOUT) {
            base_time = 4;
        } else {
            base_time = 100.f / 6.f;
        }

        for (auto it = durations_.rbegin(); it != durations_.rend(); ++it) {
            int y = height - it->duration / base_time * base_height;
            canvas->fillRect(
                RectF(cur_x - strip_width_, y, strip_width_, height - y),
                mode_ == RENDER ? Color::Orange400 : Color::Pink200);

            cur_x -= strip_width_;
            if (width - cur_x > screen_width_) {
                durations_.erase(durations_.begin(), it.base());
                break;
            }
        }

        canvas->fillRect(
            RectF(0, std::round(height - base_height), width, std::round(window_->dpToPxX(2))),
            Color::Red400);
    }

    void DebugDrawer::toggleMode() {
        if (mode_ == RENDER) {
            mode_ = LAYOUT;
        } else if (mode_ == LAYOUT) {
            mode_ = RENDER;
        }

        durations_.clear();
    }

    void DebugDrawer::addDuration(uint64_t duration) {
        durations_.push_back(FrameDuration(duration));
    }

    DebugDrawer::Mode DebugDrawer::getMode() {
        return mode_;
    }
}