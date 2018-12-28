#include "custom_non_client_frame.h"

#include <Windowsx.h>

#include <cmath>

#include "ukive/application.h"
#include "ukive/window/window.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/canvas.h"
#include "ukive/log.h"


namespace shell {

    const int kMinButtonWidth = 15;
    const int kMinButtonHeight = 15;
    const int kMinButtonRightMargin = 6;

    const int kMaxButtonWidth = 15;
    const int kMaxButtonHeight = 15;
    const int kMaxButtonRightMargin = 6;

    const int kCloseButtonWidth = 15;
    const int kCloseButtonHeight = 15;
    const int kCloseButtonRightMargin = 12;

    const int kTitleLeftMargin = 8;
    const int kTitleWidth = 200;
    const int kCaptionHeight = 30;
    const int kLeftBorderWidth = 2;
    const int kTopBorderWidth = 2;
    const int kRightBorderWidth = 2;
    const int kBottomBorderWidth = 2;

    const int kResizeHandlerIndent = 2;

    const wchar_t kTitleText[] = L"Custom Non-client Frame";


    int CustomNonClientFrame::onNcCreate(ukive::Window* w, bool* handled) {
        window_ = w;
        *handled = true;

        dc_target_ = ukive::Renderer::createDCRenderTarget();
        text_format_ = ukive::Renderer::createTextFormat(L"微软雅黑", 15, L"zh-CN");
        text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        //resources used to drawing Non-client.
        min_button_color_ = kMinButtonColor;
        max_button_color_ = kMaxButtonColor;
        close_button_color_ = kCloseButtonColor;

        mIsMousePressedInMinButton = false;
        mIsMousePressedInMaxButton = false;
        mIsMousePressedInCloseButton = false;

        RECT rcWin;
        ::GetWindowRect(window_->getHandle(), &rcWin);
        ::OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

        createCaptionButtonRgn(rcWin.right, 0, 0);

        title_rect_.left = kTitleLeftMargin;
        title_rect_.top = 0;
        title_rect_.right = title_rect_.left + kTitleWidth;
        title_rect_.bottom = title_rect_.top + kCaptionHeight + kTopBorderWidth;

        return TRUE;
    }

    int CustomNonClientFrame::onNcDestroy(bool* handled) {
        return TRUE;
    }

    LRESULT CustomNonClientFrame::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        switch (wParam) {
        case SIZE_MAXIMIZED: {
            RECT rcWin;
            ::GetWindowRect(window_->getHandle(), &rcWin);

            int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME)
                + GetSystemMetrics(SM_CXPADDEDBORDER);

            int left_ext = border_thickness;
            int top_ext = border_thickness;
            int right_ext = border_thickness;
            int bottom_ext = border_thickness;

            int win_width = rcWin.right - rcWin.left;

            title_rect_.left = kTitleLeftMargin + left_ext - kLeftBorderWidth;
            title_rect_.top = top_ext - kTopBorderWidth;
            createCaptionButtonRgn(win_width, top_ext - kTopBorderWidth, right_ext - kRightBorderWidth);
            drawCaptionAndBorder();
            break;
        }

        case SIZE_RESTORED: {
            RECT rcWin;
            ::GetWindowRect(window_->getHandle(), &rcWin);
            ::OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

            title_rect_.left = kTitleLeftMargin;
            title_rect_.top = 0;
            createCaptionButtonRgn(rcWin.right, 0, 0);
            drawCaptionAndBorder();
            break;
        }

        case SIZE_MINIMIZED:
            break;

        default:
            drawCaptionAndBorder();
        }

        return FALSE;
    }

    LRESULT CustomNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (mIsMousePressedInMinButton
            || mIsMousePressedInMaxButton
            || mIsMousePressedInCloseButton)
        {
            RECT rcWin;
            ::GetWindowRect(window_->getHandle(), &rcWin);

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            POINT pt = { xPos, yPos };
            ::ClientToScreen(window_->getHandle(), &pt);
            xPos = pt.x;
            yPos = pt.y;

            bool needRedraw = false;
            if (mIsMousePressedInMinButton) {
                if (min_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top)) {
                    min_button_color_ = kMinButtonPressedColor;
                    needRedraw = true;
                } else {
                    min_button_color_ = kMinButtonColor;
                    needRedraw = true;
                }
            } else if (mIsMousePressedInMaxButton) {
                if (max_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top)) {
                    max_button_color_ = kMaxButtonPressedColor;
                    needRedraw = true;
                } else {
                    max_button_color_ = kMaxButtonColor;
                    needRedraw = true;
                }
            } else if (mIsMousePressedInCloseButton) {
                if (close_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top)) {
                    close_button_color_ = kCloseButtonPressedColor;
                    needRedraw = true;
                } else {
                    close_button_color_ = kCloseButtonColor;
                    needRedraw = true;
                }
            }

            if (needRedraw) {
                drawCaptionAndBorder();
            }
        }

        return TRUE;
    }

    LRESULT CustomNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (mIsMousePressedInMinButton
            || mIsMousePressedInMaxButton
            || mIsMousePressedInCloseButton)
        {
            RECT rcWin;
            ::GetWindowRect(window_->getHandle(), &rcWin);

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            POINT pt = { xPos, yPos };
            ::ClientToScreen(window_->getHandle(), &pt);
            xPos = pt.x;
            yPos = pt.y;

            if (mIsMousePressedInMinButton
                && min_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top))
            {
                ::ShowWindow(window_->getHandle(), SW_MINIMIZE);
                min_button_color_ = kMinButtonColor;
                *handled = true;
            } else if (mIsMousePressedInMaxButton
                && max_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top))
            {
                if (IsZoomed(window_->getHandle())) {
                    ::ShowWindow(window_->getHandle(), SW_NORMAL);
                } else {
                    ::ShowWindow(window_->getHandle(), SW_MAXIMIZE);
                }

                max_button_color_ = kMaxButtonColor;
                drawCaptionAndBorder();
                *handled = true;
            }
            else if (mIsMousePressedInCloseButton
                && close_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top))
            {
                window_->close();
                *handled = true;
            } else {
                *handled = false;
            }

            mIsMousePressedInMinButton = false;
            mIsMousePressedInMaxButton = false;
            mIsMousePressedInCloseButton = false;

            ReleaseCapture();
        }

        return TRUE;
    }

    LRESULT CustomNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        drawCaptionAndBorder();
        return FALSE;
    }

    LRESULT CustomNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        if (wParam == FALSE) {
            drawCaptionAndBorder();
        }
        return TRUE;
    }

    LRESULT CustomNonClientFrame::onNcHitTest(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;

        RECT rcWin;
        ::GetWindowRect(window_->getHandle(), &rcWin);

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        int row = 1, col = 1;
        bool isInCaptain = true;

        if (xPos >= rcWin.left
            && xPos < rcWin.left + kLeftBorderWidth + kResizeHandlerIndent)
        {
            col = 0;
        } else if (xPos >= rcWin.right - kRightBorderWidth - kResizeHandlerIndent
            && xPos < rcWin.right)
        {
            col = 2;
        }

        if (yPos >= rcWin.top && yPos < rcWin.top + kTopBorderWidth + kCaptionHeight)
        {
            isInCaptain = (yPos > rcWin.top + kTopBorderWidth + kResizeHandlerIndent);
            if (min_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top)) {
                return HTMINBUTTON;
            } else if (max_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top)) {
                return HTMAXBUTTON;
            } else if (close_button_rect_.hit(xPos - rcWin.left, yPos - rcWin.top)) {
                return HTCLOSE;
            } else if (xPos > rcWin.left + kLeftBorderWidth + kResizeHandlerIndent + 2
                && xPos <= rcWin.left + kLeftBorderWidth + kResizeHandlerIndent + 2 + 24
                && isInCaptain)
            {
                return HTSYSMENU;
            } else {
                min_button_color_ = kMinButtonColor;
                max_button_color_ = kMaxButtonColor;
                close_button_color_ = kCloseButtonColor;
                drawCaptionAndBorder();
            }

            if ((col == 0 || col == 2) && isInCaptain) {
                row = 1;
            } else {
                row = 0;
            }
        }
        else if (yPos >= rcWin.bottom - kBottomBorderWidth - kResizeHandlerIndent
            && yPos < rcWin.bottom)
        {
            row = 2;
        }

        LRESULT hitTests[3][3] =
        {
            { HTTOPLEFT,      isInCaptain ? HTCAPTION : HTTOP,   HTTOPRIGHT },
            { HTLEFT,         HTCLIENT,                          HTRIGHT },
            { HTBOTTOMLEFT,   HTBOTTOM,                          HTBOTTOMRIGHT },
        };

        return hitTests[row][col];
    }

    LRESULT CustomNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        if (wParam == TRUE)
        {
            auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            // 0: 新窗口位置
            // 1: 旧窗口位置
            // 2: 旧客户区位置
            if (::IsZoomed(window_->getHandle())) {
                int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME)
                    + GetSystemMetrics(SM_CXPADDEDBORDER);

                int left_ext = border_thickness;
                int top_ext = border_thickness;
                int right_ext = border_thickness;
                int bottom_ext = border_thickness;

                ncp->rgrc[0].left += left_ext;
                ncp->rgrc[0].top += kTopBorderWidth + kCaptionHeight;
                ncp->rgrc[0].right -= right_ext;
                ncp->rgrc[0].bottom -= bottom_ext;
            } else {
                ncp->rgrc[0].left += kLeftBorderWidth;
                ncp->rgrc[0].top += kTopBorderWidth + kCaptionHeight;
                ncp->rgrc[0].right -= kRightBorderWidth;
                ncp->rgrc[0].bottom -= kBottomBorderWidth;
            }

            // 0: 新客户区位置
            // 1: 目标？？？
            // 2: 源？？？
            return WVR_REDRAW;
        } else {
            // 新窗口位置
            RECT *rgrc = reinterpret_cast<RECT*>(lParam);
            rgrc->left += kLeftBorderWidth;
            rgrc->top += kTopBorderWidth + kCaptionHeight;
            rgrc->right -= kRightBorderWidth;
            rgrc->bottom -= kBottomBorderWidth;

            // 新客户区位置
            return FALSE;
        }
    }

    LRESULT CustomNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (wParam == HTMINBUTTON) {
            min_button_color_ = kMinButtonPressedColor;
            drawCaptionAndBorder();
            mIsMousePressedInMinButton = true;
            *handled = true;
        } else if (wParam == HTMAXBUTTON) {
            max_button_color_ = kMaxButtonPressedColor;
            drawCaptionAndBorder();
            mIsMousePressedInMaxButton = true;
            *handled = true;
        } else if (wParam == HTCLOSE) {
            close_button_color_ = kCloseButtonPressedColor;
            drawCaptionAndBorder();
            mIsMousePressedInCloseButton = true;
            *handled = true;
        } else {
            *handled = false;
        }

        if (*handled) {
            SetCapture(window_->getHandle());
        }

        return TRUE;
    }

    LRESULT CustomNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (wParam == HTMINBUTTON) {
            ::ShowWindow(window_->getHandle(), SW_MINIMIZE);
            min_button_color_ = kMinButtonColor;
            *handled = true;
        } else if (wParam == HTMAXBUTTON) {
            if (IsZoomed(window_->getHandle())) {
                ::ShowWindow(window_->getHandle(), SW_NORMAL);
            } else {
                ::ShowWindow(window_->getHandle(), SW_MAXIMIZE);
            }

            max_button_color_ = kMaxButtonColor;
            drawCaptionAndBorder();
            *handled = true;
        } else if (wParam == HTCLOSE) {
            window_->close();
            *handled = true;
        } else {
            *handled = false;
        }

        mIsMousePressedInMinButton = false;
        mIsMousePressedInMaxButton = false;
        mIsMousePressedInCloseButton = false;

        return TRUE;
    }

    LRESULT CustomNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }


    void CustomNonClientFrame::drawCaptionAndBorder() {
        HDC hdc = ::GetWindowDC(window_->getHandle());

        RECT rcWin;
        ::GetWindowRect(window_->getHandle(), &rcWin);
        ::OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

        HRESULT hr = dc_target_->BindDC(hdc, &rcWin);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to bind dc RT!";
            return;
        }

        dc_target_->BeginDraw();
        ukive::Canvas canvas(dc_target_.cast<ID2D1RenderTarget>());

        ukive::RectF frame[4];

        // Top
        frame[0] = ukive::RectF(
            0, 0, rcWin.right - rcWin.left, kCaptionHeight + kTopBorderWidth);
        // Left
        frame[1] = ukive::RectF(
            0, kCaptionHeight + kTopBorderWidth,
            kLeftBorderWidth, rcWin.bottom - rcWin.top - kCaptionHeight + kTopBorderWidth);
        // Right
        frame[2] = ukive::RectF(
            rcWin.right - kRightBorderWidth, kCaptionHeight + kTopBorderWidth,
            kRightBorderWidth, rcWin.bottom - rcWin.top - kCaptionHeight + kTopBorderWidth);
        // Bottom
        frame[3] = ukive::RectF(
            0, rcWin.bottom - kBottomBorderWidth,
            rcWin.right - rcWin.left, kBottomBorderWidth);

        for (int i = 0; i < 4; ++i) {
            canvas.fillRect(frame[i], kBorderColor);
        }

        // Title
        canvas.drawText(kTitleText, text_format_.get(), title_rect_, kTitleColor);

        canvas.fillCircle(min_button_rect_, min_button_color_);
        canvas.fillCircle(max_button_rect_, max_button_color_);
        canvas.fillCircle(close_button_rect_, close_button_color_);

        hr = dc_target_->EndDraw();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to end draw!";
            return;
        }

        ::ReleaseDC(window_->getHandle(), hdc);
    }

    void CustomNonClientFrame::createCaptionButtonRgn(int rightOfWin, int topPadding, int rightPadding)
    {
        close_button_rect_ = ukive::RectF(
            rightOfWin - kCloseButtonRightMargin - kCloseButtonWidth - rightPadding,
            static_cast<int>(std::ceil((kCaptionHeight + kTopBorderWidth + topPadding - kCloseButtonHeight) / 2.f)),
            kCloseButtonWidth, kCloseButtonHeight
        );

        max_button_rect_ = ukive::RectF(
            close_button_rect_.left - kMaxButtonRightMargin - kMaxButtonWidth,
            static_cast<int>(std::ceil((kCaptionHeight + kTopBorderWidth + topPadding - kMaxButtonHeight) / 2.f)),
            kMaxButtonWidth, kMaxButtonHeight
        );

        min_button_rect_ = ukive::RectF(
            max_button_rect_.left - kMinButtonRightMargin - kMinButtonWidth,
            static_cast<int>(std::ceil((kCaptionHeight + kTopBorderWidth + topPadding - kMinButtonHeight) / 2.f)),
            kMinButtonWidth, kMinButtonHeight
        );
    }

}