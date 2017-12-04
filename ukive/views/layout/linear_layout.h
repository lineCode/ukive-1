#ifndef UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class LayoutParams;

    class LinearLayout : public ViewGroup
    {
    public:
        static const int VERTICAL = 1;
        static const int HORIZONTAL = 2;

    protected:
        LayoutParams *generateLayoutParams(const LayoutParams &lp) override;
        LayoutParams *generateDefaultLayoutParams() override;
        bool checkLayoutParams(LayoutParams *lp) override;

    private:
        int mOrientation;

        void measureWeightedChildren(
            int totalWeight,
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);
        void measureSequenceChildren(
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);
        void measureLinearLayoutChildren(
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

        void measureVertical(int width, int height, int widthSpec, int heightSpec);
        void measureHorizontal(int width, int height, int widthSpec, int heightSpec);

        void layoutVertical(int left, int top, int right, int bottom);
        void layoutHorizontal(int left, int top, int right, int bottom);

    public:
        LinearLayout(Window *wnd);
        LinearLayout(Window *wnd, int id);
        ~LinearLayout();

        void setOrientation(int orientation);

        virtual void onMeasure(
            int width, int height,
            int widthSpec, int heightSpec) override;
        virtual void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_