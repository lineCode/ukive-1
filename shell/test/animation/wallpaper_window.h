#ifndef SHELL_TEST_ANIMATION_WALLPAPER_WINDOW_H_
#define SHELL_TEST_ANIMATION_WALLPAPER_WINDOW_H_

#define ROTATE_CIRCLE 0
#define VANISH_DOT 1
#define ALPHA_TEXT 2
#define ENABLE_ALPHA_TEXT 3
#define EXTEND_DASHED 4

#define RADIUS_INNER 10
#define RADIUS_OUTTER 11

#include "ukive/window/window.h"
#include "ukive/animation/animator.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"


namespace ukive {
    class Canvas;
}

namespace shell {

    class WallpaperWindow :
        public ukive::Window, public ukive::Animator::OnValueChangedListener
    {
    private:
        ID2D1Effect *mEffect;
        ID2D1SolidColorBrush *mCircleBrush;
        ID2D1RadialGradientBrush *mRadialBrush;

        float mInitTextWidth;
        float mThreeDotWidth;
        float mStandbyTextWidth;
        std::wstring mInitText;
        std::wstring mStandbyText;

        std::wstring mBottomText;
        IDWriteTextFormat* mBottomTextFormat;

        ukive::Animator *mAnimator;

        void createRadialGradientBrush();
        float getTextWidth(std::wstring text, IDWriteTextFormat* format);

        void drawInnerArc(ukive::Canvas *c, float cx, float cy, float radius);
        void drawDecoredCircle(
            ukive::Canvas *c, float cx, float cy, float radius,
            float extAngle, float extRadiusAdd, float strokeWidth, ukive::Color &color);
        void drawDashedLines(ukive::Canvas *c, float cx, float cy, float radius);

        void drawCenterCircle(ukive::Canvas *canvas, float radiusMinus);
        void drawDeformedCircle(
            ukive::Canvas *canvas, float radiusMinus, bool showOutline, float outlineWidth);
        void drawCursor(ukive::Canvas *canvas, bool split, float distance);
        void drawText(ukive::Canvas *canvas, std::wstring text, float alpha);

    public:
        using ukive::Window::Window;

        void onCreate() override;
        void onDraw(ukive::Canvas *canvas) override;
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
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_WINDOW_H_