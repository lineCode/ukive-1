#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_

#include "ukive/views/layout/non_client_layout.h"


namespace ukive {

    class Window;
    class DebugView;
    class LayoutParams;
    class FrameLayout;
    class LinearLayout;

    class RootLayout : public NonClientLayout {
    public:
        RootLayout(Window* w);
        ~RootLayout();

        void addShade(View* shade);
        void removeShade(View* shade);

        void addDebugView();
        void removeDebugView();
        void toggleDebugView();

        DebugView* getDebugView() const;

        void addContent(View* content);

        void requestLayout() override;
        View* findViewById(int id) const override;

    protected:
        LayoutParams* generateLayoutParams(const LayoutParams &lp) override;
        LayoutParams* generateDefaultLayoutParams() override;
        bool checkLayoutParams(LayoutParams* lp) override;

    private:
        DebugView* debug_view_;
        FrameLayout* shade_layout_;
        LinearLayout* content_layout_;

        bool shade_added_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_