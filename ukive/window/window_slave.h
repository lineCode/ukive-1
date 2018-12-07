#ifndef UKIVE_WINDOW_WINDOW_SLAVE_H_
#define UKIVE_WINDOW_WINDOW_SLAVE_H_

#include <Windows.h>


namespace ukive {

    class Window;

    class WindowSlave {
    public:
        WindowSlave(Window *master);
        ~WindowSlave();

        void sync();

        HWND getWindowHandle();

        LRESULT CALLBACK messageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP);
        LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam, int leftExt, int topExt, int rightExt, int bottomExt);

    private:
        HWND mHandle;
        Window *mMaster;

        void initSlave();
    };

    static WindowSlave *sUWSVtr;
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

}

#endif  // UKIVE_WINDOW_WINDOW_SLAVE_H_