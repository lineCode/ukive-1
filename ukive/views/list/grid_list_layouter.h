#ifndef UKIVE_VIEWS_LIST_GRID_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_GRID_LIST_LAYOUTER_H_

#include "ukive/views/list/list_layouter.h"
#include "ukive/views/list/list_structs.h"


namespace ukive {

    class GridListLayouter : public ListLayouter {
    public:
        GridListLayouter();

        int onLayoutAtPosition(
            ListView* parent, ListAdapter* adapter, bool cur) override;
        int onScrollToPosition(
            ListView* parent, ListAdapter* adapter, int pos, int offset, bool cur) override;
        int onSmoothScrollToPosition(
            ListView* parent, ListAdapter* adapter, int pos, int offset) override;

        int onFillTopChildren(ListView* parent, ListAdapter* adapter, int dy) override;
        int onFillBottomChildren(ListView* parent, ListAdapter* adapter, int dy) override;
        int onFillLeftChildren(ListView* parent, ListAdapter* adapter, int dx) override;
        int onFillRightChildren(ListView* parent, ListAdapter* adapter, int dx) override;

        void onClear() override;

        void recordCurPositionAndOffset(ListView* parent) override;
        std::pair<int, int> computeTotalHeight(ListView* parent, ListAdapter* adapter) override;
        ListAdapter::ViewHolder* findViewHolderFromView(View* v) override;

    private:
        int col_count_;
        ColumnCollection columns_;

        int cur_row_;
        int cur_offset_in_row_;
    };

}

#endif  // UKIVE_VIEWS_LIST_GRID_LIST_LAYOUTER_H_