#ifndef UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_
#define UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class ViewGroup : public View {
    public:
        explicit ViewGroup(Window* w);
        ~ViewGroup();

        bool dispatchInputEvent(InputEvent* e) override;
        void dispatchWindowFocusChanged(bool focus) override;
        void dispatchWindowDpiChanged(int dpi_x, int dpi_y) override;

        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override {}

        virtual bool onInterceptInputEvent(InputEvent* e);

        void onAttachedToWindow() override;
        void onDetachedFromWindow() override;

        void addView(View* v, LayoutParams* params = nullptr);
        void addView(int index, View* v, LayoutParams* params = nullptr);
        void removeView(View* v, bool del = true);
        void removeAllViews(bool del = true);

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
        void dispatchDraw(Canvas* canvas) override;
        void dispatchDiscardFocus() override;
        void dispatchDiscardPendingOperations() override;

        virtual bool dispatchPointerEvent(InputEvent* e);
        virtual bool dispatchKeyboardEvent(InputEvent* e);

        virtual bool checkLayoutParams(LayoutParams* lp);
        virtual LayoutParams* generateDefaultLayoutParams();
        virtual LayoutParams* generateLayoutParams(const LayoutParams& lp);

        int getWrappedWidth();
        int getWrappedHeight();

    private:
        std::vector<View*> views_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_