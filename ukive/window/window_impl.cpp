#include "window_impl.h"

#include "ukive/application.h"
#include "ukive/log.h"
#include "ukive/window/window.h"
#include "ukive/window/window_class_manager.h"
#include "ukive/window/window_manager.h"
#include "ukive/window/frame/non_client_frame.h"
#include "ukive/window/frame/default_non_client_frame.h"


namespace ukive {

    const int kDefaultX = 0;
    const int kDefaultY = 0;
    const int kDefaultWidth = 200;
    const int kDefaultHeight = 200;
    const string16 kDefaultTitle = L"Ukive Window";

    const int kDefaultClassStyle = CS_HREDRAW | CS_VREDRAW;
    const int kDefaultWindowStyle = WS_OVERLAPPEDWINDOW;
    const int kDefaultWindowExStyle = WS_EX_APPWINDOW;


    WindowImpl::WindowImpl(Window *win)
        :delegate_(win),
        hWnd_(NULL),
        x_(kDefaultX),
        y_(kDefaultY),
        width_(kDefaultWidth),
        height_(kDefaultHeight),
        title_(kDefaultTitle),
        is_created_(false),
        is_showing_(false),
        is_startup_window_(false) {
        WindowManager::getInstance()->addWindow(this);
    }

    WindowImpl::~WindowImpl() {}


    void WindowImpl::init() {
        ClassInfo info;
        info.style = kDefaultClassStyle;
        info.icon = info.icon_small = ::LoadIcon(NULL, IDI_WINLOGO);
        info.cursor = ::LoadCursor(NULL, IDC_ARROW);

        delegate_->onPreCreate(&info);

        ATOM atom = WindowClassManager::getInstance()->retrieveWindowClass(info);

        non_client_frame_.reset(new DefaultNonClientFrame());
        //non_client_frame_->init(hWnd_);

        HWND hWnd = ::CreateWindowEx(
            kDefaultWindowExStyle,
            reinterpret_cast<wchar_t*>(atom),
            title_.c_str(),
            kDefaultWindowStyle,
            x_, y_, width_, height_, 0, 0,
            Application::getModuleHandle(), this);
        if (::IsWindow(hWnd) == FALSE) {
            Log::e(L"failed to create window.");
            return;
        }
    }

    void WindowImpl::show() {
        if (!is_created_) {
            init();
            is_created_ = true;
        }

        ::ShowWindow(hWnd_, SW_SHOW);
        is_showing_ = true;
    }

    void WindowImpl::hide() {
        if (!is_created_ || !is_showing_)
            return;

        ::ShowWindow(hWnd_, SW_HIDE);
        is_showing_ = false;
    }

    void WindowImpl::focus() {
        if (is_created_) {
            ::SetFocus(hWnd_);
        }
    }

    void WindowImpl::close() {
        close(true);
    }

    void WindowImpl::close(bool notify) {
        if (!is_created_) {
            return;
        }

        if (notify ? delegate_->onClose() : true) {
            BOOL succeed = ::DestroyWindow(hWnd_);
            if (succeed == 0) {
                Log::e(L"failed to destroy window.");
                return;
            }

            is_created_ = false;
            is_showing_ = false;
            WindowManager::getInstance()->removeWindow(this);
        }
    }

    void WindowImpl::setTitle(const string16 &title) {
        title_ = title;
        if (is_created_) {
            ::SetWindowTextW(hWnd_, title_.c_str());
        }
    }

    void WindowImpl::setBound(int x, int y, int width, int height) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        if (is_created_) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImpl::setStartupWindow(bool enable) {
        is_startup_window_ = enable;
    }

    string16 WindowImpl::getTitle() {
        return title_;
    }

    int WindowImpl::getX() {
        return x_;
    }

    int WindowImpl::getY() {
        return y_;
    }

    int WindowImpl::getWidth() {
        return width_;
    }

    int WindowImpl::getHeight() {
        return height_;
    }

    bool WindowImpl::isCreated() {
        return is_created_;
    }

    bool WindowImpl::isShowing() {
        return is_showing_;
    }

    bool WindowImpl::isStartupWindow() {
        return is_startup_window_;
    }

    void WindowImpl::notifySizeChanged(
        int param, int width, int height,
        int clientWidth, int clientHeight) {

    }

    void WindowImpl::notifyLocationChanged(int x, int y) {

    }


    LRESULT CALLBACK WindowImpl::messageHandler(
        UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CREATE:
            delegate_->onCreate();
            return TRUE;

        case 0xAE:
        case 0xAF:
            if (non_client_frame_->onInterceptDrawClassic(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_NCPAINT:
            if (non_client_frame_->onNcPaint(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_NCACTIVATE:
            if (non_client_frame_->onNcActivate(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_NCHITTEST: {
            LRESULT ret = non_client_frame_->onNcHitTest(wParam, lParam);
            if (ret != HTNOWHERE) {
                return ret;
            }
            break;
        }

        case WM_NCCALCSIZE:
            if (non_client_frame_->onNcCalSize(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_NCLBUTTONDOWN:
            if (non_client_frame_->onNcLButtonDown(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_NCLBUTTONUP:
            if (non_client_frame_->onNcLButtonUp(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_CLOSE:
            if (isStartupWindow()) {
                if (delegate_->onClose()) {
                    size_t count = WindowManager::getInstance()->getWindowCount();
                    for (size_t i = 0; i < count; ++i) {
                        auto pcWindow = WindowManager::getInstance()->getWindow(i);
                        if (!pcWindow->isStartupWindow())
                            pcWindow->close();
                    }
                    close(false);
                }
            }
            else {
                close();
            }
            return 0;

        case WM_DESTROY:
            delegate_->onDestroy();
            if (isStartupWindow())
                ::PostQuitMessage(0);
            return 0;

        case WM_SHOWWINDOW:
            delegate_->onShow((BOOL)wParam == TRUE ? true : false);
            break;

        case WM_ACTIVATE: {
            delegate_->onActivate(LOWORD(wParam));
            break;
        }

        case WM_ERASEBKGND:
            break;

        case WM_SETCURSOR: {
            break;
        }

        case WM_MOVE:
            notifyLocationChanged(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_SIZE: {
            RECT winRect;
            ::GetWindowRect(hWnd_, &winRect);

            notifySizeChanged(
                wParam,
                winRect.right - winRect.left, winRect.bottom - winRect.top,
                LOWORD(lParam), HIWORD(lParam));

            non_client_frame_->onSize(wParam, lParam);
            return 0;
        }

        case WM_MOVING: {
            if (delegate_->onMoving((RECT*)lParam)) {
                return TRUE;
            }
            break;
        }

        case WM_SIZING: {
            if (delegate_->onResizing(wParam, (RECT*)lParam)) {
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONDOWN:
            break;

        case WM_LBUTTONUP:
            if (non_client_frame_->OnLButtonUp(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            break;

        case WM_MOUSEMOVE:
            if (non_client_frame_->onMouseMove(wParam, lParam) == TRUE) {
                return TRUE;
            }
            break;

        case WM_MOUSELEAVE:
        case WM_MOUSEHOVER:
        case WM_MOUSEWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
            break;
        }

        return ::DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK WindowImpl::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCTW *cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            WindowImpl *window = reinterpret_cast<WindowImpl*>(cs->lpCreateParams);
            if (window == nullptr) {
                Log::e(L"null window creating param.");
            }

            window->hWnd_ = hWnd;

            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            return window->messageHandler(uMsg, wParam, lParam);
        }

        WindowImpl *window = reinterpret_cast<WindowImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (window == nullptr) {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        return window->messageHandler(uMsg, wParam, lParam);
    }
}