#ifndef UKIVE_VIEWS_SCROLL_VIEW_H_
#define UKIVE_VIEWS_SCROLL_VIEW_H_

#include "ukive/animation/scroller.h"
#include "ukive/event/velocity_calculator.h"
#include "ukive/views/layout/view_group.h"


namespace ukive {

    class Scroller;
    class InputEvent;

    class ScrollView : public ViewGroup {
    public:
        explicit ScrollView(Window* w);
        ScrollView(Window* w, AttrsRef attrs);

        void onMeasure(
            int width, int height,
            int width_mode, int height_mode) override;

        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;

        void onSizeChanged(
            int width, int height, int old_w, int old_h) override;

        void onScrollChanged(
            int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) override;

        bool onInputEvent(InputEvent* e) override;
        bool onInterceptInputEvent(InputEvent* e) override;

        void onComputeScroll() override;

    private:
        bool canScroll();
        int computeScrollRange();
        int computeScrollExtend();

        bool processVerticalScroll(int dy);

        int mouse_x_cache_;
        int mouse_y_cache_;
        int saved_pointer_type_;

        int prev_touch_x_ = 0;
        int prev_touch_y_ = 0;
        int start_touch_x_ = 0;
        int start_touch_y_ = 0;
        bool is_touch_down_ = false;

        Scroller scroller_;
        VelocityCalculator velocity_calculator_;
    };

}

#endif  // UKIVE_VIEWS_SCROLL_VIEW_H_