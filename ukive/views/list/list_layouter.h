#ifndef UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_

#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class ListView;
    class ListAdapter;
    class ViewHolderRecycler;

    class ListLayouter {
    public:
        enum Direction {
            TOP = 1 << 0,
            BOTTOM = 1 << 1,
            LEFT = 1 << 2,
            RIGHT = 1 << 3,

            HORIZONTAL = LEFT | RIGHT,
            VERTICAL = TOP | BOTTOM,
            ALL = HORIZONTAL | VERTICAL,
        };

        ListLayouter();
        virtual ~ListLayouter() = default;

        void bind(ListView* parent, ListAdapter* adapter);

        virtual void onMeasureAtPosition(bool cur, int width, int height) = 0;
        virtual int onLayoutAtPosition(bool cur) = 0;
        virtual int onScrollToPosition(int pos, int offset, bool cur) = 0;
        virtual int onSmoothScrollToPosition(int pos, int offset) = 0;

        virtual int onFillTopChildren(int dy) = 0;
        virtual int onFillBottomChildren(int dy) = 0;
        virtual int onFillLeftChildren(int dx) = 0;
        virtual int onFillRightChildren(int dx) = 0;

        virtual void onClear() = 0;

        virtual void recordCurPositionAndOffset() = 0;
        virtual void computeTotalHeight(int* prev, int* next) = 0;
        virtual ListAdapter::ViewHolder* findViewHolderFromView(View* v) = 0;

        virtual bool canScroll(Direction dir) const = 0;

    protected:
        bool isAvailable() const;

        ListView* parent_ = nullptr;
        ListAdapter* adapter_ = nullptr;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_