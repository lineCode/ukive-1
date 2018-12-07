#include "ukive/window/frame/drawable_non_client_frame.h"

#include <Windowsx.h>

#include "ukive/window/window.h"
#include "ukive/views/layout/root_layout.h"


namespace ukive {

    DrawableNonClientFrame::DrawableNonClientFrame()
        : window_(nullptr) {
    }


    int DrawableNonClientFrame::onNcCreate(ukive::Window* w, bool* handled) {
        *handled = false;

        window_ = w;
        return TRUE;
    }

    int DrawableNonClientFrame::onNcDestroy(bool* handled) {
        *handled = false;

        window_ = nullptr;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return 0;
    }

    LRESULT DrawableNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onNcHitTest(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        RECT win_rect;
        ::GetWindowRect(window_->getHandle(), &win_rect);

        x -= win_rect.left;
        y -= win_rect.top;

        return window_->getRootLayout()->nonClientHitTest(x, y);
    }

    LRESULT DrawableNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        if (wParam == TRUE) {
            auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            ncp->rgrc[0].left += 0;
            ncp->rgrc[0].top += 0;
            ncp->rgrc[0].right -= 0;
            ncp->rgrc[0].bottom -= 0;
        }

        return 0;
    }

    LRESULT DrawableNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }

}