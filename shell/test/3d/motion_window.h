#ifndef SHELL_TEST_3D_MOTION_WINDOW_H_
#define SHELL_TEST_3D_MOTION_WINDOW_H_

#include "ukive/window/window.h"


namespace ukive {
    struct ClassInfo;
}

namespace shell {

    class MotionWindow : public ukive::Window {
    public:
        MotionWindow() = default;
        ~MotionWindow() = default;

        void onPreCreate(
            ukive::ClassInfo* info,
            int* win_style, int* win_ex_style) override;
        void onCreate() override;
    };

}

#endif  // SHELL_TEST_3D_MOTION_WINDOW_H_