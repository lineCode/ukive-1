#ifndef SHELL_TEST_FRAME_CUSTOM_NON_CLIENT_FRAME_H_
#define SHELL_TEST_FRAME_CUSTOM_NON_CLIENT_FRAME_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include "ukive/window/frame/non_client_frame.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/graphics/rect.h"
#include "ukive/graphics/color.h"


namespace ukive {
    class WindowImpl;
}

namespace shell {

    class CustomNonClientFrame : public ukive::NonClientFrame {
    public:
        CustomNonClientFrame();

        int onNcCreate(ukive::WindowImpl* w, bool* handled) override;
        int onNcDestroy(bool* handled) override;
        void onTranslucentChanged(bool translucent) override;

        void getClientInsets(RECT* rect) override;
        void getClientOffset(POINT* offset) override;

        LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;

        LRESULT onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcHitTest(
            WPARAM wParam, LPARAM lParam, bool* handled,
            bool* pass_to_window, POINT* p) override;
        LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onDwmCompositionChanged(bool* handled) override;
        LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) override;

    private:
        ukive::Color kTitleColor;
        ukive::Color kBorderColor;
        ukive::Color kMinButtonColor;
        ukive::Color kMinButtonPressedColor;
        ukive::Color kMaxButtonColor;
        ukive::Color kMaxButtonPressedColor;
        ukive::Color kCloseButtonColor;
        ukive::Color kCloseButtonPressedColor;

        void drawCaptionAndBorder();
        void createCaptionButtonRgn(int rightOfWin, int topPadding, int rightPadding);

        ukive::WindowImpl* window_;

        ukive::ComPtr<ID2D1DCRenderTarget> dc_target_;
        ukive::ComPtr<IDWriteTextFormat> text_format_;

        ukive::RectF min_button_rect_;
        ukive::RectF max_button_rect_;
        ukive::RectF close_button_rect_;

        ukive::RectF title_rect_;

        ukive::Color min_button_color_;
        ukive::Color max_button_color_;
        ukive::Color close_button_color_;

        bool mIsMousePressedInMinButton;
        bool mIsMousePressedInMaxButton;
        bool mIsMousePressedInCloseButton;
    };

}

#endif  // SHELL_TEST_FRAME_CUSTOM_NON_CLIENT_FRAME_H_
