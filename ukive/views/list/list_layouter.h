#ifndef UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_
#define UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_

#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class ListView;
    class ListAdapter;
    class ViewHolderRecycler;

    class ListLayouter {
    public:
        virtual ~ListLayouter() = default;

        virtual int onLayoutAtPosition(
            ListView* parent, ListAdapter* adapter, bool cur) = 0;
        virtual int onScrollToPosition(
            ListView* parent, ListAdapter* adapter, int pos, int offset, bool cur) = 0;
        virtual int onSmoothScrollToPosition(
            ListView* parent, ListAdapter* adapter, int pos, int offset) = 0;

        virtual int onFillTopChildren(ListView* parent, ListAdapter* adapter, int dy) = 0;
        virtual int onFillBottomChildren(ListView* parent, ListAdapter* adapter, int dy) = 0;
        virtual int onFillLeftChildren(ListView* parent, ListAdapter* adapter, int dx) = 0;
        virtual int onFillRightChildren(ListView* parent, ListAdapter* adapter, int dx) = 0;

        virtual void onClear() = 0;

        virtual void recordCurPositionAndOffset(ListView* parent) = 0;
        virtual std::pair<int, int> computeTotalHeight(ListView* parent, ListAdapter* adapter) = 0;
        virtual ListAdapter::ViewHolder* findViewHolderFromView(View* v) = 0;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_LAYOUTER_H_