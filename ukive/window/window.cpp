#include "window.h"

#include "ukive/log.h"
#include "ukive/application.h"
#include "ukive/window/window_impl.h"
#include "ukive/window/window_manager.h"
#include "ukive/views/layout/base_layout.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/menu/context_menu.h"
#include "ukive/menu/context_menu_callback.h"
#include "ukive/text/text_action_mode_callback.h"
#include "ukive/text/text_action_mode.h"
#include "ukive/menu/menu.h"
#include "ukive/message/message.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/renderer.h"


namespace ukive {

    Window::Window()
        :impl_(std::make_unique<WindowImpl>(this)),
        min_width_(0),
        min_height_(0),
        is_startup_window_(false),
        mBaseLayout(nullptr),
        mLabourCycler(nullptr),
        mMouseHolder(nullptr),
        mFocusHolder(nullptr),
        mFocusHolderBackup(nullptr),
        mMouseHolderRef(0),
        mTextActionMode(nullptr),
        mContextMenu(nullptr),
        mCanvas(nullptr),
        mRenderer(nullptr),
        mAnimationManager(nullptr),
        mAnimStateChangedListener(nullptr),
        mAnimTimerEventListener(nullptr),
        background_color_(Color::White) {

        WindowManager::getInstance()->addWindow(this);
    }

    Window::~Window() {
    }

    void Window::show() {
        impl_->show();
    }

    void Window::hide() {
        impl_->hide();
    }

    void Window::focus() {
        impl_->focus();
    }

    void Window::close() {
        impl_->close();
    }

    void Window::center() {
        impl_->center();
    }

    void Window::setTitle(const string16 &title) {
        impl_->setTitle(title);
    }

    void Window::setX(int x) {
        impl_->setBound(x, impl_->getY(), impl_->getWidth(), impl_->getHeight());
    }

    void Window::setY(int y) {
        impl_->setBound(impl_->getX(), y, impl_->getWidth(), impl_->getHeight());
    }

    void Window::setPosition(int x, int y) {
        impl_->setBound(x, y, impl_->getWidth(), impl_->getHeight());
    }

    void Window::setWidth(int width) {
        impl_->setBound(impl_->getX(), impl_->getY(), width, impl_->getHeight());
    }

    void Window::setHeight(int height) {
        impl_->setBound(impl_->getX(), impl_->getY(), impl_->getWidth(), height);
    }

    void Window::setBound(int x, int y, int width, int height) {
        impl_->setBound(x, y, width, height);
    }

    void Window::setMinWidth(int minWidth) {
        min_width_ = minWidth;
    }

    void Window::setMinHeight(int minHeight) {
        min_height_ = minHeight;
    }

    void Window::setCurrentCursor(Cursor cursor) {
        impl_->setCurrentCursor(cursor);
    }

    void Window::setContentView(View *content) {
        mBaseLayout->addContent(content);
    }

    void Window::setBackgroundColor(Color color) {
        background_color_ = color;
        invalidate();
    }

    void Window::setStartupWindow(bool enable) {
        is_startup_window_ = enable;
    }

    int Window::getX() {
        return impl_->getX();
    }

    int Window::getY() {
        return impl_->getY();
    }

    int Window::getWidth() {
        return impl_->getWidth();
    }

    int Window::getHeight() {
        return impl_->getHeight();
    }

    int Window::getMinWidth() {
        return min_width_;
    }

    int Window::getMinHeight() {
        return min_height_;
    }

    int Window::getClientWidth() {
        return impl_->getClientWidth();
    }

    int Window::getClientHeight() {
        return impl_->getClientHeight();
    }

    BaseLayout *Window::getBaseLayout() {
        return mBaseLayout;
    }

    Color Window::getBackgroundColor() {
        return background_color_;
    }

    Cycler *Window::getCycler() {
        return mLabourCycler;
    }

    Renderer *Window::getRenderer() {
        return mRenderer;
    }

    HWND Window::getHandle() {
        return impl_->getHandle();
    }

    AnimationManager *Window::getAnimationManager() {
        return mAnimationManager;
    }

    bool Window::isShowing() {
        return impl_->isShowing();
    }

    bool Window::isCursorInClient() {
        return impl_->isCursorInClient();
    }

    bool Window::isStartupWindow() {
        return is_startup_window_;
    }

    void Window::captureMouse(View *v) {
        if (v == nullptr) {
            return;
        }

        //当已存在有捕获鼠标的 view 时，若此次调用该方法的 view
        //与之前不同，此次调用将被忽略。在使用中应避免此种情况产生。
        if (mMouseHolderRef != 0
            && v != mMouseHolder) {
            Log::e(L"abnormal capture mouse!!\n");
            return;
        }

        ++mMouseHolderRef;
        ::OutputDebugString(L"capture mouse!!\n");

        //该Widget第一次捕获鼠标。
        if (mMouseHolderRef == 1) {
            impl_->setMouseCaptureRaw();
            mMouseHolder = v;
        }
    }

    void Window::releaseMouse() {
        if (mMouseHolderRef == 0) {
            return;
        }

        --mMouseHolderRef;
        ::OutputDebugString(L"release mouse!!\n");

        //鼠标将被释放。
        if (mMouseHolderRef == 0) {
            impl_->releaseMouseCaptureRaw();
            mMouseHolder = nullptr;
        }
    }

    void Window::captureKeyboard(View *v) {
        mFocusHolder = v;
        //::OutputDebugString(L"captureKeyboard!!\n");
    }

    void Window::releaseKeyboard() {
        mFocusHolder = nullptr;
        //::OutputDebugString(L"releaseKeyboard!!\n");
    }

    View *Window::getMouseHolder() {
        return mMouseHolder;
    }

    unsigned int Window::getMouseHolderRef() {
        return mMouseHolderRef;
    }

    View *Window::getKeyboardHolder() {
        return mFocusHolder;
    }

    void Window::invalidate() {
        mLabourCycler->removeMessages(SCHEDULE_RENDER);
        mLabourCycler->sendEmptyMessage(SCHEDULE_RENDER);
    }

    void Window::invalidate(int left, int top, int right, int bottom) {
    }

    void Window::requestLayout() {
        mLabourCycler->removeMessages(SCHEDULE_LAYOUT);
        mLabourCycler->sendEmptyMessage(SCHEDULE_LAYOUT);
    }

    void Window::performLayout() {
        if (!impl_->isCreated()) {
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

    void Window::performRefresh() {
        if (!impl_->isCreated()) {
            return;
        }

        Rect rect(0, 0, getWidth(), getHeight());
        onDraw(rect);
    }

    void Window::performRefresh(int left, int top, int right, int bottom) {
        if (!impl_->isCreated()) {
            return;
        }

        Rect rect(left, top, right - left, bottom - top);
        onDraw(rect);
    }


    View *Window::findViewById(int id) {
        return mBaseLayout->findViewById(id);
    }

    ContextMenu *Window::startContextMenu(
        ContextMenuCallback *callback, View *anchor, View::Gravity gravity) {

        ContextMenu *contextMenu
            = new ContextMenu(this, callback);

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
            ContextMenuWorker(Window *w, int x, int y)
                :x_(x), y_(y), window_(w) {}

            void run() override {
                window_->mContextMenu->show(x_, y_);
            }
        private:
            int x_, y_;
            Window *window_;
        }*worker = new ContextMenuWorker(this, x, y);

        mLabourCycler->post(worker);

        return contextMenu;
    }

    TextActionMode *Window::startTextActionMode(TextActionModeCallback *callback) {
        TextActionMode *actionMode
            = new TextActionMode(this, callback);

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
            TextActionModeWorker(Window *w)
                :window_(w) {}

            void run() override {
                window_->mTextActionMode->show();
            }
        private:
            Window *window_;
        }*worker = new TextActionModeWorker(this);

        mLabourCycler->post(worker);

        return actionMode;
    }

    float Window::dpToPx(float dp) {
        return impl_->dpToPx(dp);
    }

    float Window::pxToDp(int px) {
        return impl_->pxToDp(px);
    }

    void Window::onPreCreate(ClassInfo *info, int *win_style, int *win_ex_style) {
    }

    void Window::onCreate() {
        mLabourCycler = new UpdateCycler(this);

        mBaseLayout = new BaseLayout(this, 0);
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
    }

    void Window::onShow(bool show) {
    }

    void Window::onActivate(int param) {
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
            InputEvent ev;
            ev.setEvent(InputEvent::EVENT_CANCEL);

            mFocusHolderBackup = mFocusHolder;
            mFocusHolderBackup->dispatchInputEvent(&ev);
            mFocusHolder->discardFocus();
            }*/
            break;
        }
    }

    void Window::onDraw(const Rect &rect) {
        if (impl_->isCreated())
        {
            getAnimationManager()->update();

            bool ret = mRenderer->render(
                background_color_, [this]() {

                if (mCanvas) {
                    onDrawCanvas(mCanvas);

                    if (mBaseLayout->isLayouted()) {
                        mBaseLayout->draw(mCanvas);
                    }
                }
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

    void Window::onMove(int x, int y) {
    }

    void Window::onResize(
        int param, int width, int height,
        int clientWidth, int clientHeight) {

        HRESULT hr = mRenderer->resize();
        if (FAILED(hr)) {
            throw std::runtime_error("UWindow-onResize(): Resize DirectX Renderer failed.");
        }

        switch (param) {
        case SIZE_RESTORED:
            break;
        case SIZE_MINIMIZED:
            break;
        case SIZE_MAXIMIZED:
            break;
        case SIZE_MAXSHOW:
            break;
        case SIZE_MAXHIDE:
            break;
        }

        performLayout();
        performRefresh();
    }

    bool Window::onMoving(Rect *rect) {
        return false;
    }

    bool Window::onResizing(int edge, Rect *rect) {
        int minWidth = min_width_;
        int minHeight = min_height_;
        bool processed = false;

        int width = rect->right - rect->left;
        int height = rect->bottom - rect->top;
        if (height < minHeight)
        {
            switch (edge)
            {
            case WMSZ_TOP:
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
                rect->top = rect->bottom - minHeight;
                break;
            case WMSZ_BOTTOM:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_BOTTOMRIGHT:
                rect->bottom = rect->top + minHeight;
                break;
            }
            processed = true;
        }

        if (width < minWidth)
        {
            switch (edge)
            {
            case WMSZ_LEFT:
            case WMSZ_TOPLEFT:
            case WMSZ_BOTTOMLEFT:
                rect->left = rect->right - minWidth;
                break;
            case WMSZ_RIGHT:
            case WMSZ_TOPRIGHT:
            case WMSZ_BOTTOMRIGHT:
                rect->right = rect->left + minWidth;
                break;
            }
            processed = true;
        }

        return processed;
    }

    bool Window::onClose() {
        if (isStartupWindow()) {
            size_t count = WindowManager::getInstance()->getWindowCount();
            for (size_t i = 0; i < count; ++i) {
                auto window = WindowManager::getInstance()->getWindow(i);
                if (!window->isStartupWindow()) {
                    window->close();
                }
            }
        }
        return true;
    }

    void Window::onDestroy() {
        delete mBaseLayout;

        delete mCanvas;
        mCanvas = nullptr;

        mRenderer->close();
        delete mRenderer;
        mRenderer = nullptr;

        delete mAnimTimerEventListener;
        delete mAnimStateChangedListener;

        mAnimationManager->setOnStateChangedListener(nullptr);
        mAnimationManager->close();
        delete mAnimationManager;
        delete mLabourCycler;

        WindowManager::getInstance()->removeWindow(this);

        if (isStartupWindow()) {
            Application::quitSystemQueue();
        }
    }

    bool Window::onInputEvent(InputEvent *e) {
        if (e->isMouseEvent()) {
            //若有之前捕获过鼠标的Widget存在，则直接将所有事件
            //直接发送至该Widget。
            if (mMouseHolder
                && mMouseHolder->getVisibility() == View::VISIBLE
                && mMouseHolder->isEnabled()) {

                //进行坐标变换，将目标Widget左上角映射为(0, 0)。
                int totalLeft = 0;
                int totalTop = 0;
                View *parent = mMouseHolder->getParent();
                while (parent) {
                    totalLeft += (parent->getLeft() - parent->getScrollX());
                    totalTop += (parent->getTop() - parent->getScrollY());

                    parent = parent->getParent();
                }

                e->setMouseX(e->getMouseX() - totalLeft);
                e->setMouseY(e->getMouseY() - totalTop);
                e->setIsMouseCaptured(true);

                return mMouseHolder->dispatchInputEvent(e);
            }
            else {
                return mBaseLayout->dispatchInputEvent(e);
            }
        }
        else if (e->isKeyboardEvent()) {
            if (mFocusHolder)
                return mFocusHolder->dispatchInputEvent(e);
        }

        return false;
    }

    void Window::onDpiChanged(int dpi_x, int dpi_y) {
        mBaseLayout->dispatchWindowDpiChanged(dpi_x, dpi_y);
    }

    bool Window::onDataCopy(unsigned int id, unsigned int size, void *data) {
        return false;
    }


    void Window::onDrawCanvas(Canvas *canvas) {
    }


    void Window::UpdateCycler::handleMessage(Message *msg) {
        switch (msg->what) {
        case SCHEDULE_RENDER:
            win_->performRefresh();
            break;
        case SCHEDULE_LAYOUT:
            win_->performLayout();
            break;
        }
    }

    void Window::AnimStateChangedListener::onStateChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus) {
        if (newStatus == UI_ANIMATION_MANAGER_BUSY
            && previousStatus == UI_ANIMATION_MANAGER_IDLE)
            win_->invalidate();
    }


    void Window::AnimTimerEventListener::OnPreUpdate() {
    }

    void Window::AnimTimerEventListener::OnPostUpdate() {
        window_->invalidate();
    }

    void Window::AnimTimerEventListener::OnRenderingTooSlow(unsigned int fps) {
    }

}