#ifndef UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_
#define UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class ViewGroup : public View
    {
    private:
        void initViewGroup();

    protected:
        virtual void dispatchDraw(Canvas *canvas) override;
        virtual void dispatchDiscardFocus() override;
        virtual void dispatchDiscardPendingOperations() override;

        virtual bool dispatchMouseEvent(InputEvent *e);
        virtual bool dispatchKeyboardEvent(InputEvent *e);

        virtual LayoutParams *generateLayoutParams(const LayoutParams &lp);
        virtual LayoutParams *generateDefaultLayoutParams();
        virtual bool checkLayoutParams(LayoutParams *lp);

        int getWrappedWidth();
        int getWrappedHeight();

    public:
        ViewGroup(Window *wnd);
        ViewGroup(Window *wnd, int id);
        ~ViewGroup();

        bool dispatchInputEvent(InputEvent *e) override;
        void dispatchWindowFocusChanged(bool windowFocus) override;
        void dispatchWindowDpiChanged(int dpi_x, int dpi_y) override;

        virtual void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) = 0;

        virtual bool onInterceptInputEvent(InputEvent *e);
        virtual bool onInterceptMouseEvent(InputEvent *e);
        virtual bool onInterceptKeyboardEvent(InputEvent *e);

        void onAttachedToWindow() override;
        void onDetachedFromWindow() override;

        void addView(View *v, LayoutParams *params = nullptr);
        void addView(std::size_t index, View *v, LayoutParams *params = nullptr);
        void removeView(View *v, bool del = true);
        void removeAllViews(bool del = true);

        View *findViewById(int id) override;

        static void getChildMeasure(
            int parentSize, int parentSizeMode,
            int padding, int childDimension,
            int *childSize, int *childSizeMode);

        std::size_t getChildCount();
        View *getChildById(int id);
        View *getChildAt(std::size_t index);

        void drawChild(Canvas *canvas, View *child);
        void drawChildren(Canvas *canvas);

        void measureChild(
            View *child,
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

        void measureChildWithMargins(
            View *child,
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

        void measureChildren(
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

        void measureChildrenWithMargins(
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

    private:
        std::vector<View*> view_list_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_