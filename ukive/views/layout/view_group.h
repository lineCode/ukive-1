#ifndef UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_
#define UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class ViewGroup : public View {
    public:
        ViewGroup(Window* w);
        ~ViewGroup();

        bool dispatchInputEvent(InputEvent* e) override;
        void dispatchWindowFocusChanged(bool windowFocus) override;
        void dispatchWindowDpiChanged(int dpi_x, int dpi_y) override;

        virtual void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) = 0;

        virtual bool onInterceptInputEvent(InputEvent* e);
        virtual bool onInterceptMouseEvent(InputEvent* e);
        virtual bool onInterceptKeyboardEvent(InputEvent* e);

        void onAttachedToWindow() override;
        void onDetachedFromWindow() override;

        void addView(View* v, LayoutParams* params = nullptr);
        void addView(size_t index, View* v, LayoutParams* params = nullptr);
        void removeView(View* v, bool del = true);
        void removeAllViews(bool del = true);

        size_t getChildCount();
        View* getChildById(int id);
        View* getChildAt(size_t index);
        View* findViewById(int id) override;

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
        void dispatchDraw(Canvas* canvas) override;
        void dispatchDiscardFocus() override;
        void dispatchDiscardPendingOperations() override;

        virtual bool dispatchMouseEvent(InputEvent* e);
        virtual bool dispatchKeyboardEvent(InputEvent* e);

        virtual bool checkLayoutParams(LayoutParams* lp);
        virtual LayoutParams* generateDefaultLayoutParams();
        virtual LayoutParams* generateLayoutParams(const LayoutParams& lp);

        int getWrappedWidth();
        int getWrappedHeight();

    private:
        std::vector<View*> view_list_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_