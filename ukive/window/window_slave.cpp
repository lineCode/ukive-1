#include "window_slave.h"

#include <dwmapi.h>
#include <windowsx.h>

#include "ukive/window/window.h"


namespace ukive {

    UWindowSlave::UWindowSlave(Window *master)
    {
        sUWSVtr = this;
        mMaster = master;
        mHandle = nullptr;
    }


    UWindowSlave::~UWindowSlave()
    {
    }


    void UWindowSlave::initSlave()
    {
        WNDCLASSEX wc;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = ::GetModuleHandleW(nullptr);
        wc.hIcon = LoadIconW(nullptr, IDI_WINLOGO);
        wc.hIconSm = wc.hIcon;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = L"UWindowSlave";
        wc.cbSize = sizeof(WNDCLASSEXW);

        if (RegisterClassExW(&wc) != 0)
        {
            int frameThickness = GetSystemMetrics(SM_CXFRAME);
            int frameThickness4 = GetSystemMetrics(SM_CXBORDER);
            int frameThickness5 = GetSystemMetrics(SM_CXSIZEFRAME);
            int frameThickness6 = GetSystemMetrics(SM_CYSIZEFRAME);
            int frameThickness7 = GetSystemMetrics(SM_CXDLGFRAME);
            int frameThickness8 = GetSystemMetrics(SM_CYDLGFRAME);

            mHandle = ::CreateWindowExW(
                WS_EX_TOOLWINDOW,
                L"UWindowSlave", L"", WS_POPUP,
                mMaster->getX() - 7, mMaster->getY() - 7,
                mMaster->getWidth() + 14, mMaster->getHeight() + 14,
                0, 0, ::GetModuleHandleW(nullptr), nullptr);

            if (mHandle)
            {
                ::ShowWindow(mHandle, SW_SHOWNOACTIVATE);
            }
        }
    }


    LRESULT CALLBACK UWindowSlave::messageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        Window *window = nullptr;

        switch (uMsg)
        {
        case WM_ACTIVATE:
            SetWindowPos(hWnd, mMaster->getHandle(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            return TRUE;

        case WM_MOVE:
            //window->notifyWindowLocationChanged(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_SIZE:
        {
            RECT newSlaveRect;
            ::GetWindowRect(hWnd, &newSlaveRect);

            int newX = newSlaveRect.left + 7;
            int newY = newSlaveRect.top + 7;
            int newWidth = (newSlaveRect.right - newSlaveRect.left) - 14;
            int newHeight = (newSlaveRect.bottom - newSlaveRect.top) - 14;

            ::MoveWindow(mMaster->getHandle(), newX, newY, newWidth, newHeight, FALSE);
            return 0;
        }

        case WM_MOVING:
        {
            //if (window->onMoving((RECT*)lParam))
                //return TRUE;
            break;
        }

        case WM_SIZING:
        {
            /*if (mMaster->onResizing(wParam, (RECT*)lParam))
                return TRUE;*/
            break;
        }
        }

        return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }


    void UWindowSlave::sync()
    {
        if (mMaster->getHandle() != nullptr
            && mMaster->isShowing()
            && mHandle == nullptr)
            initSlave();
    }

    HWND UWindowSlave::getWindowHandle()
    {
        return mHandle;
    }


    LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        bool callDWP = true;
        BOOL dwmEnabled = FALSE;
        LRESULT lRet = 0;
        HRESULT hr = S_OK;

        // Winproc worker for custom frame issues.
        hr = ::DwmIsCompositionEnabled(&dwmEnabled);
        if (SUCCEEDED(hr))
        {
            lRet = sUWSVtr->processDWMProc(hWnd, uMsg, wParam, lParam, &callDWP);
        }

        // Winproc worker for the rest of the application.
        if (callDWP)
        {
            lRet = sUWSVtr->messageHandler(hWnd, uMsg, wParam, lParam);
        }

        return lRet;
    }


    LRESULT UWindowSlave::processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP)
    {
        LRESULT lRet = 0;
        HRESULT hr = S_OK;
        bool fCallDWP = true; // Pass on to DefWindowProc?

        fCallDWP = !::DwmDefWindowProc(hWnd, message, wParam, lParam, &lRet);

        // Handle window creation.
        if (message == WM_CREATE)
        {
            RECT rcClient;
            GetWindowRect(hWnd, &rcClient);

            // Inform application of the frame change.
            SetWindowPos(hWnd,
                0,
                rcClient.left, rcClient.top,
                rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                SWP_FRAMECHANGED);

            fCallDWP = true;
            lRet = 0;
        }

        // Handle the non-client size message.
        if (message == WM_NCCALCSIZE && wParam == TRUE)
        {
            lRet = 0;
            fCallDWP = false;
        }

        // Handle hit testing in the NCA if not handled by DwmDefWindowProc.
        if ((message == WM_NCHITTEST) && (lRet == 0))
        {
            lRet = HitTestNCA(hWnd, wParam, lParam, 7, 7, 7, 7);
            if (lRet != HTNOWHERE)
                fCallDWP = false;
        }

        *pfCallDWP = fCallDWP;

        return lRet;
    }

    LRESULT UWindowSlave::HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam,
        int leftExt, int topExt, int rightExt, int bottomExt)
    {
        // Get the point coordinates for the hit test.
        POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        // Get the window rectangle.
        RECT rcWindow;
        GetWindowRect(hWnd, &rcWindow);

        // Determine if the hit test is for resizing. Default middle (1,1).
        USHORT uRow = 1;
        USHORT uCol = 1;

        // Determine if the point is at the top or bottom of the window.
        if (ptMouse.y >= rcWindow.top
            && ptMouse.y < rcWindow.top + topExt)
        {
            uRow = 0;
        }
        else if (ptMouse.y < rcWindow.bottom
            && ptMouse.y >= rcWindow.bottom - bottomExt)
        {
            uRow = 2;
        }

        // Determine if the point is at the left or right of the window.
        if (ptMouse.x >= rcWindow.left
            && ptMouse.x < rcWindow.left + leftExt)
        {
            uCol = 0; // left side
        }
        else if (ptMouse.x < rcWindow.right
            && ptMouse.x >= rcWindow.right - rightExt)
        {
            uCol = 2; // right side
        }

        // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
        LRESULT hitTests[3][3] =
        {
            { HTTOPLEFT,    HTTOP,    HTTOPRIGHT },
            { HTLEFT,       HTNOWHERE,     HTRIGHT },
            { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
        };

        return hitTests[uRow][uCol];
    }

}