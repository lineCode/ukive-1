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
#include "ukive/system/qpc_service.h"
#include "ukive/views/debug_view.h"
#include "ukive/window/window_listener.h"


namespace ukive {

    Window::Window()
        : impl_(std::make_unique<WindowImpl>(this)),
          canvas_(nullptr),
          renderer_(nullptr),
          labour_cycler_(nullptr),
          root_layout_(nullptr),
          mouse_holder_(nullptr),
          focus_holder_(nullptr),
          focus_holder_backup_(nullptr),
          last_input_view_(nullptr),
          mouse_holder_ref_(0),
          context_menu_(nullptr),
          text_action_mode_(nullptr),
          anim_mgr_(nullptr),
          mAnimStateChangedListener(nullptr),
          mAnimTimerEventListener(nullptr),
          background_color_(Color::White),
          is_startup_window_(false),
          min_width_(0),
          min_height_(0),
          frame_type_(FRAME_NATIVE)
    {
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

    void Window::minimize() {
        impl_->minimize();
    }

    void Window::maximize() {
        impl_->maximize();
    }

    void Window::restore() {
        impl_->restore();
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

    void Window::setContentView(int layout_id) {
        root_layout_->setContent(layout_id);
    }

    void Window::setContentView(View* content) {
        root_layout_->setContent(content);
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

    void Window::setFrameType(FrameType type) {
        frame_type_ = type;
    }

    void Window::setLastInputView(View* v) {
        last_input_view_ = v;
    }

    int Window::getX() const {
        return impl_->getX();
    }

    int Window::getY() const {
        return impl_->getY();
    }

    int Window::getWidth() const {
        return impl_->getWidth();
    }

    int Window::getHeight() const {
        return impl_->getHeight();
    }

    int Window::getMinWidth() const {
        return min_width_;
    }

    int Window::getMinHeight() const {
        return min_height_;
    }

    int Window::getClientWidth() const {
        return impl_->getClientWidth();
    }

    int Window::getClientHeight() const {
        return impl_->getClientHeight();
    }

    string16 Window::getTitle() const {
        return impl_->getTitle();
    }

    RootLayout* Window::getRootLayout() const {
        return root_layout_;
    }

    Color Window::getBackgroundColor() const {
        return background_color_;
    }

    Cycler* Window::getCycler() const {
        return labour_cycler_;
    }

    Renderer* Window::getRenderer() const {
        return renderer_;
    }

    HWND Window::getHandle() const {
        return impl_->getHandle();
    }

    AnimationManager* Window::getAnimationManager() const {
        return anim_mgr_;
    }

    Window::FrameType Window::getFrameType() const {
        return frame_type_;
    }

    View* Window::getLastInputView() const {
        return last_input_view_;
    }

    TitleBar* Window::getTitleBar() const {
        return root_layout_->getTitleBar();
    }

    bool Window::isShowing() const {
        return impl_->isShowing();
    }

    bool Window::isTranslucent() const {
        return impl_->isTranslucent();
    }

    bool Window::isStartupWindow() const {
        return is_startup_window_;
    }

    bool Window::isMinimum() const {
        return impl_->isMinimum();
    }

    bool Window::isMaximum() const {
        return impl_->isMaximum();
    }

    bool Window::isTitleBarShowing() const {
        return root_layout_->isTitleBarShowing();
    }

    void Window::showTitleBar() {
        root_layout_->showTitleBar();
    }

    void Window::hideTitleBar() {
        root_layout_->hideTitleBar();
    }

    void Window::removeTitleBar() {
        root_layout_->removeTitleBar();
    }

    void Window::addStatusChangedListener(OnWindowStatusChangedListener* l) {
        status_changed_listeners_.push_back(l);
    }

    void Window::removeStatusChangedListener(OnWindowStatusChangedListener* l) {
        for (auto it = status_changed_listeners_.begin();
            it != status_changed_listeners_.end(); ++it)
        {
            if (*it == l) {
                status_changed_listeners_.erase(it);
                break;
            }
        }
    }

    void Window::convScreenToClient(Point* p) {
        impl_->convScreenToClient(p);
    }

    void Window::convClientToScreen(Point* p) {
        impl_->convClientToScreen(p);
    }

    void Window::captureMouse(View* v) {
        if (!v) {
            return;
        }

        // 当已存在有捕获鼠标的 View 时，若此次调用该方法的 View
        // 与之前不同，此次调用将被忽略。在使用中应避免此种情况产生。
        if (mouse_holder_ref_ != 0 &&
            v != mouse_holder_)
        {
            LOG(Log::ERR) << "abnormal capture mouse!!";
            return;
        }

        ++mouse_holder_ref_;

        // 该 View 第一次捕获鼠标。
        if (mouse_holder_ref_ == 1) {
            impl_->setMouseCaptureRaw();
            mouse_holder_ = v;
        }
    }

    void Window::releaseMouse(bool all) {
        if (mouse_holder_ref_ == 0) {
            return;
        }

        if (all) {
            mouse_holder_ref_ = 0;
        } else {
            --mouse_holder_ref_;
        }

        // 鼠标将被释放。
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

    View* Window::getMouseHolder() const {
        return mouse_holder_;
    }

    int Window::getMouseHolderRef() const {
        return mouse_holder_ref_;
    }

    View* Window::getKeyboardHolder() const {
        return focus_holder_;
    }

    void Window::invalidate() {
        labour_cycler_->removeMessages(SCHEDULE_RENDER);
        labour_cycler_->sendEmptyMessage(SCHEDULE_RENDER);
    }

    void Window::invalidate(int left, int top, int right, int bottom) {}

    void Window::requestLayout() {
        labour_cycler_->removeMessages(SCHEDULE_LAYOUT);
        labour_cycler_->sendEmptyMessage(SCHEDULE_LAYOUT);
    }

    void Window::performLayout() {
        if (!impl_->isCreated()) {
            return;
        }

        auto params = root_layout_->getLayoutParams();

        int left = impl_->getClientOffX();
        int top = impl_->getClientOffY();
        int width;
        int height;
        int width_mode;
        int height_mode;
        if (impl_->isTranslucent()) {
            if (isMaximum()) {
                width = getClientWidth();
                height = getClientHeight();
            } else {
                width = getWidth();
                height = getHeight();
            }
        } else {
            width = getClientWidth();
            height = getClientHeight();
        }

        if (params->width < 0) {
            switch (params->width) {
            case LayoutParams::FIT_CONTENT:
                width_mode = View::FIT;
                break;

            case LayoutParams::MATCH_PARENT:
            default:
                width_mode = View::EXACTLY;
                break;
            }
        } else {
            width = params->width;
            width_mode = View::EXACTLY;
        }

        if (params->height < 0) {
            switch (params->height) {
            case LayoutParams::FIT_CONTENT:
                height_mode = View::FIT;
                break;

            case LayoutParams::MATCH_PARENT:
            default:
                height_mode = View::EXACTLY;
                break;
            }
        } else {
            height = params->height;
            height_mode = View::EXACTLY;
        }

        QPCService qpc_service;
        auto debug_view = root_layout_->getDebugView();
        bool enable_qpc = (debug_view && debug_view->getMode() == DebugView::Mode::LAYOUT);
        if (enable_qpc) {
            qpc_service.Start();
        }

        root_layout_->measure(width, height, width_mode, height_mode);

        int measured_width = root_layout_->getMeasuredWidth();
        int measured_height = root_layout_->getMeasuredHeight();

        root_layout_->layout(left, top, left + measured_width, top + measured_height);

        if (enable_qpc) {
            auto duration = qpc_service.Stop();
            debug_view->addDuration(duration);
        }
    }

    void Window::performRefresh() {
        if (!impl_->isCreated()) {
            return;
        }

        QPCService qpc_service;
        auto debug_view = root_layout_->getDebugView();
        bool enable_qpc = (debug_view && debug_view->getMode() == DebugView::Mode::RENDER);
        if (enable_qpc) {
            qpc_service.Start();
        }

        Rect rect(0, 0, getClientWidth(), getClientHeight());
        onDraw(rect);

        if (enable_qpc) {
            auto duration = qpc_service.Stop();
            debug_view->addDuration(duration);
        }
    }

    void Window::performRefresh(int left, int top, int right, int bottom) {
        if (!impl_->isCreated()) {
            return;
        }

        Rect rect(left, top, right - left, bottom - top);
        onDraw(rect);
    }


    View* Window::findViewById(int id) const {
        return root_layout_->findViewById(id);
    }

    ContextMenu* Window::startContextMenu(
        ContextMenuCallback* callback, View* anchor, View::Gravity gravity)
    {
        auto context_menu = new ContextMenu(this, callback);

        if (!callback->onCreateContextMenu(
            context_menu, context_menu->getMenu())) {
            delete context_menu;
            return nullptr;
        }

        callback->onPrepareContextMenu(
            context_menu, context_menu->getMenu());

        if (context_menu->getMenu()->getItemCount() == 0) {
            delete context_menu;
            return nullptr;
        }

        context_menu_ = context_menu;

        Rect rect = anchor->getBoundsInWindow();

        int x;
        int y = rect.bottom;

        switch (gravity) {
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
        return context_menu;
    }

    void Window::notifyContextMenuClose() {
        context_menu_ = nullptr;
    }

    TextActionMode* Window::startTextActionMode(TextActionModeCallback* callback) {
        auto action_mode = new TextActionMode(this, callback);
        if (!callback->onCreateActionMode(
            action_mode, action_mode->getMenu())) {
            delete action_mode;
            return nullptr;
        }

        callback->onPrepareActionMode(
            action_mode, action_mode->getMenu());

        if (action_mode->getMenu()->getItemCount() == 0) {
            delete action_mode;
            return nullptr;
        }

        text_action_mode_ = action_mode;
        text_action_mode_->show();

        return action_mode;
    }

    void Window::notifyTextActionModeClose() {
        text_action_mode_ = nullptr;
    }

    float Window::dpToPx(float dp) {
        return impl_->dpToPx(dp);
    }

    float Window::pxToDp(float px) {
        return impl_->pxToDp(px);
    }

    void Window::onPreCreate(ClassInfo* info, int* win_style, int* win_ex_style) {
    }

    void Window::onCreate() {
        labour_cycler_ = new Cycler();
        labour_cycler_->setListener(this);

        root_layout_ = new RootLayout(this);
        root_layout_->setId(0);
        root_layout_->setLayoutParams(
            new LayoutParams(
                LayoutParams::MATCH_PARENT,
                LayoutParams::MATCH_PARENT));

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
        switch (param) {
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
        default:
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

    void Window::onSetText(const string16& text) {
        for (auto listener : status_changed_listeners_) {
            listener->onWindowTextChanged(text);
        }
    }

    void Window::onSetIcon() {
        for (auto listener : status_changed_listeners_) {
            listener->onWindowIconChanged();
        }
    }

    void Window::onDraw(const Rect& rect) {
        if (impl_->isCreated())
        {
            getAnimationManager()->update();

            bool ret = renderer_->render(
                background_color_, [this]() {

                if (canvas_) {
                    onDrawCanvas(canvas_);

                    if (root_layout_->isLayouted() &&
                        root_layout_->getVisibility() == View::VISIBLE &&
                        root_layout_->getWidth() > 0 && root_layout_->getHeight() > 0)
                    {
                        canvas_->save();
                        canvas_->translate(root_layout_->getLeft(), root_layout_->getTop());
                        root_layout_->draw(canvas_);
                        canvas_->restore();
                    }
                }
            });

            if (!ret) {
                LOG(Log::FATAL) << "Failed to render.";
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
        int client_width, int client_height) {

        HRESULT hr = renderer_->resize();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Resize DirectX Renderer failed.";
            return;
        }

        switch (param) {
        case SIZE_RESTORED:
        case SIZE_MINIMIZED:
        case SIZE_MAXIMIZED:
            for (auto listener : status_changed_listeners_) {
                listener->onWindowStatusChanged();
            }
            break;
        case SIZE_MAXSHOW:
            break;
        case SIZE_MAXHIDE:
            break;
        default:
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
        if (height < minHeight) {
            switch (edge) {
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
            default:
                break;
            }
            processed = true;
        }

        if (width < minWidth) {
            switch (edge) {
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
            default:
                break;
            }
            processed = true;
        }

        return processed;
    }

    bool Window::onClose() {
        if (isStartupWindow()) {
            int count = WindowManager::getInstance()->getWindowCount();
            for (int i = 0; i < count; ++i) {
                auto window = WindowManager::getInstance()->getWindow(i);
                if (!window->isStartupWindow()) {
                    window->close();
                }
            }
        }
        return true;
    }

    void Window::onDestroy() {
        if (context_menu_) {
            context_menu_->close();
        }
        if (text_action_mode_) {
            text_action_mode_->close();
        }

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
            // 若有之前捕获过鼠标的 View 存在，则直接将所有鼠标事件
            // 直接发送至该 View。
            if (mouse_holder_ &&
                mouse_holder_->getVisibility() == View::VISIBLE &&
                mouse_holder_->isEnabled())
            {
                // 进行坐标变换，将目标 View 左上角映射为(0, 0)。
                int total_left = 0;
                int total_top = 0;
                auto parent = mouse_holder_->getParent();
                while (parent) {
                    total_left += (parent->getLeft() - parent->getScrollX());
                    total_top += (parent->getTop() - parent->getScrollY());

                    parent = parent->getParent();
                }

                e->setX(e->getX() - total_left);
                e->setY(e->getY() - total_top);
                e->setIsNoDispatch(true);

                return mouse_holder_->dispatchInputEvent(e);
            }

            if (e->getEvent() == InputEvent::EVM_LEAVE_WIN) {
                e->setEvent(InputEvent::EV_LEAVE_VIEW);
                if (last_input_view_) {
                    e->setIsNoDispatch(true);
                    last_input_view_->dispatchInputEvent(e);
                    return false;
                }
            }

            return root_layout_->dispatchInputEvent(e);
        }

        if (e->isTouchEvent()) {
            return root_layout_->dispatchInputEvent(e);
        }

        if (e->isKeyboardEvent()) {
            if (e->getEvent() == InputEvent::EVK_DOWN && e->getKeyboardVirtualKey() == 0x51) {
                bool isShiftKeyPressed = (::GetKeyState(VK_SHIFT) < 0);
                bool isCtrlKeyPressed = (::GetKeyState(VK_CONTROL) < 0);
                if (isCtrlKeyPressed && isShiftKeyPressed) {
                    root_layout_->toggleDebugView();
                } else if (isCtrlKeyPressed && !isShiftKeyPressed) {
                    auto debug_view = root_layout_->getDebugView();
                    if (debug_view) {
                        debug_view->toggleMode();
                    }
                }
            }

            if (focus_holder_) {
                return focus_holder_->dispatchInputEvent(e);
            }
        }

        return false;
    }

    HitPoint Window::onNCHitTest(int x, int y) {
        x -= root_layout_->getLeft();
        y -= root_layout_->getTop();
        return root_layout_->onNCHitTest(x, y);
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

    void Window::onHandleMessage(Message* msg) {
        switch (msg->what) {
        case SCHEDULE_RENDER:
            performRefresh();
            break;
        case SCHEDULE_LAYOUT:
            performLayout();
            break;
        default:
            break;
        }
    }

    void Window::AnimStateChangedListener::onStateChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus)
    {
        if (newStatus == UI_ANIMATION_MANAGER_BUSY &&
            previousStatus == UI_ANIMATION_MANAGER_IDLE)
        {
            win_->invalidate();
        }
    }


    void Window::AnimTimerEventListener::onPreUpdate() {
    }

    void Window::AnimTimerEventListener::onPostUpdate() {
        window_->invalidate();
    }

    void Window::AnimTimerEventListener::onRenderingTooSlow(unsigned int fps) {
    }

}