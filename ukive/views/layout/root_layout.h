#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_

#include "ukive/views/layout/non_client_layout.h"


namespace ukive {

    class Window;
    class DebugView;
    class LayoutParams;
    class FrameLayout;
    class LinearLayout;
    class TitleBar;

    class RootLayout : public NonClientLayout {
    public:
        explicit RootLayout(Window* w);
        ~RootLayout();

        void showTitleBar();
        void hideTitleBar();
        void removeTitleBar();

        void addShade(View* shade);
        void removeShade(View* shade);

        void showDebugView();
        void removeDebugView();
        void toggleDebugView();

        bool isTitleBarShowing() const;
        TitleBar* getTitleBar() const;
        DebugView* getDebugView() const;

        void setContent(View* content);

        void requestLayout() override;
        View* findViewById(int id) const override;
        HitPoint onNCHitTest(int x, int y) override;

    protected:
        LayoutParams* generateLayoutParams(const LayoutParams& lp) override;
        LayoutParams* generateDefaultLayoutParams() override;
        bool checkLayoutParams(LayoutParams* lp) override;

    private:
        TitleBar* title_bar_;
        DebugView* debug_view_;
        FrameLayout* shade_layout_;
        FrameLayout* content_layout_;
        View* content_view_;

        bool shade_added_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_