#ifndef SHELL_TEST_SHADOW_SHADOW_WINDOW_H_
#define SHELL_TEST_SHADOW_SHADOW_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/animation/animator.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/system/com_ptr.hpp"


namespace ukive {
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

        ukive::ShadowEffect* d3d_effect_;
        ukive::ComPtr<ID2D1Bitmap> shadow_bmp_;
        ukive::ComPtr<ID2D1Bitmap> content_bmp_;
        ukive::ComPtr<ID3D11Texture2D> d3d_tex2d_;
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_WINDOW_H_