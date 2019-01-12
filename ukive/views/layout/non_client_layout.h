#ifndef UKIVE_VIEWS_LAYOUT_NON_CLIENT_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_NON_CLIENT_LAYOUT_H_

#include "ukive/views/layout/view_group.h"
#include "ukive/graphics/rect.h"


namespace ukive {

    class NonClientLayout : public ViewGroup {
    public:
        explicit NonClientLayout(Window* w);

        virtual HitPoint onNCHitTest(int x, int y);

        void setNonClientPadding(int left, int top, int right, int bottom);
        void setSizeHandlePadding(int left, int top, int right, int bottom);

        void onMeasure(
            int width, int height, int width_mode, int height_mode) override;
        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
        void onDraw(Canvas* canvas) override;

    private:
        Rect nc_padding_;
        Rect sh_padding_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_NON_CLIENT_LAYOUT_H_