#include "window_impl.h"

#include "application.h"
#include "log.h"
#include "window/window.h"
#include "window/window_class_manager.h"
#include "window/window_manager.h"


namespace ukive {

    const int kDefaultX = 0;
    const int kDefaultY = 0;
    const int kDefaultWidth = 200;
    const int kDefaultHeight = 200;
    const string16 kDefaultTitle = L"Ukive Window";


    WindowImpl::WindowImpl(Window *win)
        :delegate_(win),
        hWnd_(NULL),
        x_(kDefaultX),
        y_(kDefaultY),
        width_(kDefaultWidth),
        height_(kDefaultHeight),
        min_width_(0),
        min_height_(0),
        title_(kDefaultTitle),
        is_created_(false),
        is_showing_(false)
    {}

    WindowImpl::~WindowImpl() {}

    void WindowImpl::init() {
        ClassInfo info;
        delegate_->onPreCreate(info);

        ATOM atom = WindowClassManager::getInstance()->retrieveWindowClass(info);

        HWND hWnd = ::CreateWindowEx(
            WS_EX_APPWINDOW,
            reinterpret_cast<wchar_t*>(atom),
            title_.c_str(),
            WS_OVERLAPPEDWINDOW,
            x_, y_, width_, height_, 0, 0,
            Application::getModuleHandle(), this);
        if (::IsWindow(hWnd) == FALSE) {
            Log::e(L"failed to create window.");
            return;
        }

        hWnd_ = hWnd;
    }

    void WindowImpl::show() {
        if (!is_created_) {
            init();
            is_created_ = true;
            WindowManager::getInstance()->addWindow(this);
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

        if (notify ? delegate_->onClose() : true)
        {
            BOOL succeed = ::DestroyWindow(hWnd_);
            if (succeed == 0) {
                Log::e(L"failed to destroy window.");
                return;
            }

            is_created_ = false;
            is_showing_ = false;
            WindowManager::getInstance()->addWindow(this);
        }
    }

    void WindowImpl::setTitle(string16 title) {

    }

    void WindowImpl::setX(int x) {

    }

    void WindowImpl::setY(int y) {

    }

    void WindowImpl::setPosition(int x, int y) {

    }

    void WindowImpl::setWidth(int width) {

    }

    void WindowImpl::setHeight(int height) {

    }

    void WindowImpl::setMinWidth(int minWidth) {

    }

    void WindowImpl::setMinHeight(int minHeight) {

    }

    void WindowImpl::setStartupWindow(bool enable) {

    }

    string16 WindowImpl::getTitle() {

    }

    int WindowImpl::getX() {

    }

    int WindowImpl::getY() {

    }

    int WindowImpl::getWidth() {

    }

    int WindowImpl::getHeight() {

    }

    int WindowImpl::getMinWidth() {

    }

    int WindowImpl::getMinHeight() {

    }

    bool WindowImpl::isCreated() {

    }

    bool WindowImpl::isShowing() {

    }

    bool WindowImpl::isStartupWindow() {

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

        case WM_CLOSE:
            if (isStartupWindow())
            {
                if (delegate_->onClose())
                {
                    size_t count = WindowManager::getInstance()->getWindowCount();
                    for (size_t i = 0; i < count; ++i)
                    {
                        auto pcWindow = WindowManager::getInstance()->getWindow(i);
                        if (!pcWindow->isStartupWindow())
                            pcWindow->close();
                    }
                    close(false);
                }
            }
            else
                close();
            return 0;

        case WM_DESTROY:
            delegate_->onDestroy();
            if (isStartupWindow())
                ::PostQuitMessage(0);
            return 0;

        case WM_SHOWWINDOW:
            delegate_->onShow((BOOL)wParam == TRUE ? true : false);
            break;

        case WM_ACTIVATE:
        {
            delegate_->onActivate(LOWORD(wParam));
            break;
        }

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SETCURSOR:
        {
            break;
        }

        case WM_MOVE:
            notifyLocationChanged(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_SIZE:
        {
            RECT winRect;
            ::GetWindowRect(hWnd_, &winRect);

            notifySizeChanged(
                wParam,
                winRect.right - winRect.left, winRect.bottom - winRect.top,
                LOWORD(lParam), HIWORD(lParam));
            return 0;
        }

        case WM_MOVING:
        {
            if (delegate_->onMoving((RECT*)lParam))
                return TRUE;
            break;
        }

        case WM_SIZING:
        {
            if (delegate_->onResizing(wParam, (RECT*)lParam))
                return TRUE;
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_MOUSEHOVER:
        case WM_MOUSEWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
            break;
        }

        return ::DefWindowProcW(hWnd_, uMsg, wParam, lParam);
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
        return window->messageHandler(uMsg, wParam, lParam);
    }
}