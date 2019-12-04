#ifndef SHELL_TEST_SHADOW_SHADOW_WINDOW_H_
#define SHELL_TEST_SHADOW_SHADOW_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/animation/animator.h"


namespace ukive {
    class Bitmap;
    class Button;
    class Canvas;
    class ShadowEffect;
}

namespace shell {

    class ShadowWindow :
        public ukive::Window, public ukive::AnimationListener {
    public:
        ShadowWindow();

        void onCreate() override;
        void onPreDrawCanvas(ukive::Canvas* canvas) override;
        void onDestroy() override;
        bool onInputEvent(ukive::InputEvent* e) override;

        // ukive::AnimationListener
        void onAnimationProgress(ukive::Animator* animator) override;

    private:
        ukive::Button* ce_button_;

        ukive::Animator animator_;

        std::shared_ptr<ukive::Bitmap> shadow_bmp_;
        std::shared_ptr<ukive::Bitmap> content_bmp_;
        std::unique_ptr<ukive::ShadowEffect> shadow_effect_;
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_WINDOW_H_