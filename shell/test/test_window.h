#ifndef SHELL_TEST_TEST_WINDOW_H_
#define SHELL_TEST_TEST_WINDOW_H_

#include "ukive/views/click_listener.h"
#include "ukive/window/window.h"
#include "ukive/animation/animator.h"


namespace ukive {
    class Button;
    class ImageView;
    class CheckBox;
}

namespace shell {

    class TestWindow : public ukive::Window, public ukive::OnClickListener, public ukive::AnimationListener {
    public:
        TestWindow();
        ~TestWindow();

        void onPreCreate(
            ukive::ClassInfo* info,
            int* win_style, int* win_ex_style) override;
        void onCreate() override;
        void onDraw(const ukive::Rect& rect) override;
        void onDestroy() override;

        void onClick(ukive::View* v) override;

        // ukive::AnimationListener
        void onAnimationProgress(ukive::Animator* animator) override;

    private:
        void inflateGroup();
        void inflateListView();

        ukive::Animator animator_;

        ukive::Button* dwm_button_;
        ukive::ImageView* image_view_;
        ukive::CheckBox* check_box_;
    };

}

#endif  // SHELL_TEST_TEST_WINDOW_H_