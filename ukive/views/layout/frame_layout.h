#ifndef UKIVE_VIEWS_LAYOUT_FRAME_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_FRAME_LAYOUT_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class FrameLayout : public ViewGroup
    {
    public:
        FrameLayout(Window *wnd);
        FrameLayout(Window *wnd, int id);
        virtual ~FrameLayout();

        virtual void onMeasure(
            int width, int height, int widthSpec, int heightSpec) override;
        virtual void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_FRAME_LAYOUT_H_