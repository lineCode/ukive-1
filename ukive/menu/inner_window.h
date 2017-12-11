#ifndef UKIVE_MENU_INNER_WINDOW_H_
#define UKIVE_MENU_INNER_WINDOW_H_

#include "ukive/views/layout/frame_layout.h"


namespace ukive {

    enum Gravity;
    class Canvas;
    class Window;
    class View;
    class InputEvent;
    class Drawable;

    class InnerWindow
    {
    public:
        class InnerDecorView : public FrameLayout
        {
        private:
            InnerWindow *mInnerWindow;

        public:
            InnerDecorView(InnerWindow *inner);
            InnerDecorView(InnerWindow *inner, int id);
            ~InnerDecorView();

            bool onInterceptInputEvent(InputEvent *e) override;
            bool onInputEvent(InputEvent *e) override;
        };

    private:
        int mWidth;
        int mHeight;
        float mElevation;
        bool mOutsideTouchable;
        bool mDismissByTouchOutside;
        Drawable *mBackgroundDrawable;

        Window *mParent;
        View *mContentView;
        InnerDecorView *mDecorView;
        bool mIsShowing;

        void createDecorView();

    public:
        InnerWindow(Window *wnd);
        virtual ~InnerWindow();

        void setWidth(int width);
        void setHeight(int height);
        void setSize(int width, int height);
        void setElevation(float elevation);
        void setBackground(Drawable *drawable);
        void setOutsideTouchable(bool touchable);
        void setDismissByTouchOutside(bool enable);
        void setContentView(View *contentView);

        int getWidth();
        int getHeight();
        float getElevation();
        Drawable *getBackground();
        bool isOutsideTouchable();
        bool isDismissByTouchOutside();
        Window *getParent();
        View *getContentView();
        View *getDecorView();

        bool isShowing();

        void show(int x, int y);
        void show(View *anchor, Gravity gravity);
        void update(int x, int y);
        void update(View *anchor, Gravity gravity);
        void dismiss();
    };

}

#endif  // UKIVE_MENU_INNER_WINDOW_H_