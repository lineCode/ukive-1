#ifndef UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_
#define UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_

#include <memory>
#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class ViewGroup : public View
    {
    private:
        std::vector<std::shared_ptr<View>> mWidgetList;

        void initViewGroup();

    protected:
        virtual void dispatchDraw(Canvas *canvas) override;
        virtual void dispatchDiscardFocus() override;
        virtual void dispatchDiscardPendingOperations() override;

        virtual bool dispatchMouseEvent(InputEvent *e);
        virtual bool dispatchKeyboardEvent(InputEvent *e);

        virtual LayoutParams *generateLayoutParams(LayoutParams *lp);
        virtual LayoutParams *generateDefaultLayoutParams();
        virtual bool checkLayoutParams(LayoutParams *lp);

        int getWrappedWidth();
        int getWrappedHeight();

    public:
        ViewGroup(Window *wnd);
        ViewGroup(Window *wnd, int id);
        virtual ~ViewGroup();

        virtual bool dispatchInputEvent(InputEvent *e) override;
        virtual void dispatchWindowFocusChanged(bool windowFocus) override;

        virtual void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) = 0;

        virtual bool onInterceptInputEvent(InputEvent *e);
        virtual bool onInterceptMouseEvent(InputEvent *e);
        virtual bool onInterceptKeyboardEvent(InputEvent *e);

        virtual void onAttachedToWindow() override;
        virtual void onDetachedFromWindow() override;

        void addWidget(View *widget);
        void addWidget(View *widget, LayoutParams *params);
        void addWidget(std::size_t index, View *widget, LayoutParams *params);
        void addWidget(std::shared_ptr<View> widget);
        void addWidget(std::shared_ptr<View> widget, LayoutParams *params);
        void addWidget(std::size_t index, std::shared_ptr<View> widget, LayoutParams *params);
        void removeWidget(View *widget);
        void removeWidget(std::shared_ptr<View> widget);
        void removeWidget(std::size_t index);

        virtual View *findWidgetById(int id) override;

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
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_VIEW_GROUP_H_