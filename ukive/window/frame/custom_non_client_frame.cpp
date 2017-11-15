#include "custom_non_client_frame.h"

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


namespace ukive {

    const Gdiplus::Color TITLE_COLOR(0xff, 0xff, 0xff);
    const Gdiplus::Color BORDER_COLOR(0xe5, 0x1c, 0x23);              //Red 500
    const Gdiplus::Color MIN_BUTTON_COLOR(0x41, 0xbd, 0x42);          //Green 300
    const Gdiplus::Color MIN_BUTTON_PRESSED_COLOR(0x08, 0x8f, 0x0a);  //Green 600
    const Gdiplus::Color MAX_BUTTON_COLOR(0x35, 0xd8, 0xfd);          //Yellow 600
    const Gdiplus::Color MAX_BUTTON_PRESSED_COLOR(0x17, 0x7f, 0xf5);  //Yellow 900
    const Gdiplus::Color CLOSE_BUTTON_COLOR(0x4d, 0xb7, 0xff);        //Orange 300
    const Gdiplus::Color CLOSE_BUTTON_PRESSED_COLOR(0x00, 0x8c, 0xfb);//Orange 600


    bool colorEqual(const Gdiplus::Color &lhs, const Gdiplus::Color &rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }


    void CustomNonClientFrame::init(HWND hWnd) {
        hWnd_ = hWnd;

        //resources used to drawing Non-client.
        brush_.SetColor(BORDER_COLOR);

        min_button_color_ = MIN_BUTTON_COLOR;
        max_button_color_ = MAX_BUTTON_COLOR;
        close_button_color_ = CLOSE_BUTTON_COLOR;

        mIsMousePressedInMinButton = false;
        mIsMousePressedInMaxButton = false;
        mIsMousePressedInCloseButton = false;

        RECT rcWin;
        GetWindowRect(hWnd_, &rcWin);
        OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

        createCaptionButtonRgn(rcWin.right, 0, 0);

        mWindowRgn = CreateRoundRectRgn(
            0, 0,
            rcWin.right + 1, rcWin.bottom + 1,
            WINDOW_CORNER_RADIUS, WINDOW_CORNER_RADIUS);
        SetWindowRgn(hWnd_, mWindowRgn, TRUE);

        title_rect_.X = TITLE_LEFT_MARGIN;
        title_rect_.Y = 0;
        title_rect_.Width = TITLE_WIDTH;
        title_rect_.Height = CAPTION_HEIGHT + TOP_BORDER_WIDTH;

        string_format_.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    }


    LRESULT CustomNonClientFrame::onSize(WPARAM wParam, LPARAM lParam) {

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

            title_rect_.X = TITLE_LEFT_MARGIN + leftExtended - LEFT_BORDER_WIDTH;
            title_rect_.Y = topExtended - TOP_BORDER_WIDTH;
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

            title_rect_.X = TITLE_LEFT_MARGIN;
            title_rect_.Y = 0;
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

    LRESULT CustomNonClientFrame::onMouseMove(WPARAM wParam, LPARAM lParam) {
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
                if (min_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
                {
                    if (!colorEqual(min_button_color_, MIN_BUTTON_PRESSED_COLOR))
                    {
                        min_button_color_ = MIN_BUTTON_PRESSED_COLOR;
                        needRedraw = true;
                    }
                }
                else
                {
                    if (!colorEqual(min_button_color_, MIN_BUTTON_COLOR))
                    {
                        min_button_color_ = MIN_BUTTON_COLOR;
                        needRedraw = true;
                    }
                }
            }
            else if (mIsMousePressedInMaxButton)
            {
                if (max_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
                {
                    if (!colorEqual(max_button_color_, MAX_BUTTON_PRESSED_COLOR))
                    {
                        max_button_color_ = MAX_BUTTON_PRESSED_COLOR;
                        needRedraw = true;
                    }
                }
                else
                {
                    if (!colorEqual(max_button_color_, MAX_BUTTON_COLOR))
                    {
                        max_button_color_ = MAX_BUTTON_COLOR;
                        needRedraw = true;
                    }
                }
            }
            else if (mIsMousePressedInCloseButton)
            {
                if (close_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
                {
                    if (!colorEqual(close_button_color_, CLOSE_BUTTON_PRESSED_COLOR))
                    {
                        close_button_color_ = CLOSE_BUTTON_PRESSED_COLOR;
                        needRedraw = true;
                    }
                }
                else
                {
                    if (!colorEqual(close_button_color_, CLOSE_BUTTON_COLOR))
                    {
                        close_button_color_ = CLOSE_BUTTON_COLOR;
                        needRedraw = true;
                    }
                }
            }

            if (needRedraw)
                drawCaptionAndBorder();
        }

        return TRUE;
    }

    LRESULT CustomNonClientFrame::OnLButtonUp(WPARAM wParam, LPARAM lParam) {
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
                && min_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
            {
                ShowWindow(hWnd_, SW_MINIMIZE);
                min_button_color_ = MIN_BUTTON_COLOR;
                handled = TRUE;
            }
            else if (mIsMousePressedInMaxButton
                && max_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
            {
                if (IsZoomed(hWnd_))
                    ShowWindow(hWnd_, SW_NORMAL);
                else
                    ShowWindow(hWnd_, SW_MAXIMIZE);

                max_button_color_ = MAX_BUTTON_COLOR;
                drawCaptionAndBorder();
                handled = TRUE;
            }
            else if (mIsMousePressedInCloseButton
                && close_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
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

    LRESULT CustomNonClientFrame::onNcPaint(WPARAM wParam, LPARAM lParam) {
        drawCaptionAndBorder();
        return TRUE;
    }

    LRESULT CustomNonClientFrame::onNcActivate(WPARAM wParam, LPARAM lParam) {
        if (wParam == FALSE)
            drawCaptionAndBorder();
        return TRUE;
    }

    LRESULT CustomNonClientFrame::onNcHitTest(WPARAM wParam, LPARAM lParam) {
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
            if (min_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
            {
                return HTMINBUTTON;
            }
            else if (max_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
            {
                return HTMAXBUTTON;
            }
            else if (close_button_rect_.Contains(xPos - rcWin.left, yPos - rcWin.top))
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
                if (!colorEqual(min_button_color_, MIN_BUTTON_COLOR))
                {
                    min_button_color_ = MIN_BUTTON_COLOR;
                    needRedraw = true;
                }

                if (!colorEqual(max_button_color_, MAX_BUTTON_COLOR))
                {
                    max_button_color_ = MAX_BUTTON_COLOR;
                    needRedraw = true;
                }

                if (!colorEqual(close_button_color_, CLOSE_BUTTON_COLOR))
                {
                    close_button_color_ = CLOSE_BUTTON_COLOR;
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

    LRESULT CustomNonClientFrame::onNcCalSize(WPARAM wParam, LPARAM lParam) {
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

    LRESULT CustomNonClientFrame::onNcLButtonDown(WPARAM wParam, LPARAM lParam) {
        BOOL handled = FALSE;

        if (wParam == HTMINBUTTON)
        {
            min_button_color_ = MIN_BUTTON_PRESSED_COLOR;
            drawCaptionAndBorder();
            mIsMousePressedInMinButton = true;
            handled = TRUE;
        }
        else if (wParam == HTMAXBUTTON)
        {
            max_button_color_ = MAX_BUTTON_PRESSED_COLOR;
            drawCaptionAndBorder();
            mIsMousePressedInMaxButton = true;
            handled = TRUE;
        }
        else if (wParam == HTCLOSE)
        {
            close_button_color_ = CLOSE_BUTTON_PRESSED_COLOR;
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

    LRESULT CustomNonClientFrame::onNcLButtonUp(WPARAM wParam, LPARAM lParam) {
        BOOL handled = FALSE;

        if (wParam == HTMINBUTTON)
        {
            ShowWindow(hWnd_, SW_MINIMIZE);
            min_button_color_ = MIN_BUTTON_COLOR;
            handled = TRUE;
        }
        else if (wParam == HTMAXBUTTON)
        {
            if (IsZoomed(hWnd_))
                ShowWindow(hWnd_, SW_NORMAL);
            else
                ShowWindow(hWnd_, SW_MAXIMIZE);

            max_button_color_ = MAX_BUTTON_COLOR;
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

    LRESULT CustomNonClientFrame::onInterceptDrawClassic(WPARAM wParam, LPARAM lParam) {
        return TRUE;
    }


    void CustomNonClientFrame::drawCaptionAndBorder()
    {
        HDC hdc = ::GetWindowDC(hWnd_);
        Gdiplus::Graphics graphics(hdc);

        RECT rcWin;
        ::GetWindowRect(hWnd_, &rcWin);
        ::OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

        Gdiplus::Rect frame[4];

        //Top
        frame[0] = Gdiplus::Rect(
            0, 0, rcWin.right - rcWin.left, CAPTION_HEIGHT + TOP_BORDER_WIDTH);
        //Left
        frame[1] = Gdiplus::Rect(
            0, CAPTION_HEIGHT + TOP_BORDER_WIDTH,
            LEFT_BORDER_WIDTH, rcWin.bottom - rcWin.top - CAPTION_HEIGHT + TOP_BORDER_WIDTH);
        //Right
        frame[2] = Gdiplus::Rect(
            rcWin.right - RIGHT_BORDER_WIDTH, CAPTION_HEIGHT + TOP_BORDER_WIDTH,
            RIGHT_BORDER_WIDTH, rcWin.bottom - rcWin.top - CAPTION_HEIGHT + TOP_BORDER_WIDTH);
        //Bottom
        frame[3] = Gdiplus::Rect(
            0, rcWin.bottom - BOTTOM_BORDER_WIDTH,
            rcWin.right - rcWin.left, BOTTOM_BORDER_WIDTH);

        brush_.SetColor(BORDER_COLOR);
        graphics.FillRectangles(&brush_, frame, 4);

        //Title
        brush_.SetColor(TITLE_COLOR);
        graphics.DrawString(TITLE_TEXT, wcslen(TITLE_TEXT), &title_font_, title_rect_, &string_format_, &brush_);

        //Min button
        brush_.SetColor(min_button_color_);
        graphics.FillEllipse(&brush_, min_button_rect_);

        //Max button
        brush_.SetColor(max_button_color_);
        graphics.FillEllipse(&brush_, max_button_rect_);

        //Close button
        brush_.SetColor(close_button_color_);
        graphics.FillEllipse(&brush_, close_button_rect_);

        ::ReleaseDC(hWnd_, hdc);
    }

    void CustomNonClientFrame::createCaptionButtonRgn(int rightOfWin, int topPadding, int rightPadding)
    {
        close_button_rect_ = {
            rightOfWin - CLOSE_BUTTON_RIGHT_MARGIN - CLOSE_BUTTON_WIDTH - rightPadding,
            static_cast<int>(std::ceil((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding - CLOSE_BUTTON_HEIGHT) / 2.f)),
            CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT
        };

        max_button_rect_ = {
            close_button_rect_.X - MAX_BUTTON_RIGHT_MARGIN - MAX_BUTTON_WIDTH,
            static_cast<int>(std::ceil((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding - MAX_BUTTON_HEIGHT) / 2.f)),
            MAX_BUTTON_WIDTH, MAX_BUTTON_HEIGHT,
        };

        min_button_rect_ = {
            max_button_rect_.X - MIN_BUTTON_RIGHT_MARGIN - MIN_BUTTON_WIDTH,
            static_cast<int>(std::ceil((CAPTION_HEIGHT + TOP_BORDER_WIDTH + topPadding - MIN_BUTTON_HEIGHT) / 2.f)),
            MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT
        };
    }

}