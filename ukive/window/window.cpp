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

    }

    void Window::focus() {

    }

    void Window::close() {

    }

    void Window::close(bool notify) {

    }

    void Window::setMinWidth(int minWidth) {
        min_width_ = minWidth;
    }

    void Window::setMinHeight(int minHeight) {
        min_height_ = minHeight;
    }

    int Window::getMinWidth() {
        return min_width_;
    }

    int Window::getMinHeight() {
        return min_height_;
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

    void Window::onInputEvent(InputEvent *e) {

    }

}