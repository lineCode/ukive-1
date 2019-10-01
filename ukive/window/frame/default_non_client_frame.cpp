#include "default_non_client_frame.h"

#include "ukive/log.h"
#include "ukive/window/window_impl.h"


namespace ukive {

    int DefaultNonClientFrame::onNcCreate(WindowImpl* w, bool* handled) {
        *handled = false;
        window_ = w;
        return TRUE;
    }

    int DefaultNonClientFrame::onNcDestroy(bool* handled) {
        *handled = false;
        window_ = nullptr;
        return FALSE;
    }

    void DefaultNonClientFrame::getClientInsets(RECT* rect) {
        DCHECK(rect);
        if (window_->isTranslucent()) {
            int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            rect->left = border_thickness;
            rect->right = border_thickness;
            rect->top = border_thickness;
            rect->bottom = border_thickness;
        } else {
            rect->left = 0;
            rect->right = 0;
            rect->top = 0;
            rect->bottom = 0;
        }
    }

    void DefaultNonClientFrame::getClientOffset(POINT* offset) {
        DCHECK(offset);
        if (window_->isTranslucent() && window_->isMaximum()) {
            int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            offset->x = border_thickness;
            offset->y = border_thickness;
        } else {
            offset->x = 0;
            offset->y = 0;
        }
    }

    LRESULT DefaultNonClientFrame::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcHitTest(
        WPARAM wParam, LPARAM lParam, bool* handled,
        bool* pass_to_window, POINT* p)
    {
        if (window_->isTranslucent()) {
            *handled = true;
            *pass_to_window = true;

            Point cp;
            cp.x = GET_X_LPARAM(lParam);
            cp.y = GET_Y_LPARAM(lParam);

            window_->convScreenToClient(&cp);

            p->x = cp.x;
            p->y = cp.y;
        } else {
            *handled = false;
            *pass_to_window = false;
        }

        return HTNOWHERE;
    }

    LRESULT DefaultNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (window_->isTranslucent()) {
            *handled = true;
            if (wParam == TRUE) {
                // 直接移除整个非客户区。
                auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                ncp->rgrc[0].left += 0;
                ncp->rgrc[0].top += 0;
                ncp->rgrc[0].right -= 0;
                ncp->rgrc[0].bottom -= 0;
            }
            return 0;
        }

        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onDwmCompositionChanged(bool* handled) {
        *handled = false;
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = false;
        return FALSE;
    }

}
