#include "window.h"

#include "ukive/log.h"
#include "ukive/application.h"
#include "ukive/window/window_impl.h"
#include "ukive/window/window_manager.h"
#include "ukive/views/layout/root_layout.h"
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
        root_layout_(nullptr),
        labour_cycler_(nullptr),
        mouse_holder_(nullptr),
        focus_holder_(nullptr),
        focus_holder_backup_(nullptr),
        mouse_holder_ref_(0),
        canvas_(nullptr),
        renderer_(nullptr),
        anim_mgr_(nullptr),
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

    void Window::setTitle(const string16& title) {
        impl_->setTitle(title);
    }

    void Window::setX(int x) {
        impl_->setBounds(x, impl_->getY(), impl_->getWidth(), impl_->getHeight());
    }

    void Window::setY(int y) {
        impl_->setBounds(impl_->getX(), y, impl_->getWidth(), impl_->getHeight());
    }

    void Window::setPosition(int x, int y) {
        impl_->setBounds(x, y, impl_->getWidth(), impl_->getHeight());
    }

    void Window::setWidth(int width) {
        impl_->setBounds(impl_->getX(), impl_->getY(), width, impl_->getHeight());
    }

    void Window::setHeight(int height) {
        impl_->setBounds(impl_->getX(), impl_->getY(), impl_->getWidth(), height);
    }

    void Window::setBounds(int x, int y, int width, int height) {
        impl_->setBounds(x, y, width, height);
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

    void Window::setContentView(View* content) {
        root_layout_->addContent(content);
    }

    void Window::setBackgroundColor(Color color) {
        background_color_ = color;
        invalidate();
    }

    void Window::setTranslucent(bool translucent) {
        impl_->setTranslucent(translucent);
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

    RootLayout* Window::getRootLayout() {
        return root_layout_;
    }

    Color Window::getBackgroundColor() {
        return background_color_;
    }

    Cycler* Window::getCycler() {
        return labour_cycler_;
    }

    Renderer* Window::getRenderer() {
        return renderer_;
    }

    HWND Window::getHandle() {
        return impl_->getHandle();
    }

    AnimationManager* Window::getAnimationManager() {
        return anim_mgr_;
    }

    bool Window::isShowing() {
        return impl_->isShowing();
    }

    bool Window::isCursorInClient() {
        return impl_->isCursorInClient();
    }

    bool Window::isTranslucent() {
        return impl_->isTranslucent();
    }

    bool Window::isStartupWindow() {
        return is_startup_window_;
    }

    void Window::captureMouse(View* v) {
        if (v == nullptr) {
            return;
        }

        //当已存在有捕获鼠标的 view 时，若此次调用该方法的 view
        //与之前不同，此次调用将被忽略。在使用中应避免此种情况产生。
        if (mouse_holder_ref_ != 0
            && v != mouse_holder_) {
            DCHECK(false) << "abnormal capture mouse!!";
            return;
        }

        ++mouse_holder_ref_;

        //该Widget第一次捕获鼠标。
        if (mouse_holder_ref_ == 1) {
            impl_->setMouseCaptureRaw();
            mouse_holder_ = v;
        }
    }

    void Window::releaseMouse() {
        if (mouse_holder_ref_ == 0) {
            return;
        }

        --mouse_holder_ref_;

        //鼠标将被释放。
        if (mouse_holder_ref_ == 0) {
            impl_->releaseMouseCaptureRaw();
            mouse_holder_ = nullptr;
        }
    }

    void Window::captureKeyboard(View* v) {
        focus_holder_ = v;
    }

    void Window::releaseKeyboard() {
        focus_holder_ = nullptr;
    }

    View* Window::getMouseHolder() {
        return mouse_holder_;
    }

    unsigned int Window::getMouseHolderRef() {
        return mouse_holder_ref_;
    }

    View* Window::getKeyboardHolder() {
        return focus_holder_;
    }

    void Window::invalidate() {
        labour_cycler_->removeMessages(SCHEDULE_RENDER);
        labour_cycler_->sendEmptyMessage(SCHEDULE_RENDER);
    }

    void Window::invalidate(int left, int top, int right, int bottom) {
    }

    void Window::requestLayout() {
        labour_cycler_->removeMessages(SCHEDULE_LAYOUT);
        labour_cycler_->sendEmptyMessage(SCHEDULE_LAYOUT);
    }

    void Window::performLayout() {
        if (!impl_->isCreated()) {
            return;
        }

        LayoutParams* params = root_layout_->getLayoutParams();

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
        } else {
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
        } else {
            height = params->height;
            heightSpec = View::EXACTLY;
        }

        root_layout_->measure(width, height, widthSpec, heightSpec);

        int measuredWidth = root_layout_->getMeasuredWidth();
        int measuredHeight = root_layout_->getMeasuredHeight();

        root_layout_->layout(0, 0, measuredWidth, measuredHeight);
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


    View* Window::findViewById(int id) {
        return root_layout_->findViewById(id);
    }

    ContextMenu* Window::startContextMenu(
        ContextMenuCallback* callback, View* anchor, View::Gravity gravity) {

        ContextMenu* contextMenu
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

        context_menu_.reset(contextMenu);

        int x, y;
        Rect rect = anchor->getBoundsInWindow();

        y = rect.bottom;

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

        context_menu_->show(x, y);
        return contextMenu;
    }

    TextActionMode* Window::startTextActionMode(TextActionModeCallback* callback) {
        TextActionMode* actionMode
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

        text_action_mode_.reset(actionMode);
        text_action_mode_->show();

        return actionMode;
    }

    float Window::dpToPx(float dp) {
        return impl_->dpToPx(dp);
    }

    float Window::pxToDp(int px) {
        return impl_->pxToDp(px);
    }

    void Window::onPreCreate(ClassInfo* info, int* win_style, int* win_ex_style) {
    }

    void Window::onCreate() {
        labour_cycler_ = new UpdateCycler(this);

        root_layout_ = new RootLayout(this);
        root_layout_->setId(0);
        root_layout_->setLayoutParams(
            new LayoutParams(
                LayoutParams::MATCH_PARENT,
                LayoutParams::MATCH_PARENT));

        /*FrameLayout* titleBar = new FrameLayout(this);
        titleBar->setBackground(new ColorDrawable(this, Color::Blue100));
        RootLayoutParams* titleBarLp = new RootLayoutParams(
        RootLayoutParams::MATCH_PARENT, 50);
        root_layout_->addContent(titleBar, titleBarLp);*/

        anim_mgr_ = new AnimationManager();
        HRESULT hr = anim_mgr_->init();
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to init animation manager: " << hr;
            return;
        }

        mAnimStateChangedListener = new AnimStateChangedListener(this);
        anim_mgr_->setOnStateChangedListener(mAnimStateChangedListener);

        //anim_mgr_->connectTimer(true);
        mAnimTimerEventListener = new AnimTimerEventListener(this);
        //anim_mgr_->setTimerEventListener(mAnimTimerEventListener);

        renderer_ = new Renderer();
        hr = renderer_->init(this);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to init renderer: " << hr;
            return;
        }

        renderer_->addSwapChainResizeNotifier(this);

        canvas_ = new Canvas(renderer_->getRenderTarget());

        root_layout_->onAttachedToWindow();
    }

    void Window::onShow(bool show) {
    }

    void Window::onActivate(int param) {
        switch (param)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            /*if (focus_holder_backup_)
            focus_holder_backup_->requestFocus();*/
            break;
        case WA_INACTIVE:
            /*if (focus_holder_)
            {
            InputEvent ev;
            ev.setEvent(InputEvent::EVENT_CANCEL);

            focus_holder_backup_ = focus_holder_;
            focus_holder_backup_->dispatchInputEvent(&ev);
            focus_holder_->discardFocus();
            }*/
            break;
        }
    }

    void Window::onSetFocus() {
        root_layout_->dispatchWindowFocusChanged(true);
    }

    void Window::onKillFocus() {
        while (mouse_holder_ref_ > 0) {
            releaseMouse();
        }
        root_layout_->dispatchWindowFocusChanged(false);
    }

    void Window::onDraw(const Rect& rect) {
        if (impl_->isCreated())
        {
            getAnimationManager()->update();

            bool ret = renderer_->render(
                background_color_, [this]() {

                if (canvas_) {
                    onDrawCanvas(canvas_);

                    if (root_layout_->isLayouted()) {
                        root_layout_->draw(canvas_);
                    }
                }
            });

            if (!ret) {
                Log::e(L"Window", L"failed to render.");
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

        HRESULT hr = renderer_->resize();
        if (FAILED(hr)) {
            Log::e(L"Window", L"Resize DirectX Renderer failed.");
            return;
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

    bool Window::onMoving(Rect* rect) {
        return false;
    }

    bool Window::onResizing(int edge, Rect* rect) {
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
        delete root_layout_;

        delete canvas_;
        canvas_ = nullptr;

        renderer_->removeSwapChainResizeNotifier(this);
        renderer_->close();
        delete renderer_;
        renderer_ = nullptr;

        delete mAnimTimerEventListener;
        delete mAnimStateChangedListener;

        anim_mgr_->setOnStateChangedListener(nullptr);
        anim_mgr_->close();
        delete anim_mgr_;
        delete labour_cycler_;

        WindowManager::getInstance()->removeWindow(this);

        if (isStartupWindow()) {
            Application::quitSystemQueue();
        }
    }

    bool Window::onInputEvent(InputEvent* e) {
        if (e->isMouseEvent()) {
            //若有之前捕获过鼠标的Widget存在，则直接将所有事件
            //直接发送至该Widget。
            if (mouse_holder_
                && mouse_holder_->getVisibility() == View::VISIBLE
                && mouse_holder_->isEnabled()) {

                //进行坐标变换，将目标Widget左上角映射为(0, 0)。
                int totalLeft = 0;
                int totalTop = 0;
                View* parent = mouse_holder_->getParent();
                while (parent) {
                    totalLeft += (parent->getLeft() - parent->getScrollX());
                    totalTop += (parent->getTop() - parent->getScrollY());

                    parent = parent->getParent();
                }

                e->setMouseX(e->getMouseX() - totalLeft);
                e->setMouseY(e->getMouseY() - totalTop);
                e->setIsMouseCaptured(true);

                return mouse_holder_->dispatchInputEvent(e);
            }
            else {
                return root_layout_->dispatchInputEvent(e);
            }
        }
        else if (e->isKeyboardEvent()) {
            if (focus_holder_)
                return focus_holder_->dispatchInputEvent(e);
        }

        return false;
    }

    void Window::onDpiChanged(int dpi_x, int dpi_y) {
        root_layout_->dispatchWindowDpiChanged(dpi_x, dpi_y);
    }

    bool Window::onDataCopy(unsigned int id, unsigned int size, void* data) {
        return false;
    }


    void Window::onDrawCanvas(Canvas* canvas) {
    }


    void Window::onPreSwapChainResize() {
        delete canvas_;
    }

    void Window::onPostSwapChainResize() {
        canvas_ = new Canvas(renderer_->getRenderTarget());
    }


    void Window::UpdateCycler::handleMessage(Message* msg) {
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


    void Window::AnimTimerEventListener::onPreUpdate() {
    }

    void Window::AnimTimerEventListener::onPostUpdate() {
        window_->invalidate();
    }

    void Window::AnimTimerEventListener::onRenderingTooSlow(unsigned int fps) {
    }

}