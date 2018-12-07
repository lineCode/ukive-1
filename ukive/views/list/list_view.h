#ifndef UKIVE_VIEWS_LIST_LIST_VIEW_H_
#define UKIVE_VIEWS_LIST_LIST_VIEW_H_

#include <list>
#include <memory>

#include "ukive/views/layout/view_group.h"
#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class OverlayScrollBar;
    class ViewHolderRecycler;


    class ListView : public ViewGroup, public ListDataSetChangedListener {
    public:
        ListView(Window* w);

        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override;
        bool onInputEvent(InputEvent* e) override;

        void onDraw(Canvas* canvas) override;
        void onDrawOverChild(Canvas* canvas) override;

        void setAdapter(ListAdapter* adapter);
        void scrollToPosition(int position, int offset, bool smooth);

    private:
        int determineVerticalScroll(int dy);
        void offsetChildViewTopAndBottom(int dy);
        ListAdapter::ViewHolder* getBindViewHolderAt(int index, int i);

        ListAdapter::ViewHolder* getFirstVisibleVH();
        ListAdapter::ViewHolder* getLastVisibleVH();

        void recycleTopViews(int offset);
        void recycleBottomViews(int offset);

        void updateOverlayScrollBar();
        void recordCurPositionAndOffset();

        int fillTopChildViews(int dy);
        int fillBottomChildViews(int dy);

        void locateToPosition(int position, int offset = 0);
        void scrollToPosition(int position, int offset = 0);
        void smoothScrollToPosition(int position, int offset = 0);

        void scrollByScrollBar(int dy);

        // ListScrollDelegate:
        //void OnScroll(float dx, float dy) OVERRIDE;

        // ListDataSetListener:
        void onDataSetChanged() override;
        void onItemRangeInserted(int start_position, int length) override;
        void onItemRangeChanged(int start_position, int length) override;
        void onItemRangeRemoved(int start_position, int length) override;

        int cur_position_;
        int cur_offset_in_position_;
        bool initial_layouted_;

        std::unique_ptr<ListAdapter> adapter_;
        std::unique_ptr<OverlayScrollBar> scroll_bar_;
        std::unique_ptr<ViewHolderRecycler> recycler_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_VIEW_H_