#ifndef UKIVE_WINDOW_FRAME_DEFAULT_NON_CLIENT_FRAME_H_
#define UKIVE_WINDOW_FRAME_DEFAULT_NON_CLIENT_FRAME_H_

#include <Windows.h>

#include "ukive/window/frame/non_client_frame.h"


namespace ukive {

    class DefaultNonClientFrame : public NonClientFrame {
    public:
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
    };

}

#endif  // UKIVE_WINDOW_FRAME_DEFAULT_NON_CLIENT_FRAME_H_
