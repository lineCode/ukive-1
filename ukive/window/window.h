#ifndef UKIVE_WINDOW_WINDOW_H_
#define UKIVE_WINDOW_WINDOW_H_

#include <Windows.h>

#include <memory>

#include "utils/string_utils.h"


namespace ukive {

    class Canvas;
    class InputEvent;
    class WindowImpl;
    class ClassInfo;

    class Window {
    public:
        Window();
        virtual ~Window();

        void show();
        void hide();
        void focus();
        void close();
        void close(bool notify);

        virtual void onPreCreate(ClassInfo &info);
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
        virtual void onInputEvent(InputEvent *e);

    private:
        std::unique_ptr<WindowImpl> impl_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_H_