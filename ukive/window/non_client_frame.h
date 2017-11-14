#ifndef UKIVE_WINDOW_NON_CLIENT_FRAME_H_
#define UKIVE_WINDOW_NON_CLIENT_FRAME_H_

#include <Windows.h>


namespace ukive {

    class NonClientFrame {
    public:
        NonClientFrame() {}

        void init(HWND hWnd);

        LRESULT onSize(WPARAM wParam, LPARAM lParam);
        LRESULT onMouseMove(WPARAM wParam, LPARAM lParam);
        LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);

        LRESULT onNcPaint(WPARAM wParam, LPARAM lParam);
        LRESULT onNcActivate(WPARAM wParam, LPARAM lParam);
        LRESULT onNcHitTest(WPARAM wParam, LPARAM lParam);
        LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam);
        LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam);
        LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam);
        LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam);

    private:
        void drawCaptionAndBorder();
        void createCaptionButtonRgn(int rightOfWin, int topPadding, int rightPadding);

        HWND hWnd_;

        HPEN mFramePen;
        HRGN mWindowRgn;
        HFONT mTitleFont;

        HRGN mMinButtonRgn;
        HRGN mMaxButtonRgn;
        HRGN mCloseButtonRgn;

        HBRUSH mFrameBrush;

        RECT mTitleRect;

        COLORREF mMinButtonColor;
        COLORREF mMaxButtonColor;
        COLORREF mCloseButtonColor;

        bool mIsMousePressedInMinButton;
        bool mIsMousePressedInMaxButton;
        bool mIsMousePressedInCloseButton;
    };

}

#endif  // UKIVE_WINDOW_NON_CLIENT_FRAME_H_
