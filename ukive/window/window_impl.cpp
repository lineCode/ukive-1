#include "window_impl.h"

#include <algorithm>

#include <dwmapi.h>
#include <VersionHelpers.h>

#include "ukive/application.h"
#include "ukive/log.h"
#include "ukive/window/window.h"
#include "ukive/window/window_class_manager.h"
#include "ukive/window/frame/non_client_frame.h"
#include "ukive/window/frame/default_non_client_frame.h"
#include "ukive/window/frame/drawable_non_client_frame.h"
#include "ukive/event/input_event.h"

#include "shell/test/frame/custom_non_client_frame.h"


namespace ukive {

    const int kDefaultX = 0;
    const int kDefaultY = 0;
    const int kDefaultWidth = 400;
    const int kDefaultHeight = 400;
    const string16 kDefaultTitle = L"Ukive Window";

    const int kDefaultClassStyle = CS_HREDRAW | CS_VREDRAW;
    const int kDefaultWindowStyle = WS_OVERLAPPEDWINDOW;
    const int kDefaultWindowExStyle = WS_EX_APPWINDOW;


    WindowImpl::WindowImpl(Window* win)
        : delegate_(win),
          hWnd_(nullptr),
          cursor_(Cursor::ARROW),
          x_(kDefaultX),
          y_(kDefaultY),
          prev_x_(kDefaultX),
          prev_y_(kDefaultY),
          width_(kDefaultWidth),
          height_(kDefaultHeight),
          prev_width_(kDefaultWidth),
          prev_height_(kDefaultHeight),
          title_(kDefaultTitle),
          is_created_(false),
          is_showing_(false),
          is_translucent_(false),
          is_enable_mouse_track_(true),
          is_first_nccalc_(true) {
    }

    WindowImpl::~WindowImpl() {}


    void WindowImpl::init() {
        ClassInfo info;
        info.style = kDefaultClassStyle;
        info.icon = info.icon_small = ::LoadIcon(nullptr, IDI_WINLOGO);
        info.cursor = ::LoadCursor(nullptr, IDC_ARROW);

        int win_style = kDefaultWindowStyle;
        int win_ex_style = kDefaultWindowExStyle;

        if (is_translucent_) {
            win_ex_style |= WS_EX_LAYERED;
        }

        onPreCreate(&info, &win_style, &win_ex_style);

        ATOM atom = WindowClassManager::getInstance()->retrieveWindowClass(info);
        HWND hWnd = ::CreateWindowEx(
            win_ex_style,
            reinterpret_cast<wchar_t*>(atom),
            title_.c_str(), win_style,
            x_, y_, width_, height_,
            nullptr, nullptr, Application::getModuleHandle(), this);
        if (::IsWindow(hWnd) == FALSE) {
            LOG(Log::ERR) << "failed to create window.";
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
        if (!is_created_) {
            init();
            is_created_ = true;
        }

        if (!is_showing_) {
            return;
        }

        ::ShowWindow(hWnd_, SW_HIDE);
        is_showing_ = false;
    }

    void WindowImpl::minimize() {
        if (!is_created_) {
            init();
            is_created_ = true;
        }

        ::ShowWindow(hWnd_, SW_MINIMIZE);
    }

    void WindowImpl::maximize() {
        if (!is_created_) {
            init();
            is_created_ = true;
        }

        ::ShowWindow(hWnd_, SW_MAXIMIZE);
    }

    void WindowImpl::restore() {
        if (!is_created_) {
            init();
            is_created_ = true;
        }

        ::ShowWindow(hWnd_, SW_RESTORE);
    }

    void WindowImpl::focus() {
        if (is_created_) {
            ::SetFocus(hWnd_);
        }
    }

    void WindowImpl::close() {
        if (!is_created_) {
            return;
        }

        if (onClose()) {
            BOOL ret = ::DestroyWindow(hWnd_);
            if (ret == 0) {
                LOG(Log::ERR) << "Failed to destroy window.";
            }
        }
    }

    void WindowImpl::center() {
        x_ = std::round((Application::getScreenWidth() - width_) / 2.f);
        y_ = std::round((Application::getScreenHeight() - height_) / 2.f);

        if (::IsWindow(hWnd_)) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImpl::setTitle(const string16& title) {
        title_ = title;
        if (::IsWindow(hWnd_)) {
            ::SetWindowText(hWnd_, title_.c_str());
        }
    }

    void WindowImpl::setBounds(int x, int y, int width, int height) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        if (::IsWindow(hWnd_)) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImpl::setCurrentCursor(Cursor cursor) {
        HCURSOR native_cursor = nullptr;
        switch (cursor) {
        case Cursor::ARROW: native_cursor = ::LoadCursor(nullptr, IDC_ARROW); break;
        case Cursor::IBEAM: native_cursor = ::LoadCursor(nullptr, IDC_IBEAM); break;
        case Cursor::WAIT: native_cursor = ::LoadCursor(nullptr, IDC_WAIT); break;
        case Cursor::CROSS: native_cursor = ::LoadCursor(nullptr, IDC_CROSS); break;
        case Cursor::UPARROW: native_cursor = ::LoadCursor(nullptr, IDC_UPARROW); break;
        case Cursor::SIZENWSE: native_cursor = ::LoadCursor(nullptr, IDC_SIZENWSE); break;
        case Cursor::SIZENESW: native_cursor = ::LoadCursor(nullptr, IDC_SIZENESW); break;
        case Cursor::SIZEWE: native_cursor = ::LoadCursor(nullptr, IDC_SIZEWE); break;
        case Cursor::SIZENS: native_cursor = ::LoadCursor(nullptr, IDC_SIZENS); break;
        case Cursor::SIZEALL: native_cursor = ::LoadCursor(nullptr, IDC_SIZEALL); break;
        case Cursor::NO: native_cursor = ::LoadCursor(nullptr, IDC_NO); break;
        case Cursor::HAND: native_cursor = ::LoadCursor(nullptr, IDC_HAND); break;
        case Cursor::APPSTARTING: native_cursor = ::LoadCursor(nullptr, IDC_APPSTARTING); break;
        case Cursor::HELP: native_cursor = ::LoadCursor(nullptr, IDC_HELP); break;
        case Cursor::PIN: native_cursor = ::LoadCursor(nullptr, IDC_PIN); break;
        case Cursor::PERSON: native_cursor = ::LoadCursor(nullptr, IDC_PERSON); break;
        }

        if (!native_cursor) {
            LOG(Log::ERR) << "Null native cursor.";
            return;
        }

        cursor_ = cursor;

        ::SetCursor(native_cursor);
    }

    void WindowImpl::setTranslucent(bool translucent) {
        if (::IsWindow(hWnd_)) {
            setWindowStyle(WS_EX_LAYERED, true, translucent);
            non_client_frame_->onTranslucentChanged(translucent);
            sendFrameChanged();
        }
        is_translucent_ = translucent;
    }

    string16 WindowImpl::getTitle() const {
        return title_;
    }

    int WindowImpl::getX() const {
        return x_;
    }

    int WindowImpl::getY() const {
        return y_;
    }

    int WindowImpl::getWidth() const {
        return width_;
    }

    int WindowImpl::getHeight() const {
        return height_;
    }

    int WindowImpl::getClientWidth() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        return rect.right - rect.left;
    }

    int WindowImpl::getClientHeight() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        return rect.bottom - rect.top;
    }

    int WindowImpl::getDpi() const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        // TODO: Windows 10 1607
        if (::IsWindows10OrGreater()) {
            using GetDpiForWindowPtr = UINT(WINAPI*)(HWND);
            auto func = reinterpret_cast<GetDpiForWindowPtr>(
                ::GetProcAddress(::LoadLibraryW(L"User32.dll"), "GetDpiForWindow"));
            if (func) {
                int dpi_x = func(hWnd_);
                if (dpi_x > 0) {
                    return dpi_x;
                }
            }
        }

        HDC dc = ::GetDC(hWnd_);
        int dpi_x = ::GetDeviceCaps(dc, LOGPIXELSX);
        //int dpi_y = ::GetDeviceCaps(screen, LOGPIXELSY);
        ::ReleaseDC(hWnd_, dc);

        return dpi_x;
    }

    HWND WindowImpl::getHandle() const {
        return hWnd_;
    }

    Cursor WindowImpl::getCurrentCursor() const {
        return cursor_;
    }

    bool WindowImpl::isCreated() const {
        return is_created_;
    }

    bool WindowImpl::isShowing() const {
        return is_showing_;
    }

    bool WindowImpl::isTranslucent() const {
        return is_translucent_;
    }

    bool WindowImpl::isMinimum() const {
        if (::IsWindow(hWnd_)) {
            return ::IsIconic(hWnd_) == TRUE;
        }

        return false;
    }

    bool WindowImpl::isMaximum() const {
        if (::IsWindow(hWnd_)) {
            return ::IsZoomed(hWnd_) == TRUE;
        }

        return false;
    }

    void WindowImpl::setMouseCaptureRaw() {
        if (!::IsWindow(hWnd_)) {
            return;
        }
        ::SetCapture(hWnd_);
    }

    void WindowImpl::releaseMouseCaptureRaw() {
        if (!::IsWindow(hWnd_)) {
            return;
        }
        ::ReleaseCapture();
    }

    void WindowImpl::setMouseTrack() {
        if (!::IsWindow(hWnd_)) {
            return;
        }

        if (is_enable_mouse_track_) {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;// | TME_HOVER;
            tme.hwndTrack = hWnd_;// 指定要 追踪 的窗口
            tme.dwHoverTime = 1000;  // 鼠标在按钮上停留超过 1s ，才认为状态为 HOVER
            ::_TrackMouseEvent(&tme); // 开启 Windows 的 WM_MOUSELEAVE ， WM_MOUSEHOVER 事件支持

            is_enable_mouse_track_ = false;
        }
    }

    void WindowImpl::setWindowStyle(int style, bool ex, bool enabled) {
        auto win_style = ::GetWindowLongPtr(hWnd_, ex ? GWL_EXSTYLE : GWL_STYLE);
        if (enabled) {
            win_style |= style;
        } else {
            win_style &= ~style;
        }
        ::SetWindowLongPtr(hWnd_, ex ? GWL_EXSTYLE : GWL_STYLE, win_style);
    }

    void WindowImpl::sendFrameChanged() {
        ::SetWindowPos(hWnd_, nullptr, 0, 0, 0, 0,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOCOPYBITS |
            SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOREPOSITION |
            SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER);
    }

    void WindowImpl::convScreenToClient(Point* p) {
        POINT raw_p = { p->x, p->y };
        ::ScreenToClient(hWnd_, &raw_p);
        p->x = raw_p.x;
        p->y = raw_p.y;
    }

    void WindowImpl::convClientToScreen(Point* p) {
        POINT raw_p = { p->x, p->y };
        ::ClientToScreen(hWnd_, &raw_p);
        p->x = raw_p.x;
        p->y = raw_p.y;
    }

    void WindowImpl::setWindowRectShape() {
        HRGN prev_rgn = ::CreateRectRgn(0, 0, 0, 0);
        int type = ::GetWindowRgn(hWnd_, prev_rgn);

        RECT win_rect { 0, 0, 0, 0 };
        ::GetWindowRect(hWnd_, &win_rect);
        ::OffsetRect(&win_rect, -win_rect.left, -win_rect.top);
        HRGN new_rgn = ::CreateRectRgnIndirect(&win_rect);

        if (type == ERROR || !::EqualRgn(prev_rgn, new_rgn)) {
            ::SetWindowRgn(hWnd_, new_rgn, TRUE);
        } else {
            ::DeleteObject(new_rgn);
        }

        ::DeleteObject(prev_rgn);
    }

    bool WindowImpl::isMouseTrackEnabled() {
        return is_enable_mouse_track_;
    }

    float WindowImpl::dpToPx(float dp) {
        return getDpi() / 96.f * dp;
    }

    float WindowImpl::pxToDp(float px) {
        return px / (getDpi() / 96.f);
    }

    void WindowImpl::onPreCreate(ClassInfo* info, int* win_style, int* win_ex_style) {
        delegate_->onPreCreate(info, win_style, win_ex_style);
    }

    void WindowImpl::onCreate() {
        delegate_->onCreate();
    }

    void WindowImpl::onShow(bool show) {
        delegate_->onShow(show);
    }

    void WindowImpl::onActivate(int param) {
        delegate_->onActivate(param);
    }

    void WindowImpl::onSetFocus() {
        delegate_->onSetFocus();
    }

    void WindowImpl::onKillFocus() {
        delegate_->onKillFocus();
    }

    void WindowImpl::onDraw(const Rect &rect) {
        delegate_->onDraw(rect);
    }

    void WindowImpl::onMove(int x, int y) {
        prev_x_ = x_;
        x_ = x;
        prev_y_ = y_;
        y_ = y;

        delegate_->onMove(x, y);
    }

    void WindowImpl::onResize(
        int param, int width, int height,
        int client_width, int client_height)
    {
        if (client_width <= 0 || client_height <= 0) {
            return;
        }

        prev_width_ = width_;
        prev_height_ = height_;
        width_ = width;
        height_ = height;

        delegate_->onResize(param, width, height, client_width, client_height);
    }

    bool WindowImpl::onMoving(Rect* rect) {
        return delegate_->onMoving(rect);
    }

    bool WindowImpl::onResizing(WPARAM edge, Rect* rect) {
        return delegate_->onResizing(edge, rect);
    }

    bool WindowImpl::onClose() {
        return delegate_->onClose();
    }

    void WindowImpl::onDestroy() {
        is_created_ = false;
        is_showing_ = false;

        delegate_->onDestroy();
    }

    bool WindowImpl::onTouch(const TOUCHINPUT* inputs, int size) {
        DCHECK(size > 0);

        InputEvent ev;

        for (int i = 0; i < size; ++i) {
            auto& ti = inputs[i];
            bool need_process = false;

            if (ti.dwFlags & TOUCHEVENTF_DOWN) {
                need_process = true;
                ev.setEvent(size > 1 ? InputEvent::EVT_MULTI_DOWN : InputEvent::EVT_DOWN);
            } else if (ti.dwFlags & TOUCHEVENTF_UP) {
                need_process = true;
                ev.setEvent(size > 1 ? InputEvent::EVT_MULTI_UP : InputEvent::EVT_UP);
            } else if (ti.dwFlags & TOUCHEVENTF_MOVE) {
                need_process = true;
                ev.setEvent(InputEvent::EVT_MOVE);
            }

            if (need_process) {
                ev.setMouseRawX(TOUCH_COORD_TO_PIXEL(ti.x));
                ev.setMouseRawY(TOUCH_COORD_TO_PIXEL(ti.y));

                ::POINT pt;
                pt.x = TOUCH_COORD_TO_PIXEL(ti.x);
                pt.y = TOUCH_COORD_TO_PIXEL(ti.y);
                ::ScreenToClient(hWnd_, &pt);

                ev.setMouseX(pt.x);
                ev.setMouseY(pt.y);

                return onInputEvent(&ev);
            }
        }

        return true;
    }

    bool WindowImpl::onInputEvent(InputEvent* e) {
        // 追踪鼠标，以便产生 EVM_LEAVE_WIN 事件。
        if (e->getEvent() == InputEvent::EVM_LEAVE_WIN) {
            is_enable_mouse_track_ = true;
        } else if (e->getEvent() == InputEvent::EVM_MOVE) {
            setMouseTrack();
        }

        return delegate_->onInputEvent(e);
    }

    void WindowImpl::onDpiChanged(int dpi_x, int dpi_y) {
        delegate_->onDpiChanged(dpi_x, dpi_y);
    }

    void WindowImpl::onStyleChanged(bool normal, bool ext, const STYLESTRUCT* ss) {
        if (ext) {
        }
    }

    bool WindowImpl::onDataCopy(unsigned int id, unsigned int size, void* data) {
        return delegate_->onDataCopy(id, size, data);
    }

    LRESULT WindowImpl::onNCCreate(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (delegate_->getFrameType() == Window::FRAME_CUSTOM) {
            non_client_frame_.reset(new shell::CustomNonClientFrame());
        } else if (delegate_->getFrameType() == Window::FRAME_ZERO) {
            non_client_frame_.reset(new DrawableNonClientFrame());
        } else {
            non_client_frame_.reset(new DefaultNonClientFrame());
        }

        onCreate();
        auto nc_result = non_client_frame_->onNcCreate(this, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onCreate(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }

    LRESULT WindowImpl::onNCDrawClassic1(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto nc_result = non_client_frame_->onInterceptDrawClassic(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onNCDrawClassic2(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto nc_result = non_client_frame_->onInterceptDrawClassic(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onNCPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto nc_result = non_client_frame_->onNcPaint(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onPaint(WPARAM wParam, LPARAM lParam, bool* handled) {
        onDraw({});
        return 0;
    }

    LRESULT WindowImpl::onNCActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto nc_result = non_client_frame_->onNcActivate(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onNCHitTest(WPARAM wParam, LPARAM lParam, bool* handled) {
        POINT point;
        bool pass_to_window = false;
        auto nc_result = non_client_frame_->onNcHitTest(wParam, lParam, handled, &pass_to_window, &point);
        if (*handled && pass_to_window) {
            int win_hp;
            auto hit_point = delegate_->onNCHitTest(point.x, point.y);
            switch (hit_point) {
            case HitPoint::TOP_LEFT:win_hp = HTTOPLEFT; break;
            case HitPoint::TOP:win_hp = HTTOP; break;
            case HitPoint::TOP_RIGHT:win_hp = HTTOPRIGHT; break;
            case HitPoint::LEFT:win_hp = HTLEFT; break;
            case HitPoint::RIGHT:win_hp = HTRIGHT; break;
            case HitPoint::BOTTOM_LEFT:win_hp = HTBOTTOMLEFT; break;
            case HitPoint::BOTTOM:win_hp = HTBOTTOM; break;
            case HitPoint::BOTTOM_RIGHT:win_hp = HTBOTTOMRIGHT; break;
            case HitPoint::CAPTION:win_hp = HTCAPTION; break;
            case HitPoint::SYS_MENU:win_hp = HTSYSMENU; break;
            case HitPoint::MIN_BUTTON:win_hp = HTMINBUTTON; break;
            case HitPoint::MAX_BUTTON:win_hp = HTMAXBUTTON; break;
            case HitPoint::CLOSE_BUTTON:win_hp = HTCLOSE; break;
            default: win_hp = HTCLIENT; break;
            }

            nc_result = win_hp;
        }
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onNCCalCSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (is_first_nccalc_) {
            is_first_nccalc_ = false;
            if (GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_CAPTION) {
                return 0;
            }
        }

        auto nc_result = non_client_frame_->onNcCalSize(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onMouseRange(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled) {
        LRESULT nc_result = 0;

        switch (uMsg) {
        case WM_NCLBUTTONDOWN:
            nc_result = non_client_frame_->onNcLButtonDown(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }
            break;

        case WM_NCLBUTTONUP:
            nc_result = non_client_frame_->onNcLButtonUp(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }
            break;

        case WM_LBUTTONDOWN:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            nc_result = non_client_frame_->OnLButtonUp(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_RBUTTONDOWN:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_RBUTTONUP:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MBUTTONDOWN:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MBUTTONUP:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            nc_result = non_client_frame_->onMouseMove(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_MOVE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL:
        {
            if (::GetMessageExtraInfo() != 0) {
                *handled = true;
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_WHEEL);
            ev.setMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);

            ::POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ::ScreenToClient(hWnd_, &pt);

            ev.setMouseX(pt.x);
            ev.setMouseY(pt.y);
            ev.setMouseRawX(pt.x);
            ev.setMouseRawY(pt.y);
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
        }
        default:
        break;
        }
        return 0;
    }

    LRESULT WindowImpl::onMouseHover(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (::GetMessageExtraInfo() != 0) {
            *handled = true;
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVM_HOVER);
        if (onInputEvent(&ev)) {
            *handled = true;
            return 0;
        }

        return 0;
    }

    LRESULT WindowImpl::onMouseLeave(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (::GetMessageExtraInfo() != 0) {
            *handled = true;
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVM_LEAVE_WIN);
        if (onInputEvent(&ev)) {
            *handled = true;
            return 0;
        }

        return 0;
    }

    LRESULT WindowImpl::onClose(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (!onClose()) {
            *handled = true;
        }
        return 0;
    }

    LRESULT WindowImpl::onDestroy(WPARAM wParam, LPARAM lParam, bool* handled) {
        onDestroy();
        *handled = true;
        return 0;
    }

    LRESULT WindowImpl::onNCDestroy(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto nc_result = non_client_frame_->onNcDestroy(handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onShowWindow(WPARAM wParam, LPARAM lParam, bool* handled) {
        onShow(static_cast<BOOL>(wParam) == TRUE ? true : false);
        return 0;
    }

    LRESULT WindowImpl::onActivate(WPARAM wParam, LPARAM lParam, bool* handled) {
        onActivate(LOWORD(wParam));
        return 0;
    }

    LRESULT WindowImpl::onDPIChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        int new_dpi_x = LOWORD(wParam);
        int new_dpi_y = HIWORD(wParam);

        auto nw_rect = reinterpret_cast<RECT*>(lParam);
        SetWindowPos(hWnd_,
            nullptr,
            nw_rect->left,
            nw_rect->top,
            nw_rect->right - nw_rect->left,
            nw_rect->bottom - nw_rect->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        onDpiChanged(new_dpi_x, new_dpi_y);
        return 0;
    }

    LRESULT WindowImpl::onStyleChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        onStyleChanged(
            wParam & GWL_STYLE, wParam & GWL_EXSTYLE,
            reinterpret_cast<const STYLESTRUCT*>(lParam));
        return 0;
    }

    LRESULT WindowImpl::onCopyData(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto cds = reinterpret_cast<PCOPYDATASTRUCT>(lParam);
        if (!cds) {
            return 0;
        }

        *handled = true;

        if (onDataCopy(cds->dwData, cds->cbData, cds->lpData)) {
            return TRUE;
        }

        return FALSE;
    }

    LRESULT WindowImpl::onEraseBkgnd(WPARAM wParam, LPARAM lParam, bool* handled) {
        *handled = true;
        return TRUE;
    }

    LRESULT WindowImpl::onSetCursor(WPARAM wParam, LPARAM lParam, bool* handled) {
        int hit_code = LOWORD(lParam);

        if (hit_code == HTCLIENT) {
            *handled = true;
            setCurrentCursor(cursor_);
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImpl::onSetFocus(WPARAM wParam, LPARAM lParam, bool* handled) {
        onSetFocus();
        return 0;
    }

    LRESULT WindowImpl::onKillFocus(WPARAM wParam, LPARAM lParam, bool* handled) {
        onKillFocus();
        return 0;
    }

    LRESULT WindowImpl::onSetText(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto text = reinterpret_cast<wchar_t*>(lParam);
        delegate_->onSetText(text);
        return 0;
    }

    LRESULT WindowImpl::onSetIcon(WPARAM wParam, LPARAM lParam, bool* handled) {
        delegate_->onSetIcon();
        return 0;
    }

    LRESULT WindowImpl::onMove(WPARAM wParam, LPARAM lParam, bool* handled) {
        int x_px = LOWORD(lParam);
        int y_px = HIWORD(lParam);
        onMove(x_px, y_px);
        return 0;
    }

    LRESULT WindowImpl::onSize(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (is_translucent_ && (GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_CAPTION)) {
            setWindowRectShape();
        }

        RECT w_rect;
        ::GetWindowRect(hWnd_, &w_rect);
        RECT c_rect;
        ::GetClientRect(hWnd_, &c_rect);

        int width_px = w_rect.right - w_rect.left;
        int height_px = w_rect.bottom - w_rect.top;
        int client_width_px = c_rect.right - c_rect.left;
        int client_height_px = c_rect.bottom - c_rect.top;

        onResize(wParam, width_px, height_px, client_width_px, client_height_px);
        auto nc_result = non_client_frame_->onSize(wParam, lParam, handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onMoving(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto raw_rect = reinterpret_cast<RECT*>(lParam);
        Rect rect(
            raw_rect->left, raw_rect->top,
            raw_rect->right - raw_rect->left,
            raw_rect->bottom - raw_rect->top);

        *handled = onMoving(&rect);
        raw_rect->left = rect.left;
        raw_rect->top = rect.top;
        raw_rect->right = rect.right;
        raw_rect->bottom = rect.bottom;

        if (*handled) {
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImpl::onSizing(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto raw_rect = reinterpret_cast<RECT*>(lParam);
        Rect rect(
            raw_rect->left, raw_rect->top,
            raw_rect->right - raw_rect->left,
            raw_rect->bottom - raw_rect->top);

        *handled = onResizing(wParam, &rect);
        raw_rect->left = rect.left;
        raw_rect->top = rect.top;
        raw_rect->right = rect.right;
        raw_rect->bottom = rect.bottom;

        if (*handled) {
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImpl::onKeyDown(WPARAM wParam, LPARAM lParam, bool* handled) {
        InputEvent ev;
        ev.setEvent(InputEvent::EVK_DOWN);
        ev.setKeyboardVirtualKey(wParam, lParam);

        if (onInputEvent(&ev)) {
            *handled = true;
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImpl::onKeyUp(WPARAM wParam, LPARAM lParam, bool* handled) {
        InputEvent ev;
        ev.setEvent(InputEvent::EVK_UP);
        ev.setKeyboardVirtualKey(wParam, lParam);

        if (onInputEvent(&ev)) {
            *handled = true;
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImpl::onChar(WPARAM wParam, LPARAM lParam, bool* handled) {
        InputEvent ev;
        ev.setEvent(InputEvent::EVK_CHAR);
        ev.setKeyboardCharKey(wParam, lParam);

        if (onInputEvent(&ev)) {
            *handled = true;
            return TRUE;
        }
        return 0;
    }

    LRESULT WindowImpl::onUniChar(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImpl::onGesture(WPARAM wParam, LPARAM lParam, bool* handled) {
        return 0;
    }

    LRESULT WindowImpl::onTouch(WPARAM wParam, LPARAM lParam, bool* handled) {
        UINT input_count = LOWORD(wParam);
        if (input_count == 0) {
            return 0;
        }

        std::unique_ptr<TOUCHINPUT[]> inputs(new TOUCHINPUT[input_count]);
        if (::GetTouchInputInfo(
            reinterpret_cast<HTOUCHINPUT>(lParam), input_count, inputs.get(), sizeof(TOUCHINPUT)))
        {
            *handled = onTouch(inputs.get(), input_count);
            ::CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));

            if (*handled) {
                return 0;
            }
        }
        return 0;
    }

    LRESULT WindowImpl::onDwmCompositionChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        if ((is_translucent_ && (GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_CAPTION))) {
            setWindowRectShape();
        }

        auto nc_result = non_client_frame_->onDwmCompositionChanged(handled);
        if (*handled) {
            return nc_result;
        }
        return 0;
    }

    LRESULT WindowImpl::onWindowPosChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        auto win_pos = reinterpret_cast<WINDOWPOS*>(lParam);
        if (win_pos->flags & (SWP_NOSIZE | SWP_FRAMECHANGED)) {
            //onDraw({});
        }
        return 0;
    }

    LRESULT WindowImpl::processDWMProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP)
    {
        LRESULT ret = 0;
        HRESULT hr = S_OK;
        bool call_dwp = true;

        call_dwp = !::DwmDefWindowProc(hWnd, message, wParam, lParam, &ret);

        if (message == WM_CREATE) {
            RECT rcClient;
            ::GetWindowRect(hWnd, &rcClient);

            // 通知窗口边框变化，以便尽早应用新的非客户区大小
            ::SetWindowPos(hWnd,
                nullptr,
                rcClient.left, rcClient.top,
                rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                SWP_FRAMECHANGED);

            call_dwp = true;
            ret = 0;
        }

        if (message == WM_ACTIVATE) {
            MARGINS margins = { 0,0,0,1 };
            ::DwmExtendFrameIntoClientArea(hWnd, &margins);
            call_dwp = true;
            ret = 0;
        }

        if (message == WM_PAINT) {
            call_dwp = true;
            ret = 0;
        }

        *pfCallDWP = call_dwp;
        return ret;
    }

    LRESULT CALLBACK WindowImpl::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        WindowImpl* window = nullptr;
        if (uMsg == WM_NCCREATE) {
            //::EnableNonClientDpiScaling(hWnd);
            if (::RegisterTouchWindow(hWnd, TWF_WANTPALM) == 0) {
                LOG(Log::WARNING) << "Failed to register touch window: " << GetLastError();
            }

            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            window = reinterpret_cast<WindowImpl*>(cs->lpCreateParams);
            if (!window) {
                DCHECK(Log::FATAL) << "null window creating param.";
                return FALSE;
            }

            window->hWnd_ = hWnd;
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }

        if (uMsg == WM_NCDESTROY) {
            if (::UnregisterTouchWindow(hWnd) == 0) {
                LOG(Log::WARNING) << "Failed to unregister touch window: " << GetLastError();
            }
        }

        if (!window) {
            window = reinterpret_cast<WindowImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (window) {
            bool call_dwp = true;
            if (!window->isTranslucent() && Application::isAeroEnabled()) {
                LRESULT lRet = window->processDWMProc(hWnd, uMsg, wParam, lParam, &call_dwp);
                if (!call_dwp) {
                    return lRet;
                }
            }

            if (call_dwp) {
                bool handled = false;
                auto result = window->processWindowMessage(uMsg, wParam, lParam, &handled);
                if (handled) {
                    return result;
                }
            }
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

}