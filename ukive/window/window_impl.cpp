#include "window_impl.h"

#include <dwmapi.h>

#include "ukive/application.h"
#include "ukive/event/input_event.h"
#include "ukive/log.h"
#include "ukive/window/window.h"
#include "ukive/window/window_class_manager.h"
#include "ukive/window/window_manager.h"
#include "ukive/window/frame/non_client_frame.h"
#include "ukive/window/frame/default_non_client_frame.h"
#include "ukive/views/layout/base_layout.h"
#include "ukive/text/text_action_mode.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/canvas.h"
#include "ukive/text/text_action_mode_callback.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/message/message.h"
#include "ukive/menu/context_menu.h"
#include "ukive/menu/context_menu_callback.h"
#include "ukive/menu/menu.h"
#include "ukive/graphics/color.h"


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
        is_created_(false),
        is_showing_(false),
        is_startup_window_(false) {
        WindowManager::getInstance()->addWindow(this);

        cursor_ = ::LoadCursor(NULL, IDC_ARROW);
        background_color_ = Color::White;

        mMouseHolder = nullptr;
        mFocusHolder = nullptr;
        mFocusHolderBackup = nullptr;
        mMouseHolderRef = 0;

        is_enable_mouse_track_ = true;

        mLabourCycler = nullptr;
        mBaseLayout = nullptr;
        mAnimationManager = nullptr;
        mAnimStateChangedListener = nullptr;
        mAnimTimerEventListener = nullptr;

        mCanvas = nullptr;
        mRenderer = nullptr;
        mTextActionMode = nullptr;
        mContextMenu = nullptr;
    }

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
        close(true);
    }

    void WindowImpl::close(bool notify) {
        if (!is_created_) {
            return;
        }

        if (notify ? onClose() : true) {
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

    void WindowImpl::setCurrentCursor(const string16 &cursor)
    {
        cursor_ = ::LoadCursor(0, cursor.c_str());
        ::SetCursor(cursor_);
    }

    void WindowImpl::setContentView(View *content)
    {
        mBaseLayout->addContent(content);
    }

    void WindowImpl::setBackgroundColor(const Color &color)
    {
        background_color_ = color;
        invalidate();
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

    HCURSOR WindowImpl::getCurrentCursor() {
        return cursor_;
    }

    Color WindowImpl::getBackgroundColor() {
        return background_color_;
    }

    BaseLayout *WindowImpl::getBaseLayout() {
        return mBaseLayout;
    }

    AnimationManager *WindowImpl::getAnimationManager() {
        return mAnimationManager;
    }

    Cycler *WindowImpl::getCycler() {
        return mLabourCycler;
    }

    Renderer *WindowImpl::getRenderer() {
        return mRenderer;
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

    void WindowImpl::notifySizeChanged(
        int param, int width, int height,
        int clientWidth, int clientHeight) {

        if (clientWidth <= 0 || clientHeight <= 0) {
            return;
        }

        prev_width_ = width_;
        prev_height_ = height_;
        width_ = width;
        height_ = height;

        onResize(param, width, height, clientWidth, clientHeight);

        performLayout();
        performRefresh();
    }

    void WindowImpl::notifyLocationChanged(int x, int y) {
        prev_x_ = x_;
        x_ = x;
        prev_y_ = y_;
        y_ = y;

        onMove(x, y);
    }

    void WindowImpl::invalidate() {
        mLabourCycler->removeMessages(SCHEDULE_RENDER);
        mLabourCycler->sendEmptyMessage(SCHEDULE_RENDER);
    }

    void WindowImpl::invalidate(int left, int top, int right, int bottom) {
    }

    void WindowImpl::requestLayout() {
        mLabourCycler->removeMessages(SCHEDULE_LAYOUT);
        mLabourCycler->sendEmptyMessage(SCHEDULE_LAYOUT);
    }

    void WindowImpl::performLayout() {
        if (!is_created_) {
            return;
        }

        LayoutParams *params = mBaseLayout->getLayoutParams();

        int width = getClientWidth();
        int height = getClientHeight();
        int widthSpec = View::EXACTLY;
        int heightSpec = View::EXACTLY;

        if (params->width < 0) {
            switch (params->width) {
            case LayoutParams::FIT_CONTENT:
                widthSpec = View::FIT;
                break;

            case LayoutParams::MATCH_PARENT:
                widthSpec = View::EXACTLY;
                break;
            }
        }
        else {
            width = params->width;
            widthSpec = View::EXACTLY;
        }

        if (params->height < 0) {
            switch (params->height) {
            case LayoutParams::FIT_CONTENT:
                heightSpec = View::FIT;
                break;

            case LayoutParams::MATCH_PARENT:
                heightSpec = View::EXACTLY;
                break;
            }
        }
        else {
            height = params->height;
            heightSpec = View::EXACTLY;
        }

        mBaseLayout->measure(width, height, widthSpec, heightSpec);

        int measuredWidth = mBaseLayout->getMeasuredWidth();
        int measuredHeight = mBaseLayout->getMeasuredHeight();

        mBaseLayout->layout(0, 0, measuredWidth, measuredHeight);
    }

    void WindowImpl::performRefresh() {
        if (!is_created_) {
            return;
        }

        Rect rect(0, 0, width_, height_);
        onDraw(rect);
    }

    void WindowImpl::performRefresh(int left, int top, int right, int bottom) {
        if (!is_created_) {
            return;
        }

        Rect rect(left, top, right - left, bottom - top);
        onDraw(rect);
    }


    View *WindowImpl::findViewById(int id) {
        return mBaseLayout->findViewById(id);
    }


    void WindowImpl::captureMouse(View *v) {
        if (v == nullptr) {
            return;
        }

        //当已存在有捕获鼠标的Widget时，若此次调用该方法的Widget
        //与之前不同，此次调用将被忽略。在使用中应避免此种情况产生。
        if (mMouseHolderRef != 0
            && v != mMouseHolder) {
            ::OutputDebugString(L"abnormal capture mouse!!\n");
            return;
        }

        ++mMouseHolderRef;
        ::OutputDebugString(L"capture mouse!!\n");

        //该Widget第一次捕获鼠标。
        if (mMouseHolderRef == 1) {
            ::SetCapture(hWnd_);
            mMouseHolder = v;
        }
    }

    void WindowImpl::releaseMouse() {
        if (mMouseHolderRef == 0) {
            return;
        }

        --mMouseHolderRef;
        ::OutputDebugString(L"release mouse!!\n");

        //鼠标将被释放。
        if (mMouseHolderRef == 0) {
            ::ReleaseCapture();
            mMouseHolder = nullptr;
        }
    }

    View *WindowImpl::getMouseHolder() {
        return mMouseHolder;
    }

    unsigned int WindowImpl::getMouseHolderRef() {
        return mMouseHolderRef;
    }


    void WindowImpl::captureKeyboard(View *widget) {
        mFocusHolder = widget;
        //::OutputDebugString(L"captureKeyboard!!\n");
    }

    void WindowImpl::releaseKeyboard() {
        mFocusHolder = nullptr;
        //::OutputDebugString(L"releaseKeyboard!!\n");
    }

    View *WindowImpl::getKeyboardHolder() {
        return mFocusHolder;
    }

    ContextMenu *WindowImpl::startContextMenu(
        ContextMenuCallback *callback, View *anchor, View::Gravity gravity)
    {
        ContextMenu *contextMenu
            = new ContextMenu(delegate_, callback);

        if (!callback->onCreateContextMenu(
            contextMenu, contextMenu->getMenu())) {
            delete contextMenu;
            return nullptr;
        }

        callback->onPrepareContextMenu(
            contextMenu, contextMenu->getMenu());

        if (contextMenu->getMenu()->getItemCount() == 0) {
            delete contextMenu;
            return nullptr;
        }

        mContextMenu = contextMenu;

        int x, y;
        Rect rect = anchor->getBoundInWindow();

        y = rect.bottom + 1;

        switch (gravity)
        {
        case View::LEFT:
            x = rect.left;
            break;

        case View::RIGHT:
            x = rect.right - 92;
            break;

        case View::CENTER:
            x = rect.left - (92 - rect.width()) / 2.f;
            break;

        default:
            x = rect.left;
        }

        //异步打开 ContextMenu，以防止在输入事件处理流程中
        //打开菜单时出现问题。
        class ContextMenuWorker
            : public Executable
        {
        public:
            ContextMenuWorker(WindowImpl *w, int x, int y)
                :x_(x), y_(y), window(w) {}

            void run() override {
                window->mContextMenu->show(x_, y_);
            }
        private:
            int x_, y_;
            WindowImpl *window;
        }*worker = new ContextMenuWorker(this, x, y);

        mLabourCycler->post(worker);

        return contextMenu;
    }

    TextActionMode *WindowImpl::startTextActionMode(
        TextActionModeCallback *callback)
    {
        TextActionMode *actionMode
            = new TextActionMode(delegate_, callback);

        if (!callback->onCreateActionMode(
            actionMode, actionMode->getMenu())) {
            delete actionMode;
            return nullptr;
        }

        callback->onPrepareActionMode(
            actionMode, actionMode->getMenu());

        if (actionMode->getMenu()->getItemCount() == 0) {
            delete actionMode;
            return nullptr;
        }

        mTextActionMode = actionMode;

        //异步打开TextActionMode菜单，以防止在输入事件处理流程中
        //打开菜单时出现问题。
        class TextActionModeWorker
            : public Executable
        {
        public:
            TextActionModeWorker(WindowImpl *w)
                :window(w) {}

            void run() override {
                window->mTextActionMode->show();
            }
        private:
            WindowImpl *window;
        }*worker = new TextActionModeWorker(this);

        mLabourCycler->post(worker);

        return actionMode;
    }

    float WindowImpl::dpToPx(float dp) {
        return getDpi() / 96.f * dp;
    }

    float WindowImpl::pxToDp(int px) {
        return px / (getDpi() / 96.f);
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


    void WindowImpl::onPreCreate(ClassInfo *info, int *win_style, int *win_ex_style) {
        delegate_->onPreCreate(info, win_style, win_ex_style);
    }

    void WindowImpl::onCreate() {
        mLabourCycler = new UpdateCycler(this);

        // TODO: 使用更好的 view id 机制
        mBaseLayout = new BaseLayout(delegate_, 0);
        mBaseLayout->setLayoutParams(
            new LayoutParams(
                LayoutParams::MATCH_PARENT,
                LayoutParams::MATCH_PARENT));

        /*UFrameLayout *titleBar = new UFrameLayout(this);
        titleBar->setBackground(new UColorDrawable(this, UColor::Blue100));
        UBaseLayoutParams *titleBarLp = new UBaseLayoutParams(
        UBaseLayoutParams::MATCH_PARENT, 50);
        mBaseLayout->addContent(titleBar, titleBarLp);*/

        mAnimationManager = new AnimationManager();
        HRESULT hr = mAnimationManager->init();
        if (FAILED(hr))
            throw std::runtime_error("UWindow-onCreate(): init animation manager failed.");

        mAnimStateChangedListener = new AnimStateChangedListener(this);
        mAnimationManager->setOnStateChangedListener(mAnimStateChangedListener);

        //mAnimationManager->connectTimer(true);
        mAnimTimerEventListener = new AnimTimerEventListener(this);
        //mAnimationManager->setTimerEventListener(mAnimTimerEventListener);

        mRenderer = new Renderer();
        hr = mRenderer->init(this);
        if (FAILED(hr))
            throw std::runtime_error("UWindow-onCreate(): Init DirectX renderer failed.");

        auto deviceContext = mRenderer->getD2DDeviceContext();

        mCanvas = new Canvas(deviceContext.cast<ID2D1RenderTarget>());

        mBaseLayout->onAttachedToWindow();

        delegate_->onCreate();
    }

    void WindowImpl::onShow(bool show) {
        delegate_->onShow(show);
    }

    void WindowImpl::onActivate(int param) {
        switch (param)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            mBaseLayout->dispatchWindowFocusChanged(true);
            /*if (mFocusHolderBackup)
            mFocusHolderBackup->requestFocus();*/
            break;
        case WA_INACTIVE:
            while (mMouseHolderRef > 0)
                this->releaseMouse();
            mBaseLayout->dispatchWindowFocusChanged(false);
            /*if (mFocusHolder)
            {
            UInputEvent ev;
            ev.setEvent(UInputEvent::EVENT_CANCEL);

            mFocusHolderBackup = mFocusHolder;
            mFocusHolderBackup->dispatchInputEvent(&ev);
            mFocusHolder->discardFocus();
            }*/
            break;
        }

        delegate_->onActivate(param);
    }

    void WindowImpl::onDraw(const Rect &rect) {
        if (is_created_)
        {
            getAnimationManager()->update();

            bool ret = mRenderer->render(
                background_color_, [this]() {

                if (mCanvas && mBaseLayout->isLayouted()) {
                    mBaseLayout->draw(mCanvas);
                }

                delegate_->onDraw(mCanvas);
            });

            if (!ret) {
                Log::e(L"failed to render.");
                return;
            }

            if (getAnimationManager()->isBusy()) {
                invalidate();
            }
        }
    }

    void WindowImpl::onMove(int x, int y) {
        delegate_->onMove(x, y);
    }

    void WindowImpl::onResize(
        int param, int width, int height,
        int clientWidth, int clientHeight) {

        HRESULT hr = mRenderer->resize();
        if (FAILED(hr))
            throw std::runtime_error("UWindow-onResize(): Resize DirectX Renderer failed.");

        delegate_->onResize(param, width, height, clientWidth, clientHeight);
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
        delegate_->onDestroy();

        delete mBaseLayout;

        delete mCanvas;
        mCanvas = nullptr;

        mRenderer->close();
        delete mRenderer;
        mRenderer = nullptr;

        delete mAnimTimerEventListener;
        delete mAnimStateChangedListener;

        mAnimationManager->setOnStateChangedListener(0);
        mAnimationManager->close();
        delete mAnimationManager;
        delete mLabourCycler;
    }

    bool WindowImpl::onInputEvent(InputEvent *e) {
        //追踪鼠标，以便产生EVENT_MOUSE_LEAVE_WINDOW事件。
        if (e->getEvent() == InputEvent::EVM_LEAVE_WIN)
            is_enable_mouse_track_ = true;
        else if (e->getEvent() == InputEvent::EVM_MOVE)
            this->setMouseTrack();

        if (e->isMouseEvent())
        {
            //若有之前捕获过鼠标的Widget存在，则直接将所有事件
            //直接发送至该Widget。
            if (mMouseHolder
                && mMouseHolder->getVisibility() == View::VISIBLE
                && mMouseHolder->isEnabled())
            {
                //进行坐标变换，将目标Widget左上角映射为(0, 0)。
                int totalLeft = 0;
                int totalTop = 0;
                View *parent = mMouseHolder->getParent();
                while (parent)
                {
                    totalLeft += (parent->getLeft() - parent->getScrollX());
                    totalTop += (parent->getTop() - parent->getScrollY());

                    parent = parent->getParent();
                }

                e->setMouseX(e->getMouseX() - totalLeft);
                e->setMouseY(e->getMouseY() - totalTop);
                e->setIsMouseCaptured(true);

                return mMouseHolder->dispatchInputEvent(e);
            }
            else
                return mBaseLayout->dispatchInputEvent(e);
        }
        else if (e->isKeyboardEvent())
        {
            if (mFocusHolder)
                return mFocusHolder->dispatchInputEvent(e);
        }

        return delegate_->onInputEvent(e);
    }

    void WindowImpl::onDpiChanged(int dpi_x, int dpi_y) {
        mBaseLayout->dispatchWindowDpiChanged(dpi_x, dpi_y);
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
            if (isStartupWindow()) {
                if (onClose()) {
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
            onDestroy();
            if (isStartupWindow()) {
                ::PostQuitMessage(0);
            }
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
                ::SetCursor(cursor_);
                return TRUE;
            }
            break;
        }

        case WM_MOVE: {
            int x_px = LOWORD(lParam);
            int y_px = HIWORD(lParam);
            notifyLocationChanged(x_px, y_px);
            break;
        }

        case WM_SIZE: {
            RECT winRect;
            ::GetWindowRect(hWnd_, &winRect);

            int width_px = winRect.right - winRect.left;
            int height_px = winRect.bottom - winRect.top;
            int client_width_px = LOWORD(lParam);
            int client_height_px = HIWORD(lParam);

            notifySizeChanged(
                wParam,
                width_px, height_px,
                client_width_px, client_height_px);

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


    void WindowImpl::UpdateCycler::handleMessage(Message *msg)
    {
        switch (msg->what)
        {
        case SCHEDULE_RENDER:
            win_->performRefresh();
            break;
        case SCHEDULE_LAYOUT:
            win_->performLayout();
            break;
        }
    }


    void WindowImpl::AnimStateChangedListener::onStateChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus)
    {
        if (newStatus == UI_ANIMATION_MANAGER_BUSY
            && previousStatus == UI_ANIMATION_MANAGER_IDLE)
            win_->invalidate();
    }


    WindowImpl::AnimTimerEventListener::AnimTimerEventListener(WindowImpl *window)
    {
        this->mWindow = window;
    }

    void WindowImpl::AnimTimerEventListener::OnPreUpdate()
    {

    }

    void WindowImpl::AnimTimerEventListener::OnPostUpdate()
    {
        mWindow->invalidate();
    }

    void WindowImpl::AnimTimerEventListener::OnRenderingTooSlow(unsigned int fps)
    {
    }
}