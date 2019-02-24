#ifndef UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class RestraintLayoutParams;

    class RestraintLayout : public ViewGroup {
    public:
        explicit RestraintLayout(Window* w);
        RestraintLayout(Window* w, AttrsRef attrs);

        // ViewGroup
        void onMeasure(
            int width, int height,
            int widthMode, int heightMode) override;
        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;

    protected:
        // ViewGroup
        LayoutParams* generateLayoutParams(const LayoutParams &lp) const override;
        LayoutParams* generateDefaultLayoutParams() const override;
        LayoutParams* generateLayoutParamsByAttrs(AttrsRef attrs) const override;
        bool checkLayoutParams(LayoutParams* lp) const override;

    private:
        void clearMeasureFlag();

        void measureRestrainedChildren(
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

        void checkRestrainedChildrenWeight();

        void getRestrainedChildWidth(
            View* child, RestraintLayoutParams* lp,
            int parentWidth, int parentWidthMode, int* width, int* widthMode);

        void getRestrainedChildHeight(
            View* child, RestraintLayoutParams* lp,
            int parentHeight, int parentHeightMode, int* height, int* heightMode);

        int getLeftSpacing(View* child, RestraintLayoutParams* lp);
        int getTopSpacing(View* child, RestraintLayoutParams* lp);
        int getRightSpacing(View* child, RestraintLayoutParams* lp);
        int getBottomSpacing(View* child, RestraintLayoutParams* lp);

        int measureWrappedWidth();
        int measureWrappedHeight();

        void layoutChild(
            View* child, RestraintLayoutParams* lp,
            int left, int top, int right, int bottom);

        void layoutChildVertical(
            View* child, RestraintLayoutParams* lp,
            int top, int bottom);
        void layoutChildHorizontal(
            View* child, RestraintLayoutParams* lp,
            int left, int right);
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_