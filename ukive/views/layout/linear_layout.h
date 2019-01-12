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
            int width_mode, int height_mode) override;
        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override;

    protected:
        bool checkLayoutParams(LayoutParams* lp) override;
        LayoutParams* generateDefaultLayoutParams() override;
        LayoutParams* generateLayoutParams(const LayoutParams& lp) override;

    private:
        void measureLinearChild(
            View* child, int parent_w, int parent_h, int parent_wm, int parent_hm);

        void measureWeightedChildren(
            int total_weight,
            int parent_w, int parent_h,
            int parent_wm, int parent_hm);
        void measureSequenceChildren(
            int parent_w, int parent_h,
            int parent_wm, int parent_hm);
        void measureFillModeChildren(
            int parent_w, int parent_h,
            int parent_wm, int parent_hm);
        void measureLinearLayoutChildren(
            int parent_w, int parent_h,
            int parent_wm, int parent_hm);

        void measureVertical(int width, int height, int width_mode, int height_mode);
        void measureHorizontal(int width, int height, int width_mode, int height_mode);

        void layoutVertical(int left, int top, int right, int bottom);
        void layoutHorizontal(int left, int top, int right, int bottom);

        int orientation_;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_H_