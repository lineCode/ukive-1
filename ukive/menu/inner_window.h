#ifndef UKIVE_MENU_INNER_WINDOW_H_
#define UKIVE_MENU_INNER_WINDOW_H_

#include "ukive/views/layout/frame_layout.h"


namespace ukive {

    class Canvas;
    class Window;
    class InputEvent;
    class Drawable;

    class OnInnerWindowEventListener {
    public:
        virtual ~OnInnerWindowEventListener() = default;

        virtual void onRequestDismissByTouchOutside() = 0;
    };

    class InnerWindow {
    public:
        class InnerDecorView : public FrameLayout {
        public:
            explicit InnerDecorView(InnerWindow* inner);
            ~InnerDecorView();

            bool dispatchInputEvent(InputEvent* e) override;

            bool onInterceptInputEvent(InputEvent* e) override;
            bool onInputEvent(InputEvent* e) override;

        private:
            InnerWindow* inner_window_;
        };

        explicit InnerWindow(Window* wnd);
        virtual ~InnerWindow();

        void setWidth(int width);
        void setHeight(int height);
        void setSize(int width, int height);
        void setElevation(float elevation);
        void setBackground(Drawable* drawable);
        void setOutsideTouchable(bool touchable);
        void setDismissByTouchOutside(bool enable);
        void setContentView(View* contentView);
        void setEventListener(OnInnerWindowEventListener* l);

        int getWidth();
        int getHeight();
        float getElevation();
        Drawable* getBackground();
        bool isOutsideTouchable();
        bool isDismissByTouchOutside();
        Window* getParent();
        View* getContentView();
        View* getDecorView();

        bool isShowing();

        void show(int x, int y);
        void show(View* anchor, View::Gravity gravity);
        void update(int x, int y);
        void update(View* anchor, View::Gravity gravity);
        void dismiss();

    private:
        void createDecorView();

        int width_;
        int height_;
        float elevation_;
        bool outside_touchable_;
        bool dismiss_by_touch_outside_;
        Drawable* background_drawable_;

        Window* parent_;
        View* content_view_;
        InnerDecorView* decor_view_;
        OnInnerWindowEventListener* listener_ = nullptr;
        bool is_showing_;
    };

}

#endif  // UKIVE_MENU_INNER_WINDOW_H_