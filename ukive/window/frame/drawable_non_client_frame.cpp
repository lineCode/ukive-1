#include "ukive/window/frame/drawable_non_client_frame.h"

#include <dwmapi.h>
#include <Windowsx.h>
#include <VersionHelpers.h>

#include "ukive/application.h"
#include "ukive/window/window_impl.h"
#include "ukive/log.h"
#include "ukive/utils/win10_version.h"


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
            int border_thickness = getBorderThickness();
            rect->left = border_thickness;
            rect->right = border_thickness;
            rect->top = border_thickness;
            rect->bottom = border_thickness;
        } else {
            rect->left = 0;
            rect->right = 0;
            rect->top = 0;
            // 用于抵消 onNcCalSize() 中的扩展。详见 onNcCalSize() 中的注释。
            static bool is_win10_1703_or_above = win::isWin10Ver1703OrGreater();
            if (is_win10_1703_or_above) {
                rect->bottom = (window_->isMaximum() ? 0 : 1);
            } else {
                rect->bottom = 0;
            }

        }
    }

    void DrawableNonClientFrame::getClientOffset(POINT* offset) {
        DCHECK(offset);
        if (window_->isTranslucent() && window_->isMaximum()) {
            int border_thickness = getBorderThickness();
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
            auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            if (window_->isTranslucent()) {
                // 半透明窗口，一切都需要自绘，因此直接移除整个非客户区。
                ncp->rgrc[0].left += 0;
                ncp->rgrc[0].top += 0;
                ncp->rgrc[0].right -= 0;
                ncp->rgrc[0].bottom -= 0;
            } else {
                if (window_->isMaximum()) {
                    int border_thickness = getBorderThickness();
                    ncp->rgrc[0].left += border_thickness;
                    ncp->rgrc[0].top += border_thickness;
                    ncp->rgrc[0].right -= border_thickness;
                    ncp->rgrc[0].bottom -= border_thickness;
                } else {
                    ncp->rgrc[0].left += 0;
                    ncp->rgrc[0].top += 0;
                    ncp->rgrc[0].right -= 0;
                    // 如果完全移除原生边框，则在调整窗口大小时，自绘边界会跳动，
                    // 因此让客户区在底部向外扩展 1 个像素，这样似乎可以保留边框，而且该扩展不可见。
                    // 在 Windows 10 1703 之前的系统上，无需扩展，扩展反而会有绘制问题（模糊）。
                    // 在 Windows 10 1703 及以上系统上，客户区会向外扩展 1 像素，注意在绘制时不要绘制这行扩展。
                    // 这里的扩展与 DwmExtendFrameIntoClientArea() 的设置无关，不会与其抵消。
                    static bool is_win10_1703_or_above = win::isWin10Ver1703OrGreater();
                    ncp->rgrc[0].bottom -= is_win10_1703_or_above ? -1 : 0;
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
            MARGINS margins = { 0, 0, 0, 1 };
            ::DwmExtendFrameIntoClientArea(window_->getHandle(), &margins);
            window_->sendFrameChanged();
        }

        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onActivateAfterDwm() {
        if (!window_->isTranslucent()) {
            // 让移除了标准边框的窗口能够显示原生阴影。
            // 在 Windows 7 上，该方法将会在窗口底部缩进一个像素，这一像素的横线具有毛玻璃效果；
            // 在 Windows 8 及以上系统，该方法将会在窗口底部缩进一个像素，这一像素是灰色的。
            // 这一像素是可以被覆盖的，如果覆盖的颜色完全不透明的话。
            MARGINS margins = { 0,0,0,1 };
            ::DwmExtendFrameIntoClientArea(window_->getHandle(), &margins);
        }

        return 0;
    }

    LRESULT DrawableNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }

    int DrawableNonClientFrame::getBorderThickness() const {
        int border_thickness;
        if (window_->isPopup()) {
            border_thickness = 0;
        } else {
            border_thickness = ::GetSystemMetrics(SM_CXSIZEFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER);
        }
        return border_thickness;
    }

}
