#ifndef UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_
#define UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_

#include <vector>

#include "ukive/views/view.h"
#include "ukive/event/input_event.h"


namespace ukive {

    class ViewGroup : public View {
    public:
        explicit ViewGroup(Window* w);
        ViewGroup(Window* w, AttrsRef attrs);
        ~ViewGroup();

        bool dispatchInputEvent(InputEvent* e) override;
        void dispatchWindowFocusChanged(bool focus) override;
        void dispatchWindowDpiChanged(int dpi_x, int dpi_y) override;

        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override {}

        virtual bool onInterceptInputEvent(InputEvent* e);
        virtual LayoutParams* generateLayoutParamsByAttrs(AttrsRef attrs) const;

        void onAttachedToWindow() override;
        void onDetachedFromWindow() override;

        bool isViewGroup() const override;

        void addView(View* v, LayoutParams* params = nullptr, bool req_layout = true);

        /**
         * 往当前 ViewGroup 添加 View。
         * 添加的 View 将位于 index 之前。
         */
        void addView(int index, View* v, LayoutParams* params = nullptr, bool req_layout = true);
        void removeView(View* v, bool del = true, bool req_layout = true);
        void removeAllViews(bool del = true, bool req_layout = true);

        int getChildCount() const;
        View* getChildById(int id) const;
        View* getChildAt(int index) const;
        View* findViewById(int id) const override;

        void drawChild(Canvas* canvas, View* child);
        void drawChildren(Canvas* canvas);

        void measureChild(
            View* child,
            int parent_width, int parent_height,
            int parent_width_mode, int parent_height_mode);

        void measureChildWithMargins(
            View* child,
            int parent_width, int parent_height,
            int parent_width_mode, int parent_height_mode);

        void measureChildren(
            int parent_width, int parent_height,
            int parent_width_mode, int parent_height_mode);

        void measureChildrenWithMargins(
            int parent_width, int parent_height,
            int parent_width_mode, int parent_height_mode);

        static void getChildMeasure(
            int parent_size, int parent_size_mode,
            int padding, int child_dimension,
            int* child_size, int* child_size_mode);

    protected:
        // 重置 ViewGroup 记录的事件拦截状态。
        // 从 onInterceptInputEvent() 返回 true 开始到 XXX_UP 消息结束，
        // 所有的事件会被拦截，对于触摸操作来说没有问题，但对鼠标操作来讲，鼠标即使
        // 不按下，也可以有 MOVE 和 WHEEL 操作，对于这些操作拦截之后会没有办法取消
        // 拦截状态，因此设置该方法来手动重置状态。
        void invalidateInterceptStatus();

        void dispatchDraw(Canvas* canvas) override;
        void dispatchDiscardFocus() override;
        void dispatchDiscardPendingOperations() override;

        virtual bool dispatchPointerEvent(InputEvent* e);
        virtual bool dispatchKeyboardEvent(InputEvent* e);

        virtual bool checkLayoutParams(LayoutParams* lp) const;
        virtual LayoutParams* generateDefaultLayoutParams() const;
        virtual LayoutParams* generateLayoutParams(const LayoutParams& lp) const;

        int getWrappedWidth();
        int getWrappedHeight();

    private:
        void prepareInterceptingStatus(InputEvent* e);
        void updateInterceptingStatus(InputEvent* e);

        std::vector<View*> views_;
        bool is_intercepted_ = false;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_