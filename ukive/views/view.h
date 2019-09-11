#ifndef UKIVE_VIEWS_VIEW_H_
#define UKIVE_VIEWS_VIEW_H_

#include <map>
#include <memory>

#include "ukive/utils/executable.h"
#include "ukive/graphics/rect.h"
#include "ukive/utils/string_utils.h"
#include "ukive/graphics/cursor.h"
#include "ukive/drawable/drawable.h"


namespace ukive {

    class Window;
    class Canvas;
    class Drawable;
    class InputEvent;
    class InputConnection;
    class LayoutParams;
    class OnClickListener;
    class ViewAnimator;
    class ShadowEffect;

    class View : public DrawableCallback {
    public:
        using Attributes = std::map<string16, string16>;
        using AttrsRef = const Attributes&;

        enum Gravity {
            LEFT,
            TOP,
            RIGHT,
            BOTTOM,
            CENTER,
            CENTER_HORIZONTAL,
            CENTER_VERTICAL,
        };

        enum Visibility {
            VISIBLE,
            INVISIBLE,
            VANISHED
        };

        enum MeasureMode {
            FIT,
            EXACTLY,
            UNKNOWN
        };

        enum Outline {
            OUTLINE_RECT,
            OUTLINE_OVAL,
        };

        explicit View(Window* w);
        View(Window* w, AttrsRef attrs);
        virtual ~View();

        ViewAnimator* animate();

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
        void setBackground(Drawable* drawable);
        void setForeground(Drawable* drawable);
        void setPadding(int left, int top, int right, int bottom);
        void setLayoutParams(LayoutParams* params);
        void setIsInputEventAtLast(bool is_last);
        void setPressed(bool pressed);
        void setCurrentCursor(Cursor cursor);
        void setClickable(bool clickable);
        void setFocusable(bool focusable);
        void setElevation(float elevation);
        void setReceiveOutsideInputEvent(bool receive);
        void setMinimumWidth(int width);
        void setMinimumHeight(int height);
        void setOnClickListener(OnClickListener* l);
        void setOutline(Outline outline);

        // Invoked by framework.
        void setParent(View* parent);

        void offsetTopAndBottom(int dy);
        void offsetLeftAndRight(int dx);

        double getX() const;
        double getY() const;
        double getAlpha() const;
        double getScaleX() const;
        double getScaleY() const;
        double getTranslateX() const;
        double getTranslateY() const;
        double getPivotX() const;
        double getPivotY() const;

        int getId() const;
        int getScrollX() const;
        int getScrollY() const;
        int getLeft() const;
        int getTop() const;
        int getRight() const;
        int getBottom() const;
        int getWidth() const;
        int getHeight() const;
        int getMeasuredWidth() const;
        int getMeasuredHeight() const;
        float getElevation() const;
        int getVisibility() const;
        int getOutline() const;

        int getMinimumWidth() const;
        int getMinimumHeight() const;

        int getPaddingLeft() const;
        int getPaddingTop() const;
        int getPaddingRight() const;
        int getPaddingBottom() const;

        LayoutParams* getLayoutParams() const;
        View* getParent() const;

        Window* getWindow() const;
        Drawable* getBackground() const;
        Drawable* getForeground() const;
        Drawable* getReleasedBackground();
        Drawable* getReleasedForeground();

        // bounds relative to parent.
        Rect getBounds() const;

        // bounds relative to window.
        Rect getBoundsInWindow() const;

        // bounds relative to screen.
        Rect getBoundsInScreen() const;

        // getBounds() with paddings relative to (0, 0).
        Rect getContentBounds() const;

        bool isEnabled() const;
        bool isAttachedToWindow() const;
        bool isInputEventAtLast() const;
        bool isPressed() const;
        bool hasFocus() const;
        bool isClickable() const;
        bool isFocusable() const;
        bool isLayouted() const;
        bool isLocalPointerInThis(InputEvent* e) const;
        bool isParentPointerInThis(InputEvent* e) const;
        bool isReceiveOutsideInputEvent() const;

        void scrollTo(int x, int y);
        void scrollBy(int dx, int dy);

        void draw(Canvas* canvas);
        void measure(int width, int height, int width_mode, int height_mode);
        void layout(int left, int top, int right, int bottom);

        virtual void invalidate();
        virtual void invalidate(const Rect &rect);
        virtual void invalidate(int left, int top, int right, int bottom);
        virtual void requestLayout();

        void requestFocus();
        void discardFocus();
        void discardMouseCapture();
        void discardPendingOperations();

        virtual View* findViewById(int id) const;
        virtual bool dispatchInputEvent(InputEvent* e);
        virtual void dispatchWindowFocusChanged(bool focus);
        virtual void dispatchWindowDpiChanged(int dpi_x, int dpi_y);

        virtual void onAttachedToWindow();
        virtual void onDetachedFromWindow();

        virtual bool isViewGroup() const;

    protected:
        void setMeasuredSize(int width, int height);

        void performClick();

        bool needDrawBackground();
        bool needDrawForeground();

        void drawBackground(Canvas* canvas);
        void drawForeground(Canvas* canvas);

        // DrawableCallback
        void onDrawableInvalidate(Drawable* d) override;

        bool dispatchInputEventToThis(InputEvent* e);
        virtual void dispatchDraw(Canvas* canvas) {}
        virtual void dispatchDiscardFocus() {}
        virtual void dispatchDiscardPendingOperations() {}

        virtual void onDraw(Canvas* canvas) {}
        virtual void onDrawOverChildren(Canvas* canvas) {}
        virtual void onMeasure(
            int width, int height,
            int width_mode, int height_mode);
        virtual void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) {}
        virtual bool onInputEvent(InputEvent* e);

        virtual bool onCheckIsTextEditor();
        virtual InputConnection* onCreateInputConnection();

        virtual void onSizeChanged(int width, int height, int old_width, int old_height) {}
        virtual void onVisibilityChanged(int visibility) {}
        virtual void onFocusChanged(bool get_focus);
        virtual void onWindowFocusChanged(bool window_focus);
        virtual void onWindowDpiChanged(int dpi_x, int dpi_y) {}
        virtual void onScrollChanged(
            int scroll_x, int scroll_y, int old_scroll_x, int old_scroll_y) {}

    private:
        enum Flags : uint32_t {
            MEASURED_SIZE_SET = 1,
            FORCE_LAYOUT = 1 << 1,
            BOUNDS_SET = 1 << 2,
            NEED_LAYOUT = 1 << 3,
            INVALIDATED = 1 << 4,
        };

        class ClickPerformer : public Executable {
        public:
            explicit ClickPerformer(View* v)
                :view_(v) {}
            void run() override {
                view_->performClick();
            }
        private:
            View* view_;
        };

        void updateDrawableState();
        void updateBackgroundState();
        void updateForegroundState();
        bool processInputEvent(InputEvent* e);

        int id_;
        Rect bounds_;
        Rect padding_;
        Outline outline_;

        uint32_t flags_;

        int scroll_x_;
        int scroll_y_;

        int measured_width_;
        int measured_height_;

        int old_pm_width_;
        int old_pm_height_;
        int old_pm_width_mode_;
        int old_pm_height_mode_;

        int min_width_;
        int min_height_;

        int visibility_;
        float elevation_;

        bool has_focus_;
        bool is_enabled_;
        bool is_attached_to_window_;
        bool is_input_event_at_last_;
        bool is_pressed_;
        bool is_clickable_ = false;
        bool is_focusable_;
        bool is_receive_outside_input_event_;
        bool is_mouse_down_;
        bool is_touch_down_;

        Window* window_;
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
        std::unique_ptr<ShadowEffect> shadow_effect_;
        std::unique_ptr<LayoutParams> layout_params_;
        std::unique_ptr<InputEvent> cur_ev_;

        View* parent_;
        OnClickListener* click_listener_;
        ClickPerformer* click_performer_;
        InputConnection* input_connection_;
    };

}

#endif  // UKIVE_VIEWS_VIEW_H_