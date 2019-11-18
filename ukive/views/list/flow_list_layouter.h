#ifndef UKIVE_VIEWS_LIST_FLOW_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_FLOW_LIST_LAYOUTER_H_

#include "ukive/views/list/list_layouter.h"
#include "ukive/views/list/list_structs.h"


namespace ukive {

    class FlowListLayouter : public ListLayouter {
    public:
        FlowListLayouter();

        void onMeasureAtPosition(bool cur, int width, int height) override;
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
        struct Record {
            int cur_row = 0;
            int cur_offset = 0;
            bool is_null = true;
        };

        int calPreferredCurPos() const;

        bool canScrollToTop() const;
        bool canScrollToBottom() const;
        bool canScrollToLeft() const;
        bool canScrollToRight() const;

        int col_count_;
        ColumnCollection columns_;
        std::vector<Record> cur_records_;
    };

}

#endif  // UKIVE_VIEWS_LIST_FLOW_LIST_LAYOUTER_H_