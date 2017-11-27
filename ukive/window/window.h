#ifndef UKIVE_WINDOW_WINDOW_H_
#define UKIVE_WINDOW_WINDOW_H_

#include <Windows.h>

#include <memory>

#include "ukive/utils/string_utils.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class View;
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

    enum Gravity;
    struct ClassInfo;

    class Window {
    public:
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
        void setMinWidth(int minWidth);
        void setMinHeight(int minHeight);
        void setCurrentCursor(LPCWSTR cursor);
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
        View *getKeyboardHolder();
        AnimationManager *getAnimationManager();

        bool isShowing();
        bool isCursorInClient();

        void captureMouse(View *widget);
        void releaseMouse();
        void captureKeyboard(View *widget);
        void releaseKeyboard();

        void invalidate();
        void requestLayout();

        View *findViewById(int id);

        ContextMenu *startContextMenu(
            ContextMenuCallback *callback, View *anchor, Gravity gravity);
        TextActionMode *startTextActionMode(TextActionModeCallback *callback);

        float dpToPx(float dp);
        float pxToDp(int px);

        virtual void onPreCreate(ClassInfo *info);
        virtual void onCreate();
        virtual void onShow(bool show);
        virtual void onActivate(int param);
        virtual void onDraw(Canvas *canvas);
        virtual void onMove(int x, int y);
        virtual void onResize(
            int param, int width, int height,
            int clientWidth, int clientHeight);
        virtual bool onMoving(RECT *rect);
        virtual bool onResizing(WPARAM edge, RECT *rect);
        virtual bool onClose();
        virtual void onDestroy();
        virtual bool onInputEvent(InputEvent *e);

    private:
        std::unique_ptr<WindowImpl> impl_;

        int min_width_, min_height_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_H_