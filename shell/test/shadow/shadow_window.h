#ifndef SHELL_TEST_SHADOW_SHADOW_WINDOW_H_
#define SHELL_TEST_SHADOW_SHADOW_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/animation/animator.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/com_ptr.h"


namespace ukive {
    class Button;
    class Canvas;
    class ShadowEffect;
}

namespace shell {

    class ShadowWindow :
        public ukive::Window, public ukive::Animator::OnValueChangedListener
    {
    public:
        using ukive::Window::Window;

        void onCreate() override;
        void onDrawCanvas(ukive::Canvas *canvas) override;
        void onDestroy() override;
        bool onInputEvent(ukive::InputEvent *e) override;

        void onValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard *storyboard,
            IUIAnimationVariable *variable,
            double newValue, double previousValue) override;
        void onIntegerValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard *storyboard,
            IUIAnimationVariable *variable,
            int newValue, int previousValue) override;

    private:
        ukive::Button* de_button_;
        ukive::Button* ce_button_;

        ukive::Animator* animator_;
        ukive::ComPtr<ID2D1Effect> effect_;

        ukive::ShadowEffect* d3d_effect_;
        ukive::ComPtr<ID2D1Bitmap> d3d_content_;
        ukive::ComPtr<ID3D11Texture2D> d3d_tex2d_;
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_WINDOW_H_