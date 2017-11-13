#include "window.h"

#include "window/window_impl.h"
#include "window/window_manager.h"


namespace ukive {

    Window::Window()
        :impl_(new WindowImpl(this))
    {

    }

    Window::~Window() {

    }

    void Window::show() {
        impl_->show();
        WindowManager::getInstance()->addWindow(this);
    }

    void Window::hide() {

    }

    void Window::focus() {

    }

    void Window::close() {

    }

    void Window::close(bool notify) {

    }

    void Window::onPreCreate(ClassInfo &info) {

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

    }

    bool Window::onResizing(WPARAM edge, RECT *rect) {

    }

    bool Window::onClose() {

    }

    void Window::onDestroy() {

    }

    void Window::onInputEvent(InputEvent *e) {

    }

}