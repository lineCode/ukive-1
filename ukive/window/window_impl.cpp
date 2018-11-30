#include "window_impl.h"

#include <dwmapi.h>

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

    const int WM_NCDRAWCLASSIC1 = 0xAE;
    const int WM_NCDRAWCLASSIC2 = 0xAF;


    WindowImpl::WindowImpl(Window* win)
        :delegate_(win),
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
        is_enable_mouse_track_(true) {}

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
        if (!is_created_ || !is_showing_) {
            return;
        }

        ::ShowWindow(hWnd_, SW_HIDE);
        is_showing_ = false;
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

        if (is_created_) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImpl::setTitle(const string16& title) {
        title_ = title;
        if (is_created_) {
            ::SetWindowText(hWnd_, title_.c_str());
        }
    }

    void WindowImpl::setBounds(int x, int y, int width, int height) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        if (is_created_) {
            ::MoveWindow(hWnd_, x_, y_, width_, height_, FALSE);
        }
    }

    void WindowImpl::setCurrentCursor(Cursor cursor)
    {
        // TODO: separate it.
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
        if (is_created_) {
            ::SetLastError(0);
            auto ex_style = ::GetWindowLongPtr(hWnd_, GWL_EXSTYLE);
            if (ex_style == 0) {
                auto err_no = ::GetLastError();
                if (err_no != 0) {
                    LOG(Log::WARNING) << "Failed to get window style: " << err_no;
                    return;
                }
            }

            if (translucent) {
                ex_style |= WS_EX_LAYERED;
            } else {
                ex_style &= ~WS_EX_LAYERED;
            }

            ::SetLastError(0);
            auto result = ::SetWindowLongPtr(hWnd_, GWL_EXSTYLE, ex_style);
            if (result == 0) {
                auto err_no = ::GetLastError();
                if (err_no != 0) {
                    LOG(Log::WARNING) << "Failed to set window style: " << err_no;
                    return;
                }
            }
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

        unsigned int dpi = ::GetDpiForWindow(hWnd_);
        if (dpi == 0) {
            LOG(Log::ERR) << "Failed to get window dpi.";
        }

        return dpi;
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

    bool WindowImpl::isCursorInClient() const {
        RECT clientRect;
        RECT clientInScreenRect;
        POINT cursorPos;
        POINT convertPos;

        ::GetCursorPos(&cursorPos);
        ::GetClientRect(hWnd_, &clientRect);

        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;

        convertPos.x = clientRect.left;
        convertPos.y = clientRect.top;
        ::ClientToScreen(hWnd_, &convertPos);

        clientInScreenRect.left = convertPos.x;
        clientInScreenRect.top = convertPos.y;
        clientInScreenRect.right = clientInScreenRect.left + clientWidth;
        clientInScreenRect.bottom = clientInScreenRect.top + clientHeight;

        return (cursorPos.x >= clientInScreenRect.left
            && cursorPos.x < clientInScreenRect.right
            && cursorPos.y >= clientInScreenRect.top
            && cursorPos.y < clientInScreenRect.bottom);
    }

    bool WindowImpl::isTranslucent() const {
        return is_translucent_;
    }

    void WindowImpl::setMouseCaptureRaw() {
        ::SetCapture(hWnd_);
    }

    void WindowImpl::releaseMouseCaptureRaw() {
        ::ReleaseCapture();
    }

    void WindowImpl::setMouseTrack() {
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
        int client_width, int client_height) {

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


    LRESULT CALLBACK WindowImpl::messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        bool nc_handled = false;
        LRESULT nc_result = 0;

        switch (uMsg) {
        case WM_NCCREATE: {
            onCreate();
            if (delegate_->getFrameType() == Window::FRAME_CUSTOM) {
                non_client_frame_.reset(new DrawableNonClientFrame());
            } else {
                non_client_frame_.reset(new DefaultNonClientFrame());
            }
            nc_result = non_client_frame_->onNcCreate(delegate_, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_CREATE: {
            return TRUE;
        }

        case WM_NCDRAWCLASSIC1:
        case WM_NCDRAWCLASSIC2: {
            nc_result = non_client_frame_->onInterceptDrawClassic(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_NCPAINT: {
            nc_result = non_client_frame_->onNcPaint(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_PAINT: {
            break;
        }

        case WM_NCACTIVATE: {
            nc_result = non_client_frame_->onNcActivate(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_NCHITTEST: {
            nc_result = non_client_frame_->onNcHitTest(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_NCCALCSIZE: {
            nc_result = non_client_frame_->onNcCalSize(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_NCLBUTTONDOWN: {
            nc_result = non_client_frame_->onNcLButtonDown(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_NCLBUTTONUP: {
            nc_result = non_client_frame_->onNcLButtonUp(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_CLOSE: {
            if (onClose()) {
                break;
            }
            return 0;
        }

        case WM_DESTROY: {
            onDestroy();
            return 0;
        }

        case WM_NCDESTROY: {
            nc_result = non_client_frame_->onNcDestroy(&nc_handled);
            if (nc_handled) {
                return nc_result;
            }
            break;
        }

        case WM_SHOWWINDOW: {
            onShow(static_cast<BOOL>(wParam) == TRUE ? true : false);
            break;
        }

        case WM_ACTIVATE: {
            onActivate(LOWORD(wParam));
            break;
        }

        case WM_DPICHANGED: {
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
            break;
        }

        case WM_STYLECHANGED: {
            onStyleChanged(
                wParam & GWL_STYLE, wParam & GWL_EXSTYLE,
                reinterpret_cast<const STYLESTRUCT*>(lParam));
            break;
        }

        case WM_COPYDATA: {
            PCOPYDATASTRUCT cds = reinterpret_cast<PCOPYDATASTRUCT>(lParam);
            if (cds == nullptr) {
                break;
            }

            if (onDataCopy(cds->dwData, cds->cbData, cds->lpData)) {
                return TRUE;
            }

            return FALSE;
        }

        case WM_ERASEBKGND: {
            return TRUE;
        }

        case WM_SETCURSOR: {
            if (isCursorInClient()) {
                setCurrentCursor(cursor_);
                return TRUE;
            }
            break;
        }

        case WM_SETFOCUS: {
            onSetFocus();
            break;
        }

        case WM_KILLFOCUS: {
            onKillFocus();
            break;
        }

        case WM_MOVE: {
            int x_px = LOWORD(lParam);
            int y_px = HIWORD(lParam);
            onMove(x_px, y_px);
            break;
        }

        case WM_SIZE: {
            RECT winRect;
            ::GetWindowRect(hWnd_, &winRect);

            int width_px = winRect.right - winRect.left;
            int height_px = winRect.bottom - winRect.top;
            int client_width_px = LOWORD(lParam);
            int client_height_px = HIWORD(lParam);

            onResize(wParam, width_px, height_px, client_width_px, client_height_px);
            nc_result = non_client_frame_->onSize(wParam, lParam, &nc_handled);
            break;
        }

        case WM_MOVING: {
            auto raw_rect = reinterpret_cast<RECT*>(lParam);
            Rect rect(
                raw_rect->left, raw_rect->top,
                raw_rect->right - raw_rect->left,
                raw_rect->bottom - raw_rect->top);

            bool handled = onMoving(&rect);
            raw_rect->left = rect.left;
            raw_rect->top = rect.top;
            raw_rect->right = rect.right;
            raw_rect->bottom = rect.bottom;

            if (handled) {
                return TRUE;
            }
            break;
        }

        case WM_SIZING: {
            auto raw_rect = reinterpret_cast<RECT*>(lParam);
            Rect rect(
                raw_rect->left, raw_rect->top,
                raw_rect->right - raw_rect->left,
                raw_rect->bottom - raw_rect->top);

            bool handled = onResizing(wParam, &rect);
            raw_rect->left = rect.left;
            raw_rect->top = rect.top;
            raw_rect->right = rect.right;
            raw_rect->bottom = rect.bottom;

            if (handled) {
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONDOWN: {
            if (::GetMessageExtraInfo() != 0) {
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
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP: {
            if (::GetMessageExtraInfo() != 0) {
                return 0;
            }

            nc_result = non_client_frame_->OnLButtonUp(wParam, lParam, &nc_handled);
            if (nc_handled) {
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
                return 0;
            }
            break;
        }

        case WM_RBUTTONDOWN: {
            if (::GetMessageExtraInfo() != 0) {
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
                return 0;
            }
            break;
        }

        case WM_RBUTTONUP: {
            if (::GetMessageExtraInfo() != 0) {
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
                return 0;
            }
            break;
        }

        case WM_MBUTTONDOWN: {
            if (::GetMessageExtraInfo() != 0) {
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
                return 0;
            }
            break;
        }

        case WM_MBUTTONUP: {
            if (::GetMessageExtraInfo() != 0) {
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
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE: {
            if (::GetMessageExtraInfo() != 0) {
                return 0;
            }

            nc_result = non_client_frame_->onMouseMove(wParam, lParam, &nc_handled);
            if (nc_handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_MOVE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return 0;
            }
            break;
        }

        case WM_MOUSELEAVE: {
            if (::GetMessageExtraInfo() != 0) {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_LEAVE_WIN);

            if (onInputEvent(&ev)) {
                return 0;
            }
            break;
        }

        case WM_MOUSEHOVER: {
            if (::GetMessageExtraInfo() != 0) {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_HOVER);

            if (onInputEvent(&ev)) {
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            if (::GetMessageExtraInfo() != 0) {
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
                return 0;
            }
            break;
        }

        case WM_KEYDOWN: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVK_DOWN);
            ev.setKeyboardVirtualKey(wParam, lParam);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_KEYUP: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVK_UP);
            ev.setKeyboardVirtualKey(wParam, lParam);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_CHAR: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVK_CHAR);
            ev.setKeyboardCharKey(wParam, lParam);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_UNICHAR: {
            // Do nothing.
            break;
        }

        case WM_GESTURE: {
            // Do nothing.
            break;
        }

        case WM_TOUCH: {
            UINT input_count = LOWORD(wParam);
            if (input_count == 0) {
                break;
            }

            std::unique_ptr<TOUCHINPUT[]> inputs(new TOUCHINPUT[input_count]);
            if (::GetTouchInputInfo(
                reinterpret_cast<HTOUCHINPUT>(lParam), input_count, inputs.get(), sizeof(TOUCHINPUT)))
            {
                bool handled = onTouch(inputs.get(), input_count);
                ::CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));

                if (handled) {
                    return 0;
                }
            }

            break;
        }

        default:
            break;
        }

        return ::DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }

    LRESULT WindowImpl::processDWMProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP) {

        LRESULT ret = 0;
        HRESULT hr = S_OK;
        bool call_dwp = true; // Pass on to DefWindowProc?

        call_dwp = !::DwmDefWindowProc(hWnd, message, wParam, lParam, &ret);

        // Handle window creation.
        if (message == WM_CREATE) {
            RECT rcClient;
            ::GetWindowRect(hWnd, &rcClient);

            // Inform application of the frame change.
            ::SetWindowPos(hWnd,
                nullptr,
                rcClient.left, rcClient.top,
                rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                SWP_FRAMECHANGED);

            call_dwp = true;
            ret = 0;
        }

        // Handle window activation.
        if (message == WM_ACTIVATE) {
            // Extend the frame into the client area.
            MARGINS margins;

            margins.cxLeftWidth = 0;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            margins.cyTopHeight = 0;

            hr = ::DwmExtendFrameIntoClientArea(hWnd, &margins);
            if (!SUCCEEDED(hr)) {
                // Handle error.
            }

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
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_NCCREATE) {
            //::EnableNonClientDpiScaling(hWnd);
            if (::RegisterTouchWindow(hWnd, TWF_WANTPALM) == 0) {
                LOG(Log::WARNING) << "Failed to register touch window: " << GetLastError();
            }

            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            auto window = reinterpret_cast<WindowImpl*>(cs->lpCreateParams);
            if (!window) {
                DCHECK(Log::FATAL) << "null window creating param.";
                return FALSE;
            }

            window->hWnd_ = hWnd;

            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            return window->messageHandler(uMsg, wParam, lParam);
        }

        if (uMsg == WM_NCDESTROY) {
            if (::UnregisterTouchWindow(hWnd) == 0) {
                LOG(Log::WARNING) << "Failed to unregister touch window: " << GetLastError();
            }
        }

        auto window = reinterpret_cast<WindowImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (!window) {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        bool callDWP = true;
        BOOL dwmEnabled = FALSE;
        LRESULT lRet = 0;
        HRESULT hr = S_OK;
        // Winproc worker for custom frame issues.
        hr = ::DwmIsCompositionEnabled(&dwmEnabled);
        if (SUCCEEDED(hr) && dwmEnabled == TRUE)
        {
            //lRet = window->processDWMProc(hWnd, uMsg, wParam, lParam, &callDWP);
        }

        // Winproc worker for the rest of the application.
        if (callDWP) {
            return window->messageHandler(uMsg, wParam, lParam);
        }

        return lRet;
    }

}