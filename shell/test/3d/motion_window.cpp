#include "motion_window.h"


namespace shell {

    void MotionWindow::onPreCreate(
        ukive::ClassInfo* info,
        int* win_style, int* win_ex_style) {

        //*win_style = WS_POPUP;
    }

    void MotionWindow::onCreate() {
        Window::onCreate();

        //setBackgroundColor(ukive::Color::Black);
    }

}