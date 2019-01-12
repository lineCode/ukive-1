#ifndef UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_H_
#define UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_H_

#include "ukive/views/layout/frame_layout.h"
#include "ukive/views/click_listener.h"
#include "ukive/window/window_listener.h"


namespace ukive {

    class Button;
    class TextView;

    class TitleBar :
        public FrameLayout,
        public OnClickListener,
        public OnWindowStatusChangedListener {
    public:
        explicit TitleBar(Window* w);
        ~TitleBar();

        // OnClickListener
        void onClick(View* v) override;

        // OnWindowStatusChangedListener
        void onWindowTextChanged(const string16& text) override;
        void onWindowIconChanged() override;
        void onWindowStatusChanged() override;

        HitPoint onNCHitTest(int x, int y);

    private:
        void initViews();
        Drawable* getTitleButtonBackground() const;

        TextView* title_tv_;
        Button* min_btn_;
        Button* max_btn_;
        Button* close_btn_;
    };

}

#endif  // UKIVE_VIEWS_TITLE_BAR_TITLE_BAR_H_