#ifndef UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class LayoutParams;

    class LinearLayout : public ViewGroup {
    public:
        enum Orientation {
            VERTICAL = 1,
            HORIZONTAL = 2
        };

        LinearLayout(Window* w);

        void setOrientation(int orientation);

        void onMeasure(
            int width, int height,
            int widthSpec, int heightSpec) override;
        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;

    protected:
        bool checkLayoutParams(LayoutParams* lp) override;
        LayoutParams* generateDefaultLayoutParams() override;
        LayoutParams* generateLayoutParams(const LayoutParams& lp) override;

    private:
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

        int orientation_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_