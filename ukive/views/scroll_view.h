#ifndef UKIVE_VIEWS_SCROLL_VIEW_H_
#define UKIVE_VIEWS_SCROLL_VIEW_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class Scroller;
    class InputEvent;

    class ScrollView : public ViewGroup {
    public:
        explicit ScrollView(Window* w);

        void onMeasure(
            int width, int height,
            int width_mode, int height_mode) override;

        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;

        void onSizeChanged(
            int width, int height, int old_w, int old_h) override;

        void onScrollChanged(
            int scrollX, int scrollY, int oldScrollX, int oldScrollY) override;

        bool onInputEvent(InputEvent* e) override;
        bool onInterceptInputEvent(InputEvent* e) override;

    private:
        bool canScroll();
        int computeScrollRange();
        int computeScrollExtend();

        void processVerticalScroll(int dy);

        int mouse_x_cache_;
        int mouse_y_cache_;
        int saved_pointer_type_;

        Scroller* scroller_;
    };

}

#endif  // UKIVE_VIEWS_SCROLL_VIEW_H_