#ifndef UKIVE_WINDOW_WINDOW_IMPL_H_
#define UKIVE_WINDOW_WINDOW_IMPL_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <tpcshrd.h>

#include <map>
#include <memory>
#include <vector>

#include "utils/string_utils.h"

#include "ukive/graphics/cursor.h"
#include "ukive/graphics/point.h"

#define WM_NCDRAWCLASSIC1  0xAE
#define WM_NCDRAWCLASSIC2  0xAF
#define WM_NCMOUSEFIRST    WM_NCMOUSEMOVE
#define WM_NCMOUSELAST     WM_NCXBUTTONDBLCLK

#define WINDOW_MSG_HANDLER(msg, func)  \
    if (uMsg == msg) {                 \
        auto result = func(wParam, lParam, handled);  \
        if (*handled) return result; }

#define WINDOW_MSG_RANGE_HANDLER(first_msg, last_msg, func)  \
    if (uMsg >= first_msg && uMsg <= last_msg) {             \
        auto result = func(uMsg, wParam, lParam, handled);   \
        if (*handled) return result; }


namespace ukive {

    class Rect;
    class Window;
    class InputEvent;
    class NonClientFrame;
    struct ClassInfo;

    class WindowImpl {
    public:
        explicit WindowImpl(Window* win);
        ~WindowImpl();

        void init();

        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();
        void focus();
        void close();
        void center();

        void setTitle(const string16& title);
        void setBounds(int x, int y, int width, int height);
        void setCurrentCursor(Cursor cursor);
        void setTranslucent(bool translucent);
        void setBlurBehindEnabled(bool enabled);

        string16 getTitle() const;
        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        int getClientOffX() const;
        int getClientOffY() const;
        int getClientWidth(bool total = false) const;
        int getClientHeight(bool total = false) const;
        void getDpi(int* dpi_x, int* dpi_y) const;
        HWND getHandle() const;
        Cursor getCurrentCursor() const;

        bool isCreated() const;
        bool isShowing() const;
        bool isTranslucent() const;
        bool isMinimum() const;
        bool isMaximum() const;
        bool isPopup() const;

        void setMouseCaptureRaw();
        void releaseMouseCaptureRaw();

        void setMouseTrack();
        bool isMouseTrackEnabled();

        float dpToPxX(float dp);
        float dpToPxY(float dp);
        float pxToDpX(float px);
        float pxToDpY(float px);

        void setWindowStyle(int style, bool ex, bool enabled);
        void sendFrameChanged();

        void convScreenToClient(Point* p);
        void convClientToScreen(Point* p);

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        struct TouchInputCache {
            std::unique_ptr<TOUCHINPUT[]> cache;
            UINT size = 0;
        };

        void setWindowRectShape();
        static void disableTouchFeedback(HWND hWnd);
        int getPointerTypeFromMouseMsg();

        LRESULT processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP);

        LRESULT processWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled) {
            WINDOW_MSG_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, onMouseRange);
            WINDOW_MSG_RANGE_HANDLER(WM_NCMOUSEFIRST, WM_NCMOUSELAST, onMouseRange);

            WINDOW_MSG_HANDLER(WM_MOUSEHOVER, onMouseHover);
            WINDOW_MSG_HANDLER(WM_MOUSELEAVE, onMouseLeave);

            WINDOW_MSG_HANDLER(WM_NCCREATE, onNCCreate);
            WINDOW_MSG_HANDLER(WM_CREATE, onCreate);
            WINDOW_MSG_HANDLER(WM_NCDRAWCLASSIC1, onNCDrawClassic1);
            WINDOW_MSG_HANDLER(WM_NCDRAWCLASSIC2, onNCDrawClassic2);
            WINDOW_MSG_HANDLER(WM_NCPAINT, onNCPaint);
            WINDOW_MSG_HANDLER(WM_PAINT, onPaint);
            WINDOW_MSG_HANDLER(WM_NCACTIVATE, onNCActivate);
            WINDOW_MSG_HANDLER(WM_NCHITTEST, onNCHitTest);
            WINDOW_MSG_HANDLER(WM_NCCALCSIZE, onNCCalCSize);
            WINDOW_MSG_HANDLER(WM_CLOSE, onClose);
            WINDOW_MSG_HANDLER(WM_DESTROY, onDestroy);
            WINDOW_MSG_HANDLER(WM_NCDESTROY, onNCDestroy);
            WINDOW_MSG_HANDLER(WM_SHOWWINDOW, onShowWindow);
            WINDOW_MSG_HANDLER(WM_ACTIVATE, onActivate);
            WINDOW_MSG_HANDLER(WM_DPICHANGED, onDPIChanged);
            WINDOW_MSG_HANDLER(WM_STYLECHANGED, onStyleChanged);
            WINDOW_MSG_HANDLER(WM_COPYDATA, onCopyData);
            WINDOW_MSG_HANDLER(WM_ERASEBKGND, onEraseBkgnd);
            WINDOW_MSG_HANDLER(WM_SETCURSOR, onSetCursor);
            WINDOW_MSG_HANDLER(WM_SETFOCUS, onSetFocus);
            WINDOW_MSG_HANDLER(WM_KILLFOCUS, onKillFocus);
            WINDOW_MSG_HANDLER(WM_SETTEXT, onSetText);
            WINDOW_MSG_HANDLER(WM_SETICON, onSetIcon);
            WINDOW_MSG_HANDLER(WM_MOVE, onMove);
            WINDOW_MSG_HANDLER(WM_SIZE, onSize);
            WINDOW_MSG_HANDLER(WM_MOVING, onMoving);
            WINDOW_MSG_HANDLER(WM_SIZING, onSizing);
            WINDOW_MSG_HANDLER(WM_KEYDOWN, onKeyDown);
            WINDOW_MSG_HANDLER(WM_KEYUP, onKeyUp);
            WINDOW_MSG_HANDLER(WM_CHAR, onChar);
            WINDOW_MSG_HANDLER(WM_UNICHAR, onUniChar);
            WINDOW_MSG_HANDLER(WM_GESTURE, onGesture);
            WINDOW_MSG_HANDLER(WM_TOUCH, onTouch);
            WINDOW_MSG_HANDLER(WM_DWMCOMPOSITIONCHANGED, onDwmCompositionChanged);
            WINDOW_MSG_HANDLER(WM_WINDOWPOSCHANGED, onWindowPosChanged);

            return 0;
        }

        LRESULT onNCCreate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onCreate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCDrawClassic1(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCDrawClassic2(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCPaint(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onPaint(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCActivate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCHitTest(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCCalCSize(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMouseRange(UINT uMsg, WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMouseHover(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMouseLeave(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onClose(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDestroy(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onNCDestroy(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onShowWindow(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onActivate(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDPIChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onStyleChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onCopyData(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onEraseBkgnd(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetCursor(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetFocus(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onKillFocus(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetText(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSetIcon(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMove(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSize(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onMoving(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onSizing(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onKeyDown(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onKeyUp(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onChar(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onUniChar(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onGesture(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onTouch(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onDwmCompositionChanged(WPARAM wParam, LPARAM lParam, bool* handled);
        LRESULT onWindowPosChanged(WPARAM wParam, LPARAM lParam, bool* handled);

        void onCreate();
        void onShow(bool show);
        void onActivate(int param);
        void onSetFocus();
        void onKillFocus();
        void onDraw(const Rect& rect);
        void onMove(int x, int y);
        void onResize(int param, int width, int height);
        bool onMoving(Rect* rect);
        bool onResizing(WPARAM edge, Rect* rect);
        bool onClose();
        void onDestroy();
        bool onTouch(const TOUCHINPUT* inputs, int size);
        bool onInputEvent(InputEvent* e);
        void onDpiChanged(int dpi_x, int dpi_y);
        void onStyleChanged(bool normal, bool ext, const STYLESTRUCT* ss);
        bool onDataCopy(unsigned int id, unsigned int size, void* data);

        void EnableBlurBehindOnWin7();
        void EnableBlurBehindOnWin10();

        Window* delegate_;
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
        bool is_translucent_;
        bool is_enable_mouse_track_;
        bool is_first_nccalc_;
        bool is_blur_behind_enabled_ = false;

        TouchInputCache ti_cache_;
        std::map<DWORD, TOUCHINPUT> prev_ti_;

        int prev_touch_x_ = 0;
        int prev_touch_y_ = 0;
        bool is_prev_touched_ = false;
        uint64_t prev_touch_time_ = 0;

    };

}

#endif  // UKIVE_WINDOW_WINDOW_IMPL_H_