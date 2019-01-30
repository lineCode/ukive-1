#include "ukive/window/frame/drawable_non_client_frame.h"

#include <dwmapi.h>
#include <Windowsx.h>

#include "ukive/application.h"
#include "ukive/window/window_impl.h"
#include "ukive/log.h"


namespace ukive {

    DrawableNonClientFrame::DrawableNonClientFrame()
        : window_(nullptr),
          vanish_captain_(false) { }

    int DrawableNonClientFrame::onNcCreate(WindowImpl* w, bool* handled) {
        *handled = false;
        window_ = w;

        // 某些情况下需要移除 WS_CAPTION 以消除窗口左右上角的圆角
        if (!window_->isTranslucent() && !Application::isAeroEnabled()) {
            window_->setWindowStyle(WS_CAPTION, false, false);
            window_->sendFrameChanged();
        }

        return TRUE;
    }

    int DrawableNonClientFrame::onNcDestroy(bool* handled) {
        *handled = false;

        window_ = nullptr;
        return FALSE;
    }

    void DrawableNonClientFrame::onTranslucentChanged(bool translucent) {
        if (translucent) {
            window_->setWindowStyle(WS_CAPTION, false, true);
        } else {
            if (!Application::isAeroEnabled()) {
                window_->setWindowStyle(WS_CAPTION, false, false);
            }
        }
    }

    void DrawableNonClientFrame::getClientInsets(RECT* rect) {
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
            rect->bottom = (window_->isMaximum() ? 0 : 1);
        }
    }

    void DrawableNonClientFrame::getClientOffset(POINT* offset) {
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
        *handled = !Application::isAeroEnabled();
        // 在 Aero 被禁用时，需要移除 WS_CAPTION 以避免窗口左右上角出现圆角，但 WM_DWMCOMPOSITIONCHANGED
        // 的触发时机较晚，用户在切换 Aero 时可能有机会看到原生边框。所以直接在这里，系统将要绘制非客户区时判断
        // Aero 是否已禁用，如果是，就移除掉 WS_CAPTION。WM_DWMCOMPOSITIONCHANGED 那里就不用了。
        if (!window_->isTranslucent() && !Application::isAeroEnabled()) {
            if (!vanish_captain_) {
                ::SetWindowLongPtr(
                    window_->getHandle(), GWL_STYLE,
                    ::GetWindowLongPtr(window_->getHandle(), GWL_STYLE) & ~WS_CAPTION);
                vanish_captain_ = true;
            }
        } else {
            vanish_captain_ = false;
        }
        return 0;
    }

    LRESULT DrawableNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (window_->isMinimum()) {
            *handled = false;
            return 0;
        }

        // 当 Aero 被禁用，并且窗口处于非激活状态时，原生边框会露出来，
        // 因此需要在此时拦截掉该消息。
        *handled = !Application::isAeroEnabled();
        return TRUE;
    }

    LRESULT DrawableNonClientFrame::onNcHitTest(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_window, POINT* p)
    {
        *handled = true;
        *pass_to_window = true;

        Point cp;
        cp.x = GET_X_LPARAM(lParam);
        cp.y = GET_Y_LPARAM(lParam);

        window_->convScreenToClient(&cp);

        p->x = cp.x;
        p->y = cp.y;

        return 0;
    }

    LRESULT DrawableNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        if (wParam == TRUE) {
            // 直接移除整个非客户区。
            auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            if (window_->isTranslucent()) {
                ncp->rgrc[0].left += 0;
                ncp->rgrc[0].top += 0;
                ncp->rgrc[0].right -= 0;
                ncp->rgrc[0].bottom -= 0;
            } else {
                if (window_->isMaximum()) {
                    int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
                    ncp->rgrc[0].left += border_thickness;
                    ncp->rgrc[0].top += border_thickness;
                    ncp->rgrc[0].right -= border_thickness;
                    ncp->rgrc[0].bottom -= border_thickness;
                } else {
                    ncp->rgrc[0].left += 0;
                    ncp->rgrc[0].top += 0;
                    ncp->rgrc[0].right -= 0;
                    ncp->rgrc[0].bottom -= -1;
                }
            }
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

    LRESULT DrawableNonClientFrame::onDwmCompositionChanged(bool* handled) {
        *handled = false;

        if (!window_->isTranslucent() && Application::isAeroEnabled()) {
            window_->setWindowStyle(WS_CAPTION, false, true);
            // 切回 Aero 时，窗口阴影不会立刻显示，这里让它显示出来。
            MARGINS margins = { 0,0,0,1 };
            ::DwmExtendFrameIntoClientArea(window_->getHandle(), &margins);
            window_->sendFrameChanged();
        }

        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }

}
