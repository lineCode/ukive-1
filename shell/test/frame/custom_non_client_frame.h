#ifndef SHELL_TEST_FRAME_CUSTOM_NON_CLIENT_FRAME_H_
#define SHELL_TEST_FRAME_CUSTOM_NON_CLIENT_FRAME_H_

#include <Windows.h>
#include <gdiplus.h>

#include "ukive/window/frame/non_client_frame.h"


namespace shell {

    class CustomNonClientFrame : public ukive::NonClientFrame {
    public:
        CustomNonClientFrame()
            :title_font_(NULL),
            brush_(0) {}

        void init(HWND hWnd);

        LRESULT onSize(WPARAM wParam, LPARAM lParam) override;
        LRESULT onMouseMove(WPARAM wParam, LPARAM lParam) override;
        LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam) override;

        LRESULT onNcPaint(WPARAM wParam, LPARAM lParam) override;
        LRESULT onNcActivate(WPARAM wParam, LPARAM lParam) override;
        LRESULT onNcHitTest(WPARAM wParam, LPARAM lParam) override;
        LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam) override;
        LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam) override;
        LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam) override;
        LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam) override;

    private:
        void drawCaptionAndBorder();
        void createCaptionButtonRgn(int rightOfWin, int topPadding, int rightPadding);

        HWND hWnd_;
        HRGN mWindowRgn;

        Gdiplus::Rect min_button_rect_;
        Gdiplus::Rect max_button_rect_;
        Gdiplus::Rect close_button_rect_;

        Gdiplus::Font title_font_;
        Gdiplus::RectF title_rect_;
        Gdiplus::SolidBrush brush_;
        Gdiplus::StringFormat string_format_;

        Gdiplus::Color min_button_color_;
        Gdiplus::Color max_button_color_;
        Gdiplus::Color close_button_color_;

        bool mIsMousePressedInMinButton;
        bool mIsMousePressedInMaxButton;
        bool mIsMousePressedInCloseButton;
    };

}

#endif  // SHELL_TEST_FRAME_CUSTOM_NON_CLIENT_FRAME_H_
