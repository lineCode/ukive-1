#ifndef UKIVE_WINDOW_WINDOW_IMPL_H_
#define UKIVE_WINDOW_WINDOW_IMPL_H_

#include <Windows.h>
#include <windowsx.h>

#include <memory>

#include "ukive/graphics/cursor.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Rect;
    class Window;
    class InputEvent;
    class NonClientFrame;
    struct ClassInfo;

    class WindowImpl {
    public:
        WindowImpl(Window *win);
        ~WindowImpl();

        void init();

        void show();
        void hide();
        void focus();
        void close();
        void center();

        void setTitle(const string16 &title);
        void setBound(int x, int y, int width, int height);
        void setCurrentCursor(Cursor cursor);

        string16 getTitle();
        int getX();
        int getY();
        int getWidth();
        int getHeight();
        int getClientWidth();
        int getClientHeight();
        unsigned int getDpi();
        HWND getHandle();
        Cursor getCurrentCursor();

        bool isCreated();
        bool isShowing();
        bool isCursorInClient();

        void setMouseCaptureRaw();
        void releaseMouseCaptureRaw();

        void setMouseTrack();
        bool isMouseTrackEnabled();

        float dpToPx(float dp);
        float pxToDp(int px);

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        LRESULT CALLBACK messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP);
        LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam, int leftExt, int topExt, int rightExt, int bottomExt);

        void onPreCreate(
            ClassInfo *info,
            int *win_style, int *win_ex_style);
        void onCreate();
        void onShow(bool show);
        void onActivate(int param);
        void onDraw(const Rect &rect);
        void onMove(int x, int y);
        void onResize(
            int param, int width, int height,
            int client_width, int client_height);
        bool onMoving(Rect *rect);
        bool onResizing(WPARAM edge, Rect *rect);
        bool onClose();
        void onDestroy();
        bool onInputEvent(InputEvent *e);
        void onDpiChanged(int dpi_x, int dpi_y);
        bool onDataCopy(unsigned int id, unsigned int size, void *data);

        Window *delegate_;
        std::unique_ptr<NonClientFrame> non_client_frame_;

        HWND hWnd_;
        Cursor cursor_;

        int x_, y_;
        int prev_x_, prev_y_;
        int width_, height_;
        int prev_width_, prev_height_;
        string16 title_;

        bool is_created_;
        bool is_showing_;
        bool is_enable_mouse_track_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_IMPL_H_