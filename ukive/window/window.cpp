#include "window.h"

#include "ukive/window/window_impl.h"
#include "ukive/window/window_manager.h"


namespace ukive {

    Window::Window()
        :impl_(new WindowImpl(this)),
        min_width_(0),
        min_height_(0)
    {

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

    void Window::close(bool notify) {
        impl_->close(notify);
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

    void Window::setCurrentCursor(LPCWSTR cursor) {
        impl_->setCurrentCursor(cursor);
    }

    void Window::setContentView(View *content) {
        impl_->setContentView(content);
    }

    void Window::setBackgroundColor(Color color) {
        impl_->setBackgroundColor(color);
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
        return impl_->getBaseLayout();
    }

    Color Window::getBackgroundColor() {
        return impl_->getBackgroundColor();
    }

    Cycler *Window::getCycler() {
        return impl_->getCycler();
    }

    Renderer *Window::getRenderer() {
        return impl_->getRenderer();
    }

    HWND Window::getHandle() {
        return impl_->getHandle();
    }

    View *Window::getKeyboardHolder() {
        return impl_->getKeyboardHolder();
    }

    BitmapFactory *Window::getBitmapFactory() {
        return impl_->getBitmapFactory();
    }

    AnimationManager *Window::getAnimationManager() {
        return impl_->getAnimationManager();
    }

    bool Window::isShowing() {
        return impl_->isShowing();
    }

    bool Window::isCursorInClient() {
        return impl_->isCursorInClient();
    }

    void Window::captureMouse(View *widget) {
        impl_->captureMouse(widget);
    }

    void Window::releaseMouse() {
        impl_->releaseMouse();
    }

    void Window::captureKeyboard(View *widget) {
        impl_->captureKeyboard(widget);
    }

    void Window::releaseKeyboard() {
        impl_->releaseKeyboard();
    }

    void Window::invalidate() {
        impl_->invalidate();
    }

    void Window::requestLayout() {
        impl_->requestLayout();
    }

    View *Window::findViewById(int id) {
        return impl_->findWidgetById(id);
    }

    ContextMenu *Window::startContextMenu(
        ContextMenuCallback *callback, View *anchor, Gravity gravity) {
        return impl_->startContextMenu(callback, anchor, gravity);
    }

    TextActionMode *Window::startTextActionMode(TextActionModeCallback *callback) {
        return impl_->startTextActionMode(callback);
    }

    void Window::onPreCreate(ClassInfo *info) {
    }

    void Window::onCreate() {
    }

    void Window::onShow(bool show) {
    }

    void Window::onActivate(int param) {
    }

    void Window::onDraw(Canvas *canvas) {
    }

    void Window::onMove(int x, int y) {
    }

    void Window::onResize(
        int param, int width, int height,
        int clientWidth, int clientHeight) {
        switch (param)
        {
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
    }

    bool Window::onMoving(RECT *rect) {
        return false;
    }

    bool Window::onResizing(WPARAM edge, RECT *rect) {
        int minWidth = min_width_;
        int minHeight = min_height_;
        bool processed = false;

        LONG width = rect->right - rect->left;
        LONG height = rect->bottom - rect->top;
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
        return true;
    }

    void Window::onDestroy() {
    }

    bool Window::onInputEvent(InputEvent *e) {
        return false;
    }

}