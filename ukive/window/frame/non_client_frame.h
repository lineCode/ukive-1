#ifndef UKIVE_WINDOW_FRAME_NON_CLIENT_FRAME_H_
#define UKIVE_WINDOW_FRAME_NON_CLIENT_FRAME_H_

#include <Windows.h>


namespace ukive {

    class NonClientFrame {
    public:
        virtual ~NonClientFrame() {}

        virtual LRESULT onSize(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onMouseMove(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam) = 0;

        virtual LRESULT onNcPaint(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onNcActivate(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onNcHitTest(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onNcCalSize(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onNcLButtonUp(WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT onInterceptDrawClassic(WPARAM wParam, LPARAM lParam) = 0;
    };

}

#endif  // UKIVE_WINDOW_FRAME_NON_CLIENT_FRAME_H_
