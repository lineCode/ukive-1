#ifndef UKIVE_VIEWS_DEBUG_VIEW_H_
#define UKIVE_VIEWS_DEBUG_VIEW_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class Window;

    class DebugView : public View {
    public:
        enum Mode {
            RENDER,
            LAYOUT
        };

        explicit DebugView(Window* w);
        DebugView(Window* w, AttrsRef attrs);

        void toggleMode();
        void addDuration(uint64_t duration);

        Mode getMode();

        void onDraw(Canvas* canvas) override;

    private:
        struct FrameDuration {
            float duration;

            FrameDuration(uint64_t micro)
                :duration(micro / 1000.f) {}
        };

        int strip_width_;
        int screen_width_;
        Mode mode_;
        std::vector<FrameDuration> durations_;
    };

}

#endif  // UKIVE_VIEWS_DEBUG_VIEW_H_