#ifndef UKIVE_VIEWS_VIEW_H_
#define UKIVE_VIEWS_VIEW_H_

#include <memory>

#include "ukive/utils/executable.h"
#include "ukive/graphics/rect.h"
#include "ukive/utils/string_utils.h"
#include "ukive/graphics/cursor.h"


namespace ukive {

    class Window;
    class Canvas;
    class Drawable;
    class InputEvent;
    class InputConnection;
    class LayoutParams;
    class OnClickListener;
    class ViewAnimator;

    class View {
    public:
        enum MeasureOption {
            FIT,
            EXACTLY,
            UNKNOWN
        };

        enum Visibility {
            VISIBLE,
            INVISIBLE,
            VANISHED
        };

        enum Gravity {
            LEFT,
            TOP,
            RIGHT,
            BOTTOM,
            CENTER,
            CENTER_HORIZONTAL,
            CENTER_VERTICAL,
        };

        View(Window *w);
        virtual ~View();

        ViewAnimator *animate();

        void setId(int id);
        void setX(double x);
        void setY(double y);
        void setAlpha(double alpha);
        void setScaleX(double sx);
        void setScaleY(double sy);
        void setTranslateX(double tx);
        void setTranslateY(double ty);
        void setPivotX(double px);
        void setPivotY(double py);

        void setRevealType(int type);
        void setHasReveal(bool reveal);
        void setRevealRadius(double radius);
        void setRevealCenterX(double cx);
        void setRevealCenterY(double cy);
        void setRevealWidthRadius(double widthRadius);
        void setRevealHeightRadius(double heightRadius);

        void setScrollX(int x);
        void setScrollY(int y);
        void setVisibility(int visibility);
        void setEnabled(bool enable);
        void setBackground(Drawable *drawable);
        void setForeground(Drawable *drawable);
        void setPadding(int left, int top, int right, int bottom);
        void setLayoutParams(LayoutParams *params);
        void setIsInputEventAtLast(bool isInput);
        void setPressed(bool pressed);
        void setCurrentCursor(Cursor cursor);
        void setFocusable(bool focusable);
        void setElevation(float elevation);
        void setReceiveOutsideInputEvent(bool receive);
        void setCanConsumeMouseEvent(bool enable);

        void setMinimumWidth(int width);
        void setMinimumHeight(int height);

        void setOnClickListener(OnClickListener *l);

        //设置该 view 的父 view，该方法由框架调用。
        void setParent(View *parent);

        void offsetTopAndBottom(int dy);
        void offsetLeftAndRight(int dx);

        double getX();
        double getY();
        double getAlpha();
        double getScaleX();
        double getScaleY();
        double getTranslateX();
        double getTranslateY();
        double getPivotX();
        double getPivotY();

        int getId();
        int getScrollX();
        int getScrollY();
        int getLeft();
        int getTop();
        int getRight();
        int getBottom();
        int getWidth();
        int getHeight();
        int getMeasuredWidth();
        int getMeasuredHeight();
        float getElevation();
        int getVisibility();

        int getMinimumWidth();
        int getMinimumHeight();

        int getPaddingLeft();
        int getPaddingTop();
        int getPaddingRight();
        int getPaddingBottom();

        LayoutParams *getLayoutParams();
        View *getParent();

        Window *getWindow();
        Drawable *getBackground();
        Drawable *getForeground();

        Rect getBounds();
        Rect getBoundsInWindow();
        Rect getBoundsInScreen();

        Rect getContentBounds();
        Rect getContentBoundsInThis();

        virtual View *findViewById(int id);

        bool isEnabled();
        bool isAttachedToWindow();
        bool isInputEventAtLast();
        bool isPressed();
        bool hasFocus();
        bool isFocusable();
        bool isLayouted();
        bool isLocalMouseInThis(InputEvent *e);
        bool isParentMouseInThis(InputEvent *e);
        bool isReceiveOutsideInputEvent();
        bool canConsumeMouseEvent();

        void scrollTo(int x, int y);
        void scrollBy(int dx, int dy);

        void draw(Canvas *canvas);
        void measure(int width, int height, int widthMode, int heightMode);
        void layout(int left, int top, int right, int bottom);

        void invalidate();
        void invalidate(const Rect &rect);
        void invalidate(int left, int top, int right, int bottom);
        void requestLayout();

        void requestFocus();
        void discardFocus();
        void discardPendingOperations();

        virtual bool dispatchInputEvent(InputEvent *e);
        virtual void dispatchWindowFocusChanged(bool windowFocus);
        virtual void dispatchWindowDpiChanged(int dpi_x, int dpi_y);

        virtual void onAttachedToWindow();
        virtual void onDetachedFromWindow();

    protected:
        void performClick();

        void drawBackground(Canvas *canvas);
        void drawForeground(Canvas *canvas);

        void setMeasuredDimension(int width, int height);

        virtual void dispatchDraw(Canvas *canvas);
        virtual void dispatchDiscardFocus();
        virtual void dispatchDiscardPendingOperations();

        virtual void onDraw(Canvas *canvas);
        virtual void onMeasure(
            int width, int height,
            int widthMode, int heightMode);
        virtual void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom);
        virtual bool onInputEvent(InputEvent *e);

        virtual bool onCheckIsTextEditor();
        virtual InputConnection *onCreateInputConnection();

        virtual void onSizeChanged(int width, int height, int oldWidth, int oldHeight);
        virtual void onVisibilityChanged(int visibility);
        virtual void onFocusChanged(bool getFocus);
        virtual void onWindowFocusChanged(bool windowFocus);
        virtual void onWindowDpiChanged(int dpi_x, int dpi_y);
        virtual void onScrollChanged(int scrollX, int scrollY, int oldScrollX, int oldScrollY);

    private:
        class ClickPerformer
            : public Executable {
        public:
            ClickPerformer(View *v)
                :view_(v) {}
            void run() override {
                view_->performClick();
            }
        private:
            View *view_;
        };


        int id_;
        Rect bounds_;
        Rect padding_;

        int scroll_x_;
        int scroll_y_;

        int measured_width_;
        int measured_height_;

        int min_width_;
        int min_height_;

        int visibility_;
        float elevation_;

        bool has_focus_;
        bool is_enabled_;
        bool is_attached_to_window_;
        bool is_input_event_at_last_;
        bool is_pressed_;
        bool is_focusable_;
        bool is_layouted_;
        bool is_receive_outside_input_event_;
        bool can_consume_mouse_event_;

        Window *window_;
        std::unique_ptr<Drawable> bg_drawable_;
        std::unique_ptr<Drawable> fg_drawable_;

        //普通动画变量。
        double mAlpha;
        double mScaleX;
        double mScaleY;
        double mTranslateX;
        double mTranslateY;
        double mPivotX, mPivotY;

        //揭露动画变量。
        int mRevealType;
        bool mHasReveal;
        double mRevealRadius;
        double mRevealCenterX;
        double mRevealCenterY;
        double mRevealWidthRadius;
        double mRevealHeightRadius;

        std::unique_ptr<ViewAnimator> animator_;

        View *parent_;
        LayoutParams *layout_params_;
        OnClickListener *click_listener_;
        ClickPerformer *click_performer_;
        InputConnection *input_connection_;
    };

}

#endif  // UKIVE_VIEWS_VIEW_H_