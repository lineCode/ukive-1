#ifndef SHELL_TEST_TEST_WINDOW_H_
#define SHELL_TEST_TEST_WINDOW_H_

#include "ukive/window/window.h"


namespace shell {

    class TestWindow : public ukive::Window {
    public:
        TestWindow();
        ~TestWindow();

        void onCreate() override;
    };

}

#endif  // SHELL_TEST_TEST_WINDOW_H_