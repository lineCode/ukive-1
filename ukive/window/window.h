#ifndef UKIVE_WINDOW_WINDOW_H_
#define UKIVE_WINDOW_WINDOW_H_

#include <Windows.h>

#include <memory>

#include "ukive/message/cycler.h"
#include "ukive/utils/string_utils.h"
#include "ukive/graphics/color.h"
#include "ukive/animation/animation_manager.h"
#include "ukive/views/view.h"


namespace ukive {

    class Rect;
    class Cycler;
    class Canvas;
    class Renderer;
    class InputEvent;
    class WindowImpl;
    class BaseLayout;
    class AnimationManager;
    class ContextMenu;
    class ContextMenuCallback;
    class TextActionMode;
    class TextActionModeCallback;
    struct ClassInfo;

    class Window {
    public:
        static const UINT MSG_INVALIDATE = 0;
        static const UINT MSG_RELAYOUT = 1;

        Window();
        virtual ~Window();

        void show();
        void hide();
        void focus();
        void close();
        void close(bool notify);
        void center();

        void setTitle(const string16 &title);
        void setX(int x);
        void setY(int y);
        void setPosition(int x, int y);
        void setWidth(int width);
        void setHeight(int height);
        void setBound(int x, int y, int width, int height);
        void setMinWidth(int min_width);
        void setMinHeight(int min_height);
        void setCurrentCursor(Cursor cursor);
        void setContentView(View *content);
        void setBackgroundColor(Color color);

        int getX();
        int getY();
        int getWidth();
        int getHeight();
        int getMinWidth();
        int getMinHeight();
        int getClientWidth();
        int getClientHeight();
        BaseLayout *getBaseLayout();
        Color getBackgroundColor();
        Cycler *getCycler();
        Renderer *getRenderer();
        HWND getHandle();
        AnimationManager *getAnimationManager();

        bool isShowing();
        bool isCursorInClient();

        void captureMouse(View *v);
        void releaseMouse();

        //当一个widget获取到焦点时，应调用此方法。
        void captureKeyboard(View *v);
        //当一个widget放弃焦点时，应调用此方法。
        void releaseKeyboard();

        View *getMouseHolder();
        unsigned int getMouseHolderRef();
        View *getKeyboardHolder();

        void invalidate();
        void invalidate(int left, int top, int right, int bottom);
        void requestLayout();

        void performLayout();
        void performRefresh();
        void performRefresh(int left, int top, int right, int bottom);

        View *findViewById(int id);

        ContextMenu *startContextMenu(
            ContextMenuCallback *callback, View *anchor, View::Gravity gravity);
        TextActionMode *startTextActionMode(TextActionModeCallback *callback);

        float dpToPx(float dp);
        float pxToDp(int px);

        virtual void onPreCreate(ClassInfo *info,
            int *win_style, int *win_ex_style);
        virtual void onCreate();
        virtual void onShow(bool show);
        virtual void onActivate(int param);
        virtual void onDraw(const Rect &rect);
        virtual void onMove(int x, int y);
        virtual void onResize(
            int param, int width, int height,
            int client_width, int client_height);
        virtual bool onMoving(Rect *rect);
        virtual bool onResizing(int edge, Rect *rect);
        virtual bool onClose();
        virtual void onDestroy();
        virtual bool onInputEvent(InputEvent *e);
        virtual void onDpiChanged(int dpi_x, int dpi_y);
        virtual bool onDataCopy(unsigned int id, unsigned int size, void *data);

        virtual void onDrawCanvas(Canvas *canvas);

    private:
        static const int SCHEDULE_RENDER = 0;
        static const int SCHEDULE_LAYOUT = 1;

        class UpdateCycler : public Cycler
        {
        public:
            UpdateCycler(Window *window)
                :win_(window) {}

            void handleMessage(Message *msg);
        private:
            Window * win_;
        };

        class AnimStateChangedListener
            : public AnimationManager::OnStateChangedListener
        {
        public:
            AnimStateChangedListener(Window *window)
                :win_(window) {}

            void onStateChanged(
                UI_ANIMATION_MANAGER_STATUS newStatus,
                UI_ANIMATION_MANAGER_STATUS previousStatus);
        private:
            Window * win_;
        };

        class AnimTimerEventListener
            : public AnimationManager::OnTimerEventListener
        {
        public:
            AnimTimerEventListener(Window *window)
                :window_(window) {}

            void OnPreUpdate();
            void OnPostUpdate();
            void OnRenderingTooSlow(unsigned int fps);

        private:
            Window *window_;
        };


        std::unique_ptr<WindowImpl> impl_;

        Canvas *mCanvas;
        Renderer *mRenderer;

        Cycler *mLabourCycler;
        BaseLayout *mBaseLayout;

        View *mMouseHolder;
        View *mFocusHolder;
        View *mFocusHolderBackup;
        unsigned int mMouseHolderRef;

        ContextMenu *mContextMenu;
        TextActionMode *mTextActionMode;

        AnimationManager *mAnimationManager;
        AnimationManager::OnStateChangedListener *mStateChangedListener;

        AnimStateChangedListener *mAnimStateChangedListener;
        AnimTimerEventListener *mAnimTimerEventListener;

        int min_width_, min_height_;
        Color background_color_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_H_