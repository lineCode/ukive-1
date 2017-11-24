#ifndef UKIVE_WINDOW_WINDOW_SLAVE_H_
#define UKIVE_WINDOW_WINDOW_SLAVE_H_

#include <Windows.h>


namespace ukive {

    class Window;

    class UWindowSlave
    {
    private:
        HWND mHandle;
        Window *mMaster;

        void initSlave();

    public:
        UWindowSlave(Window *master);
        ~UWindowSlave();

        void sync();

        HWND getWindowHandle();

        LRESULT CALLBACK messageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP);
        LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam, int leftExt, int topExt, int rightExt, int bottomExt);
    };

    static UWindowSlave *sUWSVtr;
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

}

#endif  // UKIVE_WINDOW_WINDOW_SLAVE_H_