#include "default_non_client_frame.h"


namespace ukive {

    LRESULT DefaultNonClientFrame::onSize(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcHitTest(WPARAM wParam, LPARAM lParam) {
        return HTNOWHERE;
    }

    LRESULT DefaultNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

    LRESULT DefaultNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam) {
        return FALSE;
    }

}