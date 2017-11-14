#include "non_client_frame.h"

#include <Windowsx.h>

#include <cmath>

#define MIN_BUTTON_WIDTH 15
#define MIN_BUTTON_HEIGHT 15
#define MIN_BUTTON_RIGHT_MARGIN 6

#define MAX_BUTTON_WIDTH 15
#define MAX_BUTTON_HEIGHT 15
#define MAX_BUTTON_RIGHT_MARGIN 6

#define CLOSE_BUTTON_WIDTH 15
#define CLOSE_BUTTON_HEIGHT 15
#define CLOSE_BUTTON_RIGHT_MARGIN 12

#define TITLE_LEFT_MARGIN 8
#define TITLE_WIDTH 100

#define CAPTION_HEIGHT 30
#define LEFT_BORDER_WIDTH 2
#define RIGHT_BORDER_WIDTH 2
#define TOP_BORDER_WIDTH 2
#define BOTTOM_BORDER_WIDTH 2

#define WINDOW_CORNER_RADIUS 4
#define RESIZE_HANDLE_INDENT 2

#define TITLE_TEXT L"WLTest"
#define TITLE_COLOR 0xffffff
#define BORDER_COLOR   0x231ce5              //Red 500

#define MIN_BUTTON_COLOR           0x41bd42  //Green 300
#define MIN_BUTTON_PRESSED_COLOR   0x088f0a  //Green 600

#define MAX_BUTTON_COLOR           0x35d8fd  //Yellow 600
#define MAX_BUTTON_PRESSED_COLOR   0x177ff5  //Yellow 900

#define CLOSE_BUTTON_COLOR         0x4db7ff  //Orange 300
#define CLOSE_BUTTON_PRESSED_COLOR 0x008cfb  //Orange 600


namespace ukive {

    void NonClientFrame::init(HWND hWnd) {
        hWnd_ = hWnd;

        //resources used to drawing Non-client.
        mFramePen = CreatePen(PS_SOLID, 0, BORDER_COLOR);
        mFrameBrush = CreateSolidBrush(BORDER_COLOR);

        mMinButtonColor = MIN_BUTTON_COLOR;
        mMaxButtonColor = MAX_BUTTON_COLOR;
        mCloseButtonColor = CLOSE_BUTTON_COLOR;

        mIsMousePressedInMinButton = false;
        mIsMousePressedInMaxButton = false;
        mIsMousePressedInCloseButton = false;

        HDC dc = GetDC(hWnd_);
        int fontHeight = -MulDiv(10, GetDeviceCaps(dc, LOGPIXELSY), 72);
        ReleaseDC(hWnd_, dc);
        mTitleFont = CreateFontW(fontHeight, 0,
            0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH, L"Î¢ÈíÑÅºÚ");

        RECT rcWin;
        GetWindowRect(hWnd_, &rcWin);
        OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

        createCaptionButtonRgn(rcWin.right, 0, 0);

        mWindowRgn = CreateRoundRectRgn(
            0, 0,
            rcWin.right + 1, rcWin.bottom + 1,
            WINDOW_CORNER_RADIUS, WINDOW_CORNER_RADIUS);
        SetWindowRgn(hWnd_, mWindowRgn, TRUE);

        mTitleRect.left = TITLE_LEFT_MARGIN;
        mTitleRect.top = 0;
        mTitleRect.right = TITLE_WIDTH;
        mTitleRect.bottom = CAPTION_HEIGHT + TOP_BORDER_WIDTH;
    }


    LRESULT NonClientFrame::onSize(WPARAM wParam, LPARAM lParam) {
        DeleteObject(mMinButtonRgn);
        DeleteObject(mMaxButtonRgn);
        DeleteObject(mCloseButtonRgn);

        switch (wParam)
        {
        case SIZE_MAXIMIZED:
        {
            RECT rcWin;
            GetWindowRect(hWnd_, &rcWin);

            int leftExtended = -rcWin.left;
            int topExtended = -rcWin.top;
            int rightExtended = rcWin.right - GetSystemMetrics(SM_CXFULLSCREEN);
            int bottomExtended = rcWin.bottom - (GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION));

            OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

            mWindowRgn = CreateRoundRectRgn(
                leftExtended, topExtended,
                rcWin.right - rightExtended + 1,
                rcWin.bottom - bottomExtended + 1, 0, 0);
            SetWindowRgn(hWnd_, mWindowRgn, TRUE);

            mTitleRect.left = TITLE_LEFT_MARGIN + leftExtended - LEFT_BORDER_WIDTH;
            mTitleRect.top = topExtended - TOP_BORDER_WIDTH;
            createCaptionButtonRgn(rcWin.right, topExtended - TOP_BORDER_WIDTH, rightExtended - RIGHT_BORDER_WIDTH);
            drawCaptionAndBorder();
            break;
        }

        case SIZE_RESTORED:
        {
            RECT rcWin;
            GetWindowRect(hWnd_, &rcWin);
            OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

            mWindowRgn = CreateRoundRectRgn(
                0, 0,
                rcWin.right + 1, rcWin.bottom + 1,
                WINDOW_CORNER_RADIUS, WINDOW_CORNER_RADIUS);
            SetWindowRgn(hWnd_, mWindowRgn, TRUE);

            mTitleRect.left = TITLE_LEFT_MARGIN;
            mTitleRect.top = 0;
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

    LRESULT NonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam) {
        if (mIsMousePressedInMinButton
            || mIsMousePressedInMaxButton
            || mIsMousePressedInCloseButton)
        {
            RECT rcWin;
            GetWindowRect(hWnd_, &rcWin);

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            POINT pt = { xPos, yPos };
            ClientToScreen(hWnd_, &pt);
            xPos = pt.x;
            yPos = pt.y;

            bool needRedraw = false;
            if (mIsMousePressedInMinButton)
            {
                if (PtInRegion(mMinButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
                {
                    if (mMinButtonColor != MIN_BUTTON_PRESSED_COLOR)
                    {
                        mMinButtonColor = MIN_BUTTON_PRESSED_COLOR;
                        needRedraw = true;
                    }
                }
                else
                {
                    if (mMinButtonColor != MIN_BUTTON_COLOR)
                    {
                        mMinButtonColor = MIN_BUTTON_COLOR;
                        needRedraw = true;
                    }
                }
            }
            else if (mIsMousePressedInMaxButton)
            {
                if (PtInRegion(mMaxButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
                {
                    if (mMaxButtonColor != MAX_BUTTON_PRESSED_COLOR)
                    {
                        mMaxButtonColor = MAX_BUTTON_PRESSED_COLOR;
                        needRedraw = true;
                    }
                }
                else
                {
                    if (mMaxButtonColor != MAX_BUTTON_COLOR)
                    {
                        mMaxButtonColor = MAX_BUTTON_COLOR;
                        needRedraw = true;
                    }
                }
            }
            else if (mIsMousePressedInCloseButton)
            {
                if (PtInRegion(mCloseButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
                {
                    if (mCloseButtonColor != CLOSE_BUTTON_PRESSED_COLOR)
                    {
                        mCloseButtonColor = CLOSE_BUTTON_PRESSED_COLOR;
                        needRedraw = true;
                    }
                }
                else
                {
                    if (mCloseButtonColor != CLOSE_BUTTON_COLOR)
                    {
                        mCloseButtonColor = CLOSE_BUTTON_COLOR;
                        needRedraw = true;
                    }
                }
            }

            if (needRedraw)
                drawCaptionAndBorder();
        }

        return TRUE;
    }

    LRESULT NonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam) {
        BOOL handled = FALSE;

        if (mIsMousePressedInMinButton
            || mIsMousePressedInMaxButton
            || mIsMousePressedInCloseButton)
        {
            RECT rcWin;
            GetWindowRect(hWnd_, &rcWin);

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            POINT pt = { xPos, yPos };
            ClientToScreen(hWnd_, &pt);
            xPos = pt.x;
            yPos = pt.y;

            if (mIsMousePressedInMinButton
                && PtInRegion(mMinButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
            {
                ShowWindow(hWnd_, SW_MINIMIZE);
                mMinButtonColor = MIN_BUTTON_COLOR;
                handled = TRUE;
            }
            else if (mIsMousePressedInMaxButton
                && PtInRegion(mMaxButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
            {
                if (IsZoomed(hWnd_))
                    ShowWindow(hWnd_, SW_NORMAL);
                else
                    ShowWindow(hWnd_, SW_MAXIMIZE);

                mMaxButtonColor = MAX_BUTTON_COLOR;
                drawCaptionAndBorder();
                handled = TRUE;
            }
            else if (mIsMousePressedInCloseButton
                && PtInRegion(mCloseButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
            {
                //CloseDialog(0);
                handled = TRUE;
            }
            else
                handled = FALSE;

            mIsMousePressedInMinButton = false;
            mIsMousePressedInMaxButton = false;
            mIsMousePressedInCloseButton = false;

            ReleaseCapture();
        }

        return handled;
    }

    LRESULT NonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam) {
        drawCaptionAndBorder();
        return TRUE;
    }

    LRESULT NonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam) {
        if (wParam == FALSE)
            drawCaptionAndBorder();
        return TRUE;
    }

    LRESULT NonClientFrame::onNcHitTest(WPARAM wParam, LPARAM lParam) {
        RECT rcWin;
        GetWindowRect(hWnd_, &rcWin);

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        int row = 1, col = 1;
        bool isInCaptain = true;

        if (xPos >= rcWin.left
            && xPos < rcWin.left + LEFT_BORDER_WIDTH + RESIZE_HANDLE_INDENT)
        {
            col = 0;
        }
        else if (xPos >= rcWin.right - RIGHT_BORDER_WIDTH - RESIZE_HANDLE_INDENT
            && xPos < rcWin.right)
        {
            col = 2;
        }

        if (yPos >= rcWin.top && yPos < rcWin.top + TOP_BORDER_WIDTH + CAPTION_HEIGHT)
        {
            isInCaptain = (yPos > rcWin.top + TOP_BORDER_WIDTH + RESIZE_HANDLE_INDENT);
            if (PtInRegion(mMinButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
            {
                return HTMINBUTTON;
            }
            else if (PtInRegion(mMaxButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
            {
                return HTMAXBUTTON;
            }
            else if (PtInRegion(mCloseButtonRgn, xPos - rcWin.left, yPos - rcWin.top))
            {
                return HTCLOSE;
            }
            else if (xPos > rcWin.left + LEFT_BORDER_WIDTH + RESIZE_HANDLE_INDENT + 2
                && xPos <= rcWin.left + LEFT_BORDER_WIDTH + RESIZE_HANDLE_INDENT + 2 + 24
                && isInCaptain)
            {
                return HTSYSMENU;
            }
            else
            {
                bool needRedraw = false;
                if (mMinButtonColor != MIN_BUTTON_COLOR)
                {
                    mMinButtonColor = MIN_BUTTON_COLOR;
                    needRedraw = true;
                }

                if (mMaxButtonColor != MAX_BUTTON_COLOR)
                {
                    mMaxButtonColor = MAX_BUTTON_COLOR;
                    needRedraw = true;
                }

                if (mCloseButtonColor != CLOSE_BUTTON_COLOR)
                {
                    mCloseButtonColor = CLOSE_BUTTON_COLOR;
                    needRedraw = true;
                }

                if (needRedraw)
                    drawCaptionAndBorder();
            }

            if ((col == 0 || col == 2) && isInCaptain)
                row = 1;
            else
                row = 0;
        }
        else if (yPos >= rcWin.bottom - BOTTOM_BORDER_WIDTH - RESIZE_HANDLE_INDENT
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

    LRESULT NonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam) {
        if (wParam == TRUE)
        {
            NCCALCSIZE_PARAMS *ncp = (NCCALCSIZE_PARAMS*)lParam;
            if (IsZoomed(hWnd_))
            {
                RECT rcWin;
                GetWindowRect(hWnd_, &rcWin);

                int leftExtended = -rcWin.left;
                int topExtended = -rcWin.top;
                int rightExtended = rcWin.right - GetSystemMetrics(SM_CXFULLSCREEN);
                int bottomExtended = rcWin.bottom - (GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION));

                ncp->rgrc[0].left += leftExtended;
                ncp->rgrc[0].top += TOP_BORDER_WIDTH + CAPTION_HEIGHT;
                ncp->rgrc[0].right -= rightExtended;
                ncp->rgrc[0].bottom -= bottomExtended;
            }
            else
            {
                ncp->rgrc[0].left += LEFT_BORDER_WIDTH;
                ncp->rgrc[0].top += TOP_BORDER_WIDTH + CAPTION_HEIGHT;
                ncp->rgrc[0].right -= RIGHT_BORDER_WIDTH;
                ncp->rgrc[0].bottom -= BOTTOM_BORDER_WIDTH;
            }

            return TRUE;
        }

        return FALSE;
    }

    LRESULT NonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam) {
        BOOL handled = FALSE;

        if (wParam == HTMINBUTTON)
        {
            mMinButtonColor = MIN_BUTTON_PRESSED_COLOR;
            drawCaptionAndBorder();
            mIsMousePressedInMinButton = true;
            handled = TRUE;
        }
        else if (wParam == HTMAXBUTTON)
        {
            mMaxButtonColor = MAX_BUTTON_PRESSED_COLOR;
            drawCaptionAndBorder();
            mIsMousePressedInMaxButton = true;
            handled = TRUE;
        }
        else if (wParam == HTCLOSE)
        {
            mCloseButtonColor = CLOSE_BUTTON_PRESSED_COLOR;
            drawCaptionAndBorder();
            mIsMousePressedInCloseButton = true;
            handled = TRUE;
        }
        else
            handled = FALSE;

        if (handled)
            SetCapture(hWnd_);

        return handled;
    }

    LRESULT NonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam) {
        BOOL handled = FALSE;

        if (wParam == HTMINBUTTON)
        {
            ShowWindow(hWnd_, SW_MINIMIZE);
            mMinButtonColor = MIN_BUTTON_COLOR;
            handled = TRUE;
        }
        else if (wParam == HTMAXBUTTON)
        {
            if (IsZoomed(hWnd_))
                ShowWindow(hWnd_, SW_NORMAL);
            else
                ShowWindow(hWnd_, SW_MAXIMIZE);

            mMaxButtonColor = MAX_BUTTON_COLOR;
            drawCaptionAndBorder();
            handled = TRUE;
        }
        else if (wParam == HTCLOSE)
        {
            //CloseDialog(0);
            handled = TRUE;
        }
        else
            handled = FALSE;

        mIsMousePressedInMinButton = false;
        mIsMousePressedInMaxButton = false;
        mIsMousePressedInCloseButton = false;

        return handled;
    }

    LRESULT NonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam) {
        return TRUE;
    }


    void NonClientFrame::drawCaptionAndBorder()
    {
        HDC hdc = GetWindowDC(hWnd_);
        SelectObject(hdc, mFramePen);
        SelectObject(hdc, mFrameBrush);
        SelectObject(hdc, mTitleFont);

        RECT rcWin;
        GetWindowRect(hWnd_, &rcWin);
        OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

        //Top
        Rectangle(hdc,
            0, 0,
            rcWin.right, CAPTION_HEIGHT + TOP_BORDER_WIDTH);

        //Left
        Rectangle(hdc,
            0, CAPTION_HEIGHT + TOP_BORDER_WIDTH,
            LEFT_BORDER_WIDTH, rcWin.bottom);

        //Right
        Rectangle(hdc,
            rcWin.right - RIGHT_BORDER_WIDTH, CAPTION_HEIGHT + TOP_BORDER_WIDTH,
            rcWin.right, rcWin.bottom);

        //Bottom
        Rectangle(hdc,
            0, rcWin.bottom - BOTTOM_BORDER_WIDTH,
            rcWin.right, rcWin.bottom);

        //Title
        SetBkColor(hdc, BORDER_COLOR);
        SetTextColor(hdc, TITLE_COLOR);
        DrawTextW(hdc, TITLE_TEXT, wcslen(TITLE_TEXT), &mTitleRect, DT_SINGLELINE | DT_VCENTER);

        SelectObject(hdc, GetStockObject(DC_BRUSH));

        //Min button
        SetDCBrushColor(hdc, mMinButtonColor);
        PaintRgn(hdc, mMinButtonRgn);

        //Max button
        SetDCBrushColor(hdc, mMaxButtonColor);
        PaintRgn(hdc, mMaxButtonRgn);

        //Close button
        SetDCBrushColor(hdc, mCloseButtonColor);
        PaintRgn(hdc, mCloseButtonRgn);

        ReleaseDC(hWnd_, hdc);
    }

    void NonClientFrame::createCaptionButtonRgn(int rightOfWin, int topPadding, int rightPadding)
    {
        RECT rcCloseButton = {
            rightOfWin - CLOSE_BUTTON_RIGHT_MARGIN - CLOSE_BUTTON_WIDTH - rightPadding,
            static_cast<int>(std::ceil((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding - CLOSE_BUTTON_HEIGHT) / 2.f)),
            rightOfWin - CLOSE_BUTTON_RIGHT_MARGIN - rightPadding,
            static_cast<int>(std::floor((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding + CLOSE_BUTTON_HEIGHT) / 2.f))
        };

        RECT rcMaxButton = {
            rcCloseButton.left - MAX_BUTTON_RIGHT_MARGIN - MAX_BUTTON_WIDTH,
            static_cast<int>(std::ceil((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding - MAX_BUTTON_HEIGHT) / 2.f)),
            rcCloseButton.left - MAX_BUTTON_RIGHT_MARGIN,
            static_cast<int>(std::floor((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding + MAX_BUTTON_HEIGHT) / 2.f))
        };

        RECT rcMinButton = {
            rcMaxButton.left - MIN_BUTTON_RIGHT_MARGIN - MIN_BUTTON_WIDTH,
            static_cast<int>(std::ceil((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding - MIN_BUTTON_HEIGHT) / 2.f)),
            rcMaxButton.left - MIN_BUTTON_RIGHT_MARGIN,
            static_cast<int>(std::floor((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding + MIN_BUTTON_HEIGHT) / 2.f))
        };

        mMinButtonRgn = CreateRectRgnIndirect(&rcMinButton);
        mMaxButtonRgn = CreateRectRgnIndirect(&rcMaxButton);
        mCloseButtonRgn = CreateRectRgnIndirect(&rcCloseButton);
    }

}