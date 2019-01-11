#include "ukive/window/frame/drawable_non_client_frame.h"

#include <dwmapi.h>
#include <Windowsx.h>

#include "ukive/application.h"
#include "ukive/window/window_impl.h"


namespace ukive {

    DrawableNonClientFrame::DrawableNonClientFrame()
        : window_(nullptr),
          vanish_captain_(false) { }


    int DrawableNonClientFrame::onNcCreate(WindowImpl* w, bool* handled) {
        *handled = false;
        window_ = w;

        // ĳЩ�������Ҫ�Ƴ� WS_CAPTION ���������������Ͻǵ�Բ��
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
        // �� Aero ������ʱ����Ҫ�Ƴ� WS_CAPTION �Ա��ⴰ�������Ͻǳ���Բ�ǣ��� WM_DWMCOMPOSITIONCHANGED
        // �Ĵ���ʱ�������û����л� Aero ʱ�����л��ῴ��ԭ���߿�����ֱ�������ϵͳ��Ҫ���Ʒǿͻ���ʱ�ж�
        // Aero �Ƿ��ѽ��ã�����ǣ����Ƴ��� WS_CAPTION��WM_DWMCOMPOSITIONCHANGED ����Ͳ����ˡ�
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

        // �� Aero �����ã����Ҵ��ڴ��ڷǼ���״̬ʱ��ԭ���߿��¶������
        // �����Ҫ�ڴ�ʱ���ص�����Ϣ��
        *handled = !Application::isAeroEnabled();
        return TRUE;
    }

    LRESULT DrawableNonClientFrame::onNcHitTest(
        WPARAM wParam, LPARAM lParam, bool* handled, bool* pass_to_window, POINT* p)
    {
        *handled = true;
        *pass_to_window = true;

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        RECT win_rect;
        ::GetWindowRect(window_->getHandle(), &win_rect);

        x -= win_rect.left;
        y -= win_rect.top;

        p->x = x;
        p->y = y;

        return 0;
    }

    LRESULT DrawableNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        if (wParam == TRUE) {
            // ֱ���Ƴ������ǿͻ�����
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

    LRESULT DrawableNonClientFrame::onDwmCompositionChanged(bool* handled) {
        *handled = false;

        if (!window_->isTranslucent() && Application::isAeroEnabled()) {
            window_->setWindowStyle(WS_CAPTION, false, true);
            // �л� Aero ʱ��������Ӱ����������ʾ������������ʾ������
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
