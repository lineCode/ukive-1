#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_

#include "ukive/views/layout/frame_layout.h"


namespace ukive {

    class Window;
    class LayoutParams;
    class LinearLayout;

    class RootLayout : public FrameLayout {
    public:
        RootLayout(Window* w);
        ~RootLayout();

        void addShade(View* shade);
        void removeShade(View* shade);

        void addContent(View* content);

        void requestLayout() override;
        View* findViewById(int id) override;

    protected:
        LayoutParams* generateLayoutParams(const LayoutParams &lp) override;
        LayoutParams* generateDefaultLayoutParams() override;
        bool checkLayoutParams(LayoutParams* lp) override;

    private:
        FrameLayout* shade_layout_;
        LinearLayout* content_layout_;

        bool shade_added_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_