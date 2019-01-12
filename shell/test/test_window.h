#ifndef SHELL_TEST_TEST_WINDOW_H_
#define SHELL_TEST_TEST_WINDOW_H_

#include "ukive/views/click_listener.h"
#include "ukive/window/window.h"


namespace ukive {
    class Button;
}

namespace shell {

    class TestWindow : public ukive::Window, public ukive::OnClickListener {
    public:
        TestWindow();
        ~TestWindow();

        void onPreCreate(
            ukive::ClassInfo* info,
            int* win_style, int* win_ex_style) override;
        void onCreate() override;

        void onClick(ukive::View* v) override;

    private:
        void inflateGroup();
        void inflateListView();

        ukive::Button* dwm_button_;
    };

}

#endif  // SHELL_TEST_TEST_WINDOW_H_