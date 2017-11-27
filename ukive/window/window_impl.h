#ifndef UKIVE_WINDOW_WINDOW_IMPL_H_
#define UKIVE_WINDOW_WINDOW_IMPL_H_

#include <Windows.h>
#include <windowsx.h>

#include <memory>

#include "ukive/utils/string_utils.h"
#include "ukive/message/cycler.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/animation/animation_manager.h"
#include "ukive/views/view.h"


namespace ukive {

    class Window;
    class Renderer;
    class BaseLayout;
    class ContextMenu;
    class TextActionMode;
    class NonClientFrame;
    class ContextMenuCallback;
    class TextActionModeCallback;
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
        void close(bool notify);
        void center();

        void setTitle(const string16 &title);
        void setBound(int x, int y, int width, int height);
        void setStartupWindow(bool enable);
        void setCurrentCursor(LPCWSTR cursor);
        void setContentView(View *content);
        void setBackgroundColor(const Color &color);

        string16 getTitle();
        int getX();
        int getY();
        int getWidth();
        int getHeight();
        int getClientWidth();
        int getClientHeight();
        unsigned int getDpi();
        HWND getHandle();
        HCURSOR getCurrentCursor();
        Color getBackgroundColor();
        BaseLayout *getBaseLayout();
        Cycler *getCycler();
        Renderer *getRenderer();
        AnimationManager *getAnimationManager();

        bool isCreated();
        bool isShowing();
        bool isStartupWindow();
        bool isCursorInClient();

        void notifySizeChanged(
            int param, int width, int height,
            int clientWidth, int clientHeight);
        void notifyLocationChanged(int x, int y);

        void invalidate();
        void invalidate(int left, int top, int right, int bottom);
        void requestLayout();

        void performLayout();
        void performRefresh();
        void performRefresh(int left, int top, int right, int bottom);

        View *findWidgetById(int id);

        void captureMouse(View *widget);
        void releaseMouse();
        View *getMouseHolder();
        unsigned int getMouseHolderRef();

        //当一个widget获取到焦点时，应调用此方法。
        void captureKeyboard(View *widget);

        //当一个widget放弃焦点时，应调用此方法。
        void releaseKeyboard();
        View *getKeyboardHolder();

        ContextMenu *startContextMenu(
            ContextMenuCallback *callback, View *anchor, Gravity gravity);
        TextActionMode *startTextActionMode(TextActionModeCallback *callback);

        float dpToPx(float dp);
        float pxToDp(int px);

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    public:
        static const UINT MSG_INVALIDATE = 0;
        static const UINT MSG_RELAYOUT = 1;

    private:
        void setMouseTrack();

        LRESULT CALLBACK messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT processDWMProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP);
        LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam, int leftExt, int topExt, int rightExt, int bottomExt);

        class UpdateCycler : public Cycler
        {
        private:
           WindowImpl *mWindow;
        public:
            UpdateCycler(WindowImpl *window);

            void handleMessage(Message *msg);
        };

        class AnimStateChangedListener
            : public AnimationManager::OnStateChangedListener
        {
        private:
            WindowImpl *mWindow;
        public:
            AnimStateChangedListener(WindowImpl *window);

            void onStateChanged(
                UI_ANIMATION_MANAGER_STATUS newStatus,
                UI_ANIMATION_MANAGER_STATUS previousStatus);
        };

        class AnimTimerEventListener
            : public AnimationManager::OnTimerEventListener
        {
        private:
            WindowImpl *mWindow;
        public:
            AnimTimerEventListener(WindowImpl *window);

            void OnPreUpdate();
            void OnPostUpdate();
            void OnRenderingTooSlow(unsigned int fps);
        };

        void onPreCreate(ClassInfo *info);
        void onCreate();
        void onShow(bool show);
        void onActivate(int param);
        void onDraw(const Rect &rect);
        void onMove(int x, int y);
        void onResize(
            int param, int width, int height,
            int clientWidth, int clientHeight);
        bool onMoving(Rect *rect);
        bool onResizing(WPARAM edge, Rect *rect);
        bool onClose();
        void onDestroy();
        bool onInputEvent(InputEvent *e);
        void onDpiChanged(int dpi_x, int dpi_y);

        Cycler *mLabourCycler;
        AnimStateChangedListener *mAnimStateChangedListener;
        AnimTimerEventListener *mAnimTimerEventListener;

    private:
        static const int SCHEDULE_RENDER = 0;
        static const int SCHEDULE_LAYOUT = 1;

        Window *delegate_;
        std::unique_ptr<NonClientFrame> non_client_frame_;

        Canvas *mCanvas;
        Renderer *mRenderer;

        AnimationManager *mAnimationManager;
        AnimationManager::OnStateChangedListener *mStateChangedListener;
        BaseLayout *mBaseLayout;
        ContextMenu *mContextMenu;
        TextActionMode *mTextActionMode;

        View *mMouseHolder;
        View *mFocusHolder;
        View *mFocusHolderBackup;
        unsigned int mMouseHolderRef;

        HWND hWnd_;
        HCURSOR cursor_;

        int x_, y_;
        int prev_x_, prev_y_;
        int width_, height_;
        int prev_width_, prev_height_;
        string16 title_;
        Color background_color_;

        bool is_created_;
        bool is_showing_;
        bool is_startup_window_;
        bool is_enable_mouse_track_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_IMPL_H_