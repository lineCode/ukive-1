#ifndef SHELL_TEST_TEST_WINDOW_H_
#define SHELL_TEST_TEST_WINDOW_H_

#include "ukive/views/click_listener.h"
#include "ukive/window/window.h"
#include "ukive/animation/animator.h"


namespace ukive {
    class Button;
    class ImageView;
    class CheckBox;
    class SpinnerView;
}

namespace shell {

    class TestWindow :
        public ukive::Window,
        public ukive::OnClickListener,
        public ukive::AnimationListener {
    public:
        TestWindow();
        ~TestWindow();

        // ukive::Window
        void onCreate() override;
        void onDraw(const ukive::Rect& rect) override;
        void onDestroy() override;
        bool onGetWindowIconName(string16* icon_name, string16* small_icon_name) const override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

        // ukive::AnimationListener
        void onAnimationProgress(ukive::Animator* animator) override;

    private:
        void inflateGroup();
        void inflateListView();

        ukive::Animator animator_;

        ukive::Button* test_button_ = nullptr;
        ukive::Button* dwm_button_ = nullptr;
        ukive::ImageView* image_view_ = nullptr;
        ukive::CheckBox* check_box_ = nullptr;
        ukive::SpinnerView* spinner_view_ = nullptr;
    };

}

#endif  // SHELL_TEST_TEST_WINDOW_H_