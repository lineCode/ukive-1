#ifndef UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class RestraintLayoutParams;

    class RestraintLayout : public ViewGroup
    {
    public:
        RestraintLayout(Window *w);

    protected:
        LayoutParams *generateLayoutParams(const LayoutParams &lp) override;
        LayoutParams *generateDefaultLayoutParams() override;
        bool checkLayoutParams(LayoutParams *lp) override;

    private:
        View *getChildById(int id);
        void clearMeasureFlag();

        void measureRestrainedChildren(
            int parentWidth, int parentHeight,
            int parentWidthMode, int parentHeightMode);

        void checkRestrainedChildrenWeight();

        void getRestrainedChildWidth(
            View *child, RestraintLayoutParams *lp,
            int parentWidth, int parentWidthMode, int *width, int *widthMode);

        void getRestrainedChildHeight(
            View *child, RestraintLayoutParams *lp,
            int parentHeight, int parentHeightMode, int *height, int *heightMode);

        int getLeftSpacing(View *child, RestraintLayoutParams *lp);
        int getTopSpacing(View *child, RestraintLayoutParams *lp);
        int getRightSpacing(View *child, RestraintLayoutParams *lp);
        int getBottomSpacing(View *child, RestraintLayoutParams *lp);

        int measureWrappedWidth();
        int measureWrappedHeight();

        void layoutChild(
            View *child, RestraintLayoutParams *lp,
            int left, int top, int right, int bottom);

        void layoutChildVertical(
            View *child, RestraintLayoutParams *lp,
            int top, int bottom);
        void layoutChildHorizontal(
            View *child, RestraintLayoutParams *lp,
            int left, int right);

    public:
        void onMeasure(
            int width, int height,
            int widthMode, int heightMode) override;
        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_H_