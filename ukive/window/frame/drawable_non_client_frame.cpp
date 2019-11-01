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
            // ���ڵ��� onNcCalSize() �е���չ����� onNcCalSize() �е�ע�͡�
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
                // ��͸�����ڣ�һ�ж���Ҫ�Ի棬���ֱ���Ƴ������ǿͻ�����
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
                    // �����ȫ�Ƴ�ԭ���߿����ڵ������ڴ�Сʱ���Ի�߽��������
                    // ����ÿͻ����ڵײ�������չ 1 �����أ������ƺ����Ա����߿򣬶��Ҹ���չ���ɼ���
                    // �� Windows 10 1703 ֮ǰ��ϵͳ�ϣ�������չ����չ�������л������⣨ģ������
                    // �� Windows 10 1703 ������ϵͳ�ϣ��ͻ�����������չ 1 ���أ�ע���ڻ���ʱ��Ҫ����������չ��
                    // �������չ�� DwmExtendFrameIntoClientArea() �������޹أ��������������
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
            // �л� Aero ʱ��������Ӱ����������ʾ������������ʾ������
            MARGINS margins = { 0, 0, 0, 1 };
            ::DwmExtendFrameIntoClientArea(window_->getHandle(), &margins);
            window_->sendFrameChanged();
        }

        return FALSE;
    }

    LRESULT DrawableNonClientFrame::onActivateAfterDwm() {
        if (!window_->isTranslucent()) {
            // ���Ƴ��˱�׼�߿�Ĵ����ܹ���ʾԭ����Ӱ��
            // �� Windows 7 �ϣ��÷��������ڴ��ڵײ�����һ�����أ���һ���صĺ��߾���ë����Ч����
            // �� Windows 8 ������ϵͳ���÷��������ڴ��ڵײ�����һ�����أ���һ�����ǻ�ɫ�ġ�
            // ��һ�����ǿ��Ա����ǵģ�������ǵ���ɫ��ȫ��͸���Ļ���
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
