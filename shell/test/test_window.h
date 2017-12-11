#ifndef SHELL_TEST_TEST_WINDOW_H_
#define SHELL_TEST_TEST_WINDOW_H_

#include "ukive/window/window.h"


namespace shell {

    class TestWindow : public ukive::Window {
    public:
        TestWindow();
        ~TestWindow();

        void onPreCreate(
            ukive::ClassInfo *info,
            int *win_style, int *win_ex_style) override;
        void onCreate() override;

    private:
        void inflateGroup();
        void inflateListView();
    };

}

#endif  // SHELL_TEST_TEST_WINDOW_H_