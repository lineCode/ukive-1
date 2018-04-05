#ifndef UKIVE_VIEWS_SCROLL_VIEW_H_
#define UKIVE_VIEWS_SCROLL_VIEW_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class Scroller;
    class InputEvent;

    class ScrollView : public ViewGroup
    {
    public:
        ScrollView(Window* w);

        void onMeasure(
            int width, int height,
            int widthSpec, int heightSpec) override;

        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;

        void onSizeChanged(
            int width, int height, int oldWidth, int oldHeight) override;

        void onScrollChanged(
            int scrollX, int scrollY, int oldScrollX, int oldScrollY) override;

        bool onInputEvent(InputEvent* e) override;
        bool onInterceptMouseEvent(InputEvent* e) override;

    private:
        bool canScroll();
        int computeScrollRange();
        int computeScrollExtend();

        void processVerticalScroll(int dy);

        int mMouseXCache;
        int mMouseYCache;

        Scroller* mScroller;
    };

}

#endif  // UKIVE_VIEWS_SCROLL_VIEW_H_