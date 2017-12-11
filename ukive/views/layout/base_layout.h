#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_

#include "ukive/views/layout/frame_layout.h"


namespace ukive {

    class Window;
    class LayoutParams;
    class LinearLayout;

    class BaseLayout : public FrameLayout
    {
    public:
        BaseLayout(Window *wnd);
        BaseLayout(Window *wnd, int id);
        ~BaseLayout();

        void addShade(View *shade);
        void removeShade(View *shade);

        void addContent(View *content);

        View *findViewById(int id) override;

    protected:
        LayoutParams *generateLayoutParams(const LayoutParams &lp) override;
        LayoutParams *generateDefaultLayoutParams() override;
        bool checkLayoutParams(LayoutParams *lp) override;

    private:
        void initBaseLayout();

        FrameLayout* shade_layout_;
        LinearLayout* content_layout_;

        bool shade_added_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_