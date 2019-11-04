#ifndef UKIVE_VIEWS_LIST_LINEAR_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_LINEAR_LIST_LAYOUTER_H_

#include "ukive/views/list/list_layouter.h"
#include "ukive/views/list/list_structs.h"


namespace ukive {

    class LinearListLayouter : public ListLayouter {
    public:
        LinearListLayouter();

        int onLayoutAtPosition(bool cur) override;
        int onScrollToPosition(int pos, int offset, bool cur) override;
        int onSmoothScrollToPosition(int pos, int offset) override;

        int onFillTopChildren(int dy) override;
        int onFillBottomChildren(int dy) override;
        int onFillLeftChildren(int dx) override;
        int onFillRightChildren(int dx) override;

        void onClear() override;

        void recordCurPositionAndOffset() override;
        void computeTotalHeight(int* prev, int* next) override;
        ListAdapter::ViewHolder* findViewHolderFromView(View* v) override;

        bool canScroll(Direction dir) const override;

    private:
        bool canScrollToTop() const;
        bool canScrollToBottom() const;
        bool canScrollToLeft() const;
        bool canScrollToRight() const;

        Column column_;

        int cur_position_;
        int cur_offset_in_position_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LINEAR_LIST_LAYOUTER_H_