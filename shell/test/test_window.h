#ifndef SHELL_TEST_TEST_WINDOW_H_
#define SHELL_TEST_TEST_WINDOW_H_

#include "ukive/views/click_listener.h"
#include "ukive/window/window.h"
#include "ukive/animation/animator.h"
#include "ukive/animation/animator2.h"


namespace ukive {
    class Button;
    class ImageView;
}

namespace shell {

    class TestWindow : public ukive::Window, public ukive::OnClickListener, public ukive::AnimationListener, public ukive::Animator::OnValueChangedListener {
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
        void onAnimationProgress(ukive::Animator2* animator) override;

        void onValueChanged(
            unsigned varIndex,
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            double newValue, double previousValue) override;

    private:
        void inflateGroup();
        void inflateListView();

        ukive::Animator* animator1_;
        ukive::Animator2 animator_;

        ukive::Button* dwm_button_;
        ukive::ImageView* image_view_;
    };

}

#endif  // SHELL_TEST_TEST_WINDOW_H_