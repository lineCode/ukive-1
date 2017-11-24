#ifndef UKIVE_VIEWS_SCROLL_VIEW_H_
#define UKIVE_VIEWS_SCROLL_VIEW_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class UScroller;
    class UInputEvent;

    class ScrollView : public ViewGroup
    {
    private:
        int mMouseXCache;
        int mMouseYCache;

        UScroller *mScroller;

        bool canScroll();
        int computeScrollRange();
        int computeScrollExtend();

        void processVerticalScroll(int dy);

    public:
        ScrollView(Window *wnd);
        ScrollView(Window *wnd, int id);
        virtual ~ScrollView();

        virtual void onMeasure(
            int width, int height,
            int widthSpec, int heightSpec) override;

        virtual void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;

        virtual void onSizeChanged(
            int width, int height, int oldWidth, int oldHeight) override;

        virtual void onScrollChanged(
            int scrollX, int scrollY, int oldScrollX, int oldScrollY) override;

        virtual bool onInputEvent(InputEvent *e) override;
        virtual bool onInterceptMouseEvent(InputEvent *e) override;
    };

}

#endif  // UKIVE_VIEWS_SCROLL_VIEW_H_