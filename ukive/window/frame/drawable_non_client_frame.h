#ifndef UKIVE_WINDOW_FRAME_DRAWABLE_NON_CLIENT_FRAME_H_
#define UKIVE_WINDOW_FRAME_DRAWABLE_NON_CLIENT_FRAME_H_

#include "ukive/window/frame/non_client_frame.h"


namespace ukive {

    class Window;

    class DrawableNonClientFrame : public NonClientFrame {
    public:
        DrawableNonClientFrame();

        int onNcCreate(Window* w, bool* handled) override;
        int onNcDestroy(bool* handled) override;

        LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;

        LRESULT onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcHitTest(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) override;
        LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) override;

    private:
        Window* window_;
    };

}

#endif  // UKIVE_WINDOW_FRAME_DRAWABLE_NON_CLIENT_FRAME_H_