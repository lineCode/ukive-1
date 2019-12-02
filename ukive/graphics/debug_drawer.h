#ifndef UKIVE_GRAPHICS_DEBUG_DRAWER_H_
#define UKIVE_GRAPHICS_DEBUG_DRAWER_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class Window;

    class DebugDrawer {
    public:
        enum Mode {
            RENDER,
            LAYOUT
        };

        explicit DebugDrawer(Window* w);

        void toggleMode();
        void addDuration(uint64_t duration);

        Mode getMode();

        void draw(int width, int height, Canvas* canvas);

    private:
        struct FrameDuration {
            float duration;

            FrameDuration(uint64_t micro)
                :duration(micro / 1000.f) {}
        };

        Window* window_;
        int strip_width_;
        int screen_width_;
        Mode mode_;
        std::vector<FrameDuration> durations_;
    };

}

#endif  // UKIVE_GRAPHICS_DEBUG_DRAWER_H_