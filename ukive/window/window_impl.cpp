#include "window_impl.h"

#include <dwmapi.h>

#include "ukive/application.h"
#include "ukive/log.h"
#include "ukive/window/window.h"
#include "ukive/window/window_class_manager.h"
#include "ukive/window/frame/non_client_frame.h"
#include "ukive/window/frame/default_non_client_frame.h"
#include "ukive/event/input_event.h"


namespace ukive {

    const int kDefaultX = 0;
    const int kDefaultY = 0;
    const int kDefaultWidth = 400;
    const int kDefaultHeight = 400;
    const string16 kDefaultTitle = L"Ukive Window";

    const int kDefaultClassStyle = CS_HREDRAW | CS_VREDRAW;
    const int kDefaultWindowStyle = WS_OVERLAPPEDWINDOW;
    const int kDefaultWindowExStyle = WS_EX_APPWINDOW;


    WindowImpl::WindowImpl(Window *win)
        :delegate_(win),
        hWnd_(NULL),
        x_(kDefaultX),
        y_(kDefaultY),
        prev_x_(kDefaultX),
        prev_y_(kDefaultY),
        width_(kDefaultWidth),
        height_(kDefaultHeight),
        prev_width_(kDefaultWidth),
        prev_height_(kDefaultHeight),
        title_(kDefaultTitle),
        cursor_(Cursor::ARROW),
        is_created_(false),
        is_showing_(false),
        is_enable_mouse_track_(true) {}

    WindowImpl::~WindowImpl() {}


    void WindowImpl::init() {
        ClassInfo info;
        info.style = kDefaultClassStyle;
        info.icon = info.icon_small = ::LoadIcon(NULL, IDI_WINLOGO);
        info.cursor = ::LoadCursor(NULL, IDC_ARROW);

        int win_style = kDefaultWindowStyle;
        int win_ex_style = kDefaultWindowExStyle;

        onPreCreate(&info, &win_style, &win_ex_style);

        ATOM atom = WindowClassManager::getInstance()->retrieveWindowClass(info);

        non_client_frame_.reset(new DefaultNonClientFrame());
        //non_client_frame_->init(hWnd_);

        HWND hWnd = ::CreateWindowEx(
            win_ex_style,
            reinterpret_cast<wchar_t*>(atom),
            title_.c_str(), win_style,
            x_, y_, width_, height_,
            0, 0, Application::getModuleHandle(), this);
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
        if (!is_created_) {
            return;
        }

        if (onClose()) {
            BOOL ret = ::DestroyWindow(hWnd_);
            if (ret == 0) {
                Log::e(L"failed to destroy window.");
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

    void WindowImpl::setTitle(const string16 &title) {
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
        HCURSOR native_cursor = NULL;
        switch (cursor) {
        case Cursor::ARROW: native_cursor = ::LoadCursor(0, IDC_ARROW); break;
        case Cursor::IBEAM: native_cursor = ::LoadCursor(0, IDC_IBEAM); break;
        case Cursor::WAIT: native_cursor = ::LoadCursor(0, IDC_WAIT); break;
        case Cursor::CROSS: native_cursor = ::LoadCursor(0, IDC_CROSS); break;
        case Cursor::UPARROW: native_cursor = ::LoadCursor(0, IDC_UPARROW); break;
        case Cursor::SIZENWSE: native_cursor = ::LoadCursor(0, IDC_SIZENWSE); break;
        case Cursor::SIZENESW: native_cursor = ::LoadCursor(0, IDC_SIZENESW); break;
        case Cursor::SIZEWE: native_cursor = ::LoadCursor(0, IDC_SIZEWE); break;
        case Cursor::SIZENS: native_cursor = ::LoadCursor(0, IDC_SIZENS); break;
        case Cursor::SIZEALL: native_cursor = ::LoadCursor(0, IDC_SIZEALL); break;
        case Cursor::NO: native_cursor = ::LoadCursor(0, IDC_NO); break;
        case Cursor::HAND: native_cursor = ::LoadCursor(0, IDC_HAND); break;
        case Cursor::APPSTARTING: native_cursor = ::LoadCursor(0, IDC_APPSTARTING); break;
        case Cursor::HELP: native_cursor = ::LoadCursor(0, IDC_HELP); break;
        case Cursor::PIN: native_cursor = ::LoadCursor(0, IDC_PIN); break;
        case Cursor::PERSON: native_cursor = ::LoadCursor(0, IDC_PERSON); break;
        }

        if (native_cursor == NULL) {
            Log::e(L"null native cursor.");
            return;
        }

        cursor_ = cursor;

        ::SetCursor(native_cursor);
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

    int WindowImpl::getClientWidth() {
        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        return rect.right - rect.left;
    }

    int WindowImpl::getClientHeight() {
        RECT rect;
        ::GetClientRect(hWnd_, &rect);
        return rect.bottom - rect.top;
    }

    unsigned int WindowImpl::getDpi() {
        unsigned int dpi = ::GetDpiForWindow(hWnd_);
        if (dpi == 0) {
            Log::e(L"failed to get window dpi.");
        }

        return dpi;
    }

    HWND WindowImpl::getHandle() {
        return hWnd_;
    }

    Cursor WindowImpl::getCurrentCursor() {
        return cursor_;
    }

    bool WindowImpl::isCreated() {
        return is_created_;
    }

    bool WindowImpl::isShowing() {
        return is_showing_;
    }

    bool WindowImpl::isCursorInClient() {
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
            && cursorPos.x <= clientInScreenRect.right
            && cursorPos.y >= clientInScreenRect.top
            && cursorPos.y <= clientInScreenRect.bottom);
    }

    void WindowImpl::setMouseCaptureRaw() {
        ::SetCapture(hWnd_);
    }

    void WindowImpl::releaseMouseCaptureRaw() {
        ::ReleaseCapture();
    }

    void WindowImpl::setMouseTrack()
    {
        if (is_enable_mouse_track_) {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE | TME_HOVER;
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

    float WindowImpl::pxToDp(int px) {
        return px / (getDpi() / 96.f);
    }


    void WindowImpl::onPreCreate(ClassInfo *info, int *win_style, int *win_ex_style) {
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

    bool WindowImpl::onMoving(Rect *rect) {
        return delegate_->onMoving(rect);
    }

    bool WindowImpl::onResizing(WPARAM edge, Rect *rect) {
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

    bool WindowImpl::onInputEvent(InputEvent *e) {
        // 追踪鼠标，以便产生 EVM_LEAVE_WIN 事件。
        if (e->getEvent() == InputEvent::EVM_LEAVE_WIN) {
            is_enable_mouse_track_ = true;
        }
        else if (e->getEvent() == InputEvent::EVM_MOVE) {
            setMouseTrack();
        }

        return delegate_->onInputEvent(e);
    }

    void WindowImpl::onDpiChanged(int dpi_x, int dpi_y) {
        delegate_->onDpiChanged(dpi_x, dpi_y);
    }

    bool WindowImpl::onDataCopy(unsigned int id, unsigned int size, void *data) {
        return delegate_->onDataCopy(id, size, data);
    }


    LRESULT CALLBACK WindowImpl::messageHandler(
        UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CREATE:
            onCreate();
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
            if (onClose()) {
                break;
            }
            return 0;

        case WM_DESTROY:
            onDestroy();
            return 0;

        case WM_SHOWWINDOW:
            onShow((BOOL)wParam == TRUE ? true : false);
            break;

        case WM_ACTIVATE: {
            onActivate(LOWORD(wParam));
            break;
        }

        case WM_DPICHANGED: {
            int new_dpi_x = LOWORD(wParam);
            int new_dpi_y = HIWORD(wParam);

            RECT* const prcNewWindow = (RECT*)lParam;
            SetWindowPos(hWnd_,
                NULL,
                prcNewWindow->left,
                prcNewWindow->top,
                prcNewWindow->right - prcNewWindow->left,
                prcNewWindow->bottom - prcNewWindow->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
            onDpiChanged(new_dpi_x, new_dpi_y);
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

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SETCURSOR: {
            if (isCursorInClient()) {
                setCurrentCursor(cursor_);
                return TRUE;
            }
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
            non_client_frame_->onSize(wParam, lParam);
            break;
        }

        case WM_MOVING: {
            RECT *raw_rect = reinterpret_cast<RECT*>(lParam);
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
            RECT *raw_rect = reinterpret_cast<RECT*>(lParam);
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
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONUP: {
            if (non_client_frame_->OnLButtonUp(wParam, lParam) == TRUE) {
                return TRUE;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setMouseKey(InputEvent::MK_LEFT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_RBUTTONDOWN: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_RBUTTONUP: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setMouseKey(InputEvent::MK_RIGHT);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_MBUTTONDOWN: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_DOWN);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_MBUTTONUP: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_UP);
            ev.setMouseKey(InputEvent::MK_MIDDLE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_MOUSEMOVE: {
            if (non_client_frame_->onMouseMove(wParam, lParam) == TRUE) {
                return TRUE;
            }

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_MOVE);
            ev.setMouseX(GET_X_LPARAM(lParam));
            ev.setMouseY(GET_Y_LPARAM(lParam));
            ev.setMouseRawX(GET_X_LPARAM(lParam));
            ev.setMouseRawY(GET_Y_LPARAM(lParam));

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_MOUSELEAVE: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_LEAVE_WIN);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_MOUSEHOVER: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVM_HOVER);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            ::POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ::ScreenToClient(hWnd_, &pt);

            InputEvent ev;
            ev.setEvent(InputEvent::EVM_WHEEL);
            ev.setMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
            ev.setMouseX(pt.x);
            ev.setMouseY(pt.y);
            ev.setMouseRawX(pt.x);
            ev.setMouseRawY(pt.y);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_KEYDOWN: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVK_DOWN);
            ev.setKeyboardKey(wParam, lParam);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }

        case WM_KEYUP: {
            InputEvent ev;
            ev.setEvent(InputEvent::EVK_UP);
            ev.setKeyboardKey(wParam, lParam);

            if (onInputEvent(&ev)) {
                return TRUE;
            }
            break;
        }
        }

        return ::DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }

    LRESULT WindowImpl::processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP)
    {
        LRESULT lRet = 0;
        HRESULT hr = S_OK;
        bool fCallDWP = true; // Pass on to DefWindowProc?

        fCallDWP = !::DwmDefWindowProc(hWnd, message, wParam, lParam, &lRet);

        // Handle window creation.
        if (message == WM_CREATE)
        {
            RECT rcClient;
            ::GetWindowRect(hWnd, &rcClient);

            // Inform application of the frame change.
            ::SetWindowPos(hWnd,
                0,
                rcClient.left, rcClient.top,
                rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                SWP_FRAMECHANGED);

            fCallDWP = true;
            lRet = 0;
        }

        // Handle window activation.
        if (message == WM_ACTIVATE)
        {
            // Extend the frame into the client area.
            MARGINS margins;

            margins.cxLeftWidth = 8;
            margins.cxRightWidth = 8;
            margins.cyBottomHeight = 8;
            margins.cyTopHeight = 8;

            hr = ::DwmExtendFrameIntoClientArea(hWnd, &margins);

            if (!SUCCEEDED(hr))
            {
                // Handle error.
            }

            fCallDWP = true;
            lRet = 0;
        }

        if (message == WM_PAINT)
        {
            fCallDWP = true;
            lRet = 0;
        }

        // Handle the non-client size message.
        if (message == WM_NCCALCSIZE && wParam == TRUE)
        {
            // Calculate new NCCALCSIZE_PARAMS based on custom NCA inset.
            NCCALCSIZE_PARAMS *pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

            RECT newW;
            ::CopyRect(&newW, &pncsp->rgrc[0]);
            //new window client.
            pncsp->rgrc[0].left = newW.left + 0;
            pncsp->rgrc[0].top = newW.top + 0;
            pncsp->rgrc[0].right = newW.right - 0;
            pncsp->rgrc[0].bottom = newW.bottom - 0;

            lRet = 0;

            // No need to pass the message on to the DefWindowProc.
            fCallDWP = false;
        }

        // Handle hit testing in the NCA if not handled by DwmDefWindowProc.
        if ((message == WM_NCHITTEST) && (lRet == 0))
        {
            lRet = HitTestNCA(hWnd, wParam, lParam, 8, 8, 8, 8);

            if (lRet != HTNOWHERE)
            {
                fCallDWP = false;
            }
        }

        *pfCallDWP = fCallDWP;

        return lRet;
    }

    LRESULT WindowImpl::HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam,
        int leftExt, int topExt, int rightExt, int bottomExt)
    {
        // Get the point coordinates for the hit test.
        POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        // Get the window rectangle.
        RECT rcWindow;
        ::GetWindowRect(hWnd, &rcWindow);

        // Get the frame rectangle, adjusted for the style without a caption.
        //RECT rcFrame = { 0 };
        //AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

        // Determine if the hit test is for resizing. Default middle (1,1).
        USHORT uRow = 1;
        USHORT uCol = 1;
        //bool fOnResizeBorder = false;

        // Determine if the point is at the top or bottom of the window.
        if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top + topExt)
        {
            //fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
            uRow = 0;
        }
        else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom - bottomExt)
        {
            uRow = 2;
        }

        // Determine if the point is at the left or right of the window.
        if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left + leftExt)
        {
            uCol = 0; // left side
        }
        else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right - rightExt)
        {
            uCol = 2; // right side
        }

        // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
        LRESULT hitTests[3][3] =
        {
            { HTTOPLEFT,    HTCAPTION,    HTTOPRIGHT },
            { HTLEFT,       HTNOWHERE,     HTRIGHT },
            { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
        };

        return hitTests[uRow][uCol];
    }

    LRESULT CALLBACK WindowImpl::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_NCCREATE)
        {
            //::EnableNonClientDpiScaling(hWnd);

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

        bool callDWP = true;
        BOOL dwmEnabled = FALSE;
        LRESULT lRet = 0;
        HRESULT hr = S_OK;
        // Winproc worker for custom frame issues.
        /*hr = DwmIsCompositionEnabled(&dwmEnabled);
        if (SUCCEEDED(hr))
        {
        lRet = sUWCVtr->processDWMProc(hWnd, uMsg, wParam, lParam, &callDWP);
        }*/

        // Winproc worker for the rest of the application.
        if (callDWP) {
            return window->messageHandler(uMsg, wParam, lParam);
        }

        return 0;
    }

}