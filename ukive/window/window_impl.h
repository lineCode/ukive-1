#ifndef UKIVE_WINDOW_WINDOW_IMPL_H_
#define UKIVE_WINDOW_WINDOW_IMPL_H_

#include <Windows.h>

#include <memory>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Window;
    class NonClientFrame;

    class WindowImpl {
    public:
        WindowImpl(Window *win);
        ~WindowImpl();

        void init();

        void show();
        void hide();
        void focus();
        void close();
        void close(bool notify);

        void setTitle(string16 title);
        void setBound(int x, int y, int width, int height);
        void setStartupWindow(bool enable);

        string16 getTitle();
        int getX();
        int getY();
        int getWidth();
        int getHeight();

        bool isCreated();
        bool isShowing();
        bool isStartupWindow();

        void notifySizeChanged(
            int param, int width, int height,
            int clientWidth, int clientHeight);
        void notifyLocationChanged(int x, int y);

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        LRESULT CALLBACK messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        Window *delegate_;
        std::unique_ptr<NonClientFrame> non_client_frame_;

        HWND hWnd_;

        int x_, y_;
        int width_, height_;
        string16 title_;

        bool is_created_;
        bool is_showing_;
        bool is_startup_window_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_IMPL_H_