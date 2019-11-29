#include "window_impl.h"

#include <algorithm>

#include <dwmapi.h>
#include <VersionHelpers.h>

#include "utils/log.h"
#include "utils/stl_utils.h"

#include "ukive/application.h"
#include "ukive/window/window.h"
#include "ukive/window/window_class_manager.h"
#include "ukive/window/frame/non_client_frame.h"
#include "ukive/window/frame/default_non_client_frame.h"
#include "ukive/window/frame/drawable_non_client_frame.h"
#include "ukive/event/input_event.h"
#include "ukive/system/win10_version.h"
#include "ukive/system/dynamic_windows_api.h"
#include "ukive/system/time_utils.h"

#define MI_WP_SIGNATURE  0xFF515700
#define SIGNATURE_MASK   0xFFFFFF00
#define TOUCH_PEN_MASK   0x80


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
          is_first_nccalc_(true)
    {
        DCHECK(delegate_);
    }

    WindowImpl::~WindowImpl() {}


    void WindowImpl::init() {
        ClassInfo info;
        info.style = kDefaultClassStyle;

        string16 icon_name, small_icon_name;
        if (delegate_->onGetWindowIconName(&icon_name, &small_icon_name)) {
            info.icon = ::LoadIcon(nullptr, icon_name.c_str());
            info.icon_small = ::LoadIcon(nullptr, small_icon_name.c_str());
        } else {
            info.icon = info.icon_small = ::LoadIcon(nullptr, IDI_WINLOGO);
        }

        info.cursor = ::LoadCursor(nullptr, IDC_ARROW);

        int win_style = kDefaultWindowStyle;
        int win_ex_style = kDefaultWindowExStyle;

        if (is_translucent_) {
            win_ex_style |= WS_EX_LAYERED;
        }

        //onPreCreate(&info, &win_style, &win_ex_style);

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

    void WindowImpl::setBlurBehindEnabled(bool enabled) {
        if (is_blur_behind_enabled_ != enabled) {
            is_blur_behind_enabled_ = enabled;

            if (::IsWindow(hWnd_)) {
                ::SendMessageW(
                    hWnd_, WM_ACTIVATE, MAKEWORD(WA_INACTIVE, ::IsIconic(hWnd_)), LPARAM(hWnd_));
            }
        }
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

    int WindowImpl::getClientOffX() const {
        POINT p;
        non_client_frame_->getClientOffset(&p);
        return p.x;
    }

    int WindowImpl::getClientOffY() const {
        POINT p;
        non_client_frame_->getClientOffset(&p);
        return p.y;
    }

    int WindowImpl::getClientWidth(bool total) const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        int width = rect.right - rect.left;
        if (width == 0) {
            return 0;
        }

        if (!total) {
            non_client_frame_->getClientInsets(&rect);
            width -= (rect.left + rect.right);
        }

        return std::max(width, 0);
    }

    int WindowImpl::getClientHeight(bool total) const {
        if (!::IsWindow(hWnd_)) {
            return 0;
        }

        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        int height = rect.bottom - rect.top;
        if (height == 0) {
            return 0;
        }

        if (!total) {
            non_client_frame_->getClientInsets(&rect);
            height -= (rect.top + rect.bottom);
        }

        return std::max(height, 0);
    }

    void WindowImpl::getDpi(int* dpi_x, int* dpi_y) const {
        if (!::IsWindow(hWnd_)) {
            Application::getPrimaryDpi(dpi_x, dpi_y);
            return;
        }

        static bool is_win10_1607_or_above = win::isWin10Ver1607OrGreater();
        if (is_win10_1607_or_above) {
            int dpi = utl::STLCInt(win::UDGetDpiForWindow(hWnd_));
            if (dpi > 0) {
                *dpi_x = dpi;
                *dpi_y = dpi;
                return;
            }
        }

        HDC dc = ::GetDC(hWnd_);
        *dpi_x = ::GetDeviceCaps(dc, LOGPIXELSX);
        *dpi_y = ::GetDeviceCaps(dc, LOGPIXELSY);
        ::ReleaseDC(hWnd_, dc);
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
            return ::IsIconic(hWnd_) != 0;
        }

        DCHECK(false);
        return false;
    }

    bool WindowImpl::isMaximum() const {
        if (::IsWindow(hWnd_)) {
            return ::IsZoomed(hWnd_) != 0;
        }

        DCHECK(false);
        return false;
    }

    bool WindowImpl::isPopup() const {
        if (::IsWindow(hWnd_)) {
            return ::GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_POPUP;
        }

        DCHECK(false);
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
        if (!::IsWindow(hWnd_)) {
            return;
        }

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

    void WindowImpl::disableTouchFeedback(HWND hWnd) {
        if (::IsWindows8OrGreater()) {
            BOOL enabled = FALSE;
            win::UDSetWindowFeedbackSetting(hWnd, FEEDBACK_TOUCH_TAP, 0, sizeof(BOOL), &enabled);
            win::UDSetWindowFeedbackSetting(hWnd, FEEDBACK_TOUCH_PRESSANDHOLD, 0, sizeof(BOOL), &enabled);
            win::UDSetWindowFeedbackSetting(hWnd, FEEDBACK_TOUCH_RIGHTTAP, 0, sizeof(BOOL), &enabled);
        }
    }

    int WindowImpl::getPointerTypeFromMouseMsg() {
        int pointer_type;
        auto info = ::GetMessageExtraInfo();
        if ((info & SIGNATURE_MASK) == MI_WP_SIGNATURE) {
            // Generated by Pen or Touch
            if (info & TOUCH_PEN_MASK) {
                // Touch
                pointer_type = InputEvent::PT_TOUCH;
            } else {
                // Pen
                pointer_type = InputEvent::PT_PEN;
            }
        } else {
            // Generated by Mouse
            pointer_type = InputEvent::PT_MOUSE;
        }
        return pointer_type;
    }

    bool WindowImpl::isMouseTrackEnabled() {
        return is_enable_mouse_track_;
    }

    float WindowImpl::dpToPxX(float dp) {
        int dpi_x, dpi_y;
        getDpi(&dpi_x, &dpi_y);
        return dpi_x / 96.f * dp;
    }

    float WindowImpl::dpToPxY(float dp) {
        int dpi_x, dpi_y;
        getDpi(&dpi_x, &dpi_y);
        return dpi_y / 96.f * dp;
    }

    float WindowImpl::pxToDpX(float px) {
        int dpi_x, dpi_y;
        getDpi(&dpi_x, &dpi_y);
        return px / (dpi_x / 96.f);
    }

    float WindowImpl::pxToDpY(float px) {
        int dpi_x, dpi_y;
        getDpi(&dpi_x, &dpi_y);
        return px / (dpi_y / 96.f);
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

    void WindowImpl::onDraw(const Rect& rect) {
        delegate_->onDraw(rect);
    }

    void WindowImpl::onMove(int x, int y) {
        prev_x_ = x_;
        x_ = x;
        prev_y_ = y_;
        y_ = y;

        delegate_->onMove(x, y);
    }

    void WindowImpl::onResize(int param, int width, int height) {
        prev_width_ = width_;
        prev_height_ = height_;
        width_ = width;
        height_ = height;

        delegate_->onResize(param, width, height);
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
        bool consumed = false;
        for (int i = 0; i < size; ++i) {
            InputEvent ev;
            ev.setPointerType(InputEvent::PT_TOUCH);
            bool has_move = false;
            bool has_down_up = false;
            auto& input = inputs[i];
            if (input.dwFlags & TOUCHEVENTF_DOWN) {
                //DLOG(Log::INFO) << "TOUCH DOWN";

                DCHECK(!has_down_up);
                DCHECK(prev_ti_.find(input.dwID) == prev_ti_.end());
                has_down_up = true;
                ev.setEvent(InputEvent::EVT_DOWN);
                ev.setCurTouchId(utl::STLCInt(input.dwID));
                prev_ti_[input.dwID] = input;
            } else if (input.dwFlags & TOUCHEVENTF_UP) {
                //DLOG(Log::INFO) << "TOUCH UP";

                DCHECK(!has_down_up);
                auto it = prev_ti_.find(input.dwID);
                DCHECK(it != prev_ti_.end());
                has_down_up = true;
                ev.setEvent(InputEvent::EVT_UP);
                ev.setCurTouchId(utl::STLCInt(input.dwID));
                prev_ti_.erase(it);
            } else if (input.dwFlags & TOUCHEVENTF_MOVE) {
                if (!has_down_up && !has_move) {
                    auto it = prev_ti_.find(input.dwID);
                    DCHECK(it != prev_ti_.end());
                    if (input.x != it->second.x || input.y != it->second.y) {
                        has_move = true;
                        ev.setEvent(InputEvent::EVT_MOVE);
                        ev.setCurTouchId(utl::STLCInt(input.dwID));
                    }
                }
                prev_ti_[input.dwID] = input;
            }

            if (has_down_up || has_move) {
                int tx = TOUCH_COORD_TO_PIXEL(input.x);
                int ty = TOUCH_COORD_TO_PIXEL(input.y);

                POINT pt = { tx, ty };
                ::ScreenToClient(hWnd_, &pt);

                if (/*(input.dwFlags & TOUCHEVENTF_PRIMARY) &&*/ (input.dwFlags & TOUCHEVENTF_UP)) {
                    is_prev_touched_ = true;
                    prev_touch_time_ = TimeUtils::upTimeMillis();
                }

                ev.setRawX(pt.x);
                ev.setRawY(pt.y);
                ev.setX(pt.x);
                ev.setY(pt.y);

                consumed |= onInputEvent(&ev);
            }
        }

        return consumed;
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
        if (::GetUpdateRect(hWnd_, nullptr, FALSE) == 0) {
            return 0;
        }

        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hWnd_, &ps);

        onDraw(Rect(
            ps.rcPaint.left, ps.rcPaint.top,
            ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top));

        ::EndPaint(hWnd_, &ps);
        *handled = true;
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
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            nc_result = non_client_frame_->OnLButtonUp(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_RBUTTONDOWN:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_RBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MBUTTONDOWN:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MBUTTONUP:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                prev_touch_x_ = GET_X_LPARAM(lParam);
                prev_touch_y_ = GET_Y_LPARAM(lParam);
                return 0;
            }

            // 有时，在某些具有触屏的设备上启动程序时，以及接收到 WM_TOUCH 后，
            // Windows 会随机触发 WM_MOUSEMOVE 或 WM_LBUTTONUP 事件，而这些
            // 事件会被 GetMessageExtraInfo() 判定为鼠标事件（尽管这些事件是由操作触屏引发的）。
            // 以下网址中包含有相关讨论和解决方法：
            // https://social.msdn.microsoft.com/Forums/en-US/1b7217bb-1e60-4e00-83c9-193c7f88c249
            if (is_prev_touched_) {
                if (TimeUtils::upTimeMillis() - prev_touch_time_ <= 1000) {
                    if (GET_X_LPARAM(lParam) == prev_touch_x_ && GET_Y_LPARAM(lParam) == prev_touch_y_) {
                        is_prev_touched_ = false;
                        return 0;
                    }
                } else {
                    is_prev_touched_ = false;
                }
            }

            nc_result = non_client_frame_->onMouseMove(wParam, lParam, handled);
            if (*handled) {
                return nc_result;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_MOVE);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setX(GET_X_LPARAM(lParam));
            ev.setY(GET_Y_LPARAM(lParam));
            ev.setRawX(GET_X_LPARAM(lParam));
            ev.setRawY(GET_Y_LPARAM(lParam));
            if (onInputEvent(&ev)) {
                *handled = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL:
        {
            int p_type = getPointerTypeFromMouseMsg();
            if (p_type != InputEvent::PT_MOUSE &&
                p_type != InputEvent::PT_PEN)
            {
                return 0;
            }

            int wheel = GET_WHEEL_DELTA_WPARAM(wParam);
            bool is_wheel = (std::abs(wheel) % WHEEL_DELTA) == 0;

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_WHEEL);
            ev.setPointerType(InputEvent::PT_MOUSE);
            ev.setMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), is_wheel);

            ::POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ::ScreenToClient(hWnd_, &pt);

            ev.setX(pt.x);
            ev.setY(pt.y);
            ev.setRawX(pt.x);
            ev.setRawY(pt.y);
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
        int p_type = getPointerTypeFromMouseMsg();
        if (p_type != InputEvent::PT_MOUSE &&
            p_type != InputEvent::PT_PEN)
        {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVM_HOVER);
        ev.setPointerType(InputEvent::PT_MOUSE);
        if (onInputEvent(&ev)) {
            *handled = true;
            return 0;
        }

        return 0;
    }

    LRESULT WindowImpl::onMouseLeave(WPARAM wParam, LPARAM lParam, bool* handled) {
        int p_type = getPointerTypeFromMouseMsg();
        if (p_type != InputEvent::PT_MOUSE &&
            p_type != InputEvent::PT_PEN)
        {
            return 0;
        }

        InputEvent ev;
        ev.setEvent(InputEvent::EVM_LEAVE_WIN);
        ev.setPointerType(InputEvent::PT_MOUSE);
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

        int width_px = w_rect.right - w_rect.left;
        int height_px = w_rect.bottom - w_rect.top;

        onResize(wParam, width_px, height_px);
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
        ev.setPointerType(InputEvent::PT_KEYBOARD);
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
        ev.setPointerType(InputEvent::PT_KEYBOARD);
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
        ev.setPointerType(InputEvent::PT_KEYBOARD);
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

        if (input_count > ti_cache_.size) {
            ti_cache_.cache.reset(new TOUCHINPUT[input_count]);
            ti_cache_.size = input_count;
        }

        if (::GetTouchInputInfo(
            reinterpret_cast<HTOUCHINPUT>(lParam),
            input_count,
            ti_cache_.cache.get(),
            sizeof(TOUCHINPUT)))
        {
            *handled = onTouch(ti_cache_.cache.get(), utl::STLCInt(input_count));
            ::CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));

            if (*handled) {
                return 0;
            }
        }
        return 0;
    }

    LRESULT WindowImpl::onDwmCompositionChanged(WPARAM wParam, LPARAM lParam, bool* handled) {
        if (is_translucent_ && (GetWindowLongPtr(hWnd_, GWL_STYLE) & WS_CAPTION)) {
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
        bool call_dwp = !::DwmDefWindowProc(hWnd, message, wParam, lParam, &ret);

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
            non_client_frame_->onActivateAfterDwm();

            if (is_blur_behind_enabled_) {
                static bool is_win8_or_greater = IsWindows8OrGreater();
                static bool is_win10_or_greater = IsWindows10OrGreater();
                if (is_win10_or_greater) {
                    EnableBlurBehindOnWin10();
                } else if (!is_win8_or_greater) {
                    EnableBlurBehindOnWin7();
                }
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
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        WindowImpl* window = nullptr;
        if (uMsg == WM_NCCREATE) {
            //::EnableNonClientDpiScaling(hWnd);
            disableTouchFeedback(hWnd);
            if (::RegisterTouchWindow(hWnd, TWF_FINETOUCH) == 0) {
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
            if (Application::isAeroEnabled()) {
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

    void WindowImpl::EnableBlurBehindOnWin7() {
        // Create and populate the Blur Behind structure
        DWM_BLURBEHIND bb = { 0 };

        // Enable Blur Behind and apply to the entire client area
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = true;
        bb.hRgnBlur = nullptr;

        // Apply Blur Behind
        HRESULT hr = DwmEnableBlurBehindWindow(hWnd_, &bb);
        DCHECK(SUCCEEDED(hr));
    }

    void WindowImpl::EnableBlurBehindOnWin10() {
        win::ACCENT_POLICY accent;
        accent.AccentState = win::ACCENT_ENABLE_BLURBEHIND;
        accent.AccentFlags = 0;
        accent.AnimationId = 0;
        // AABBGGRR
        accent.GradientColor = 0;

        win::WINDOWCOMPOSITIONATTRIBDATA data;
        data.Attrib = win::WCA_ACCENT_POLICY;
        data.pvData = &accent;
        data.cbData = sizeof(accent);

        BOOL result = win::UDSetWindowCompositionAttribute(hWnd_, &data);
        DCHECK(result != 0);
    }

}
