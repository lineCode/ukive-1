#ifndef UKIVE_WINDOW_FRAME_DEFAULT_NON_CLIENT_FRAME_H_
#define UKIVE_WINDOW_FRAME_DEFAULT_NON_CLIENT_FRAME_H_

#include <Windows.h>

#include "ukive/window/frame/non_client_frame.h"


namespace ukive {

    class Window;

    class DefaultNonClientFrame : public NonClientFrame {
    public:
        int onNcCreate(WindowImpl* w, bool* handled) override;
        int onNcDestroy(bool* handled) override;
        void onTranslucentChanged(bool translucent) override {}

        void getClientInsets(RECT* rect) override;

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
    };

}

#endif  // UKIVE_WINDOW_FRAME_DEFAULT_NON_CLIENT_FRAME_H_
