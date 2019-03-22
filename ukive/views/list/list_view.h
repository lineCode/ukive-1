#ifndef UKIVE_VIEWS_LIST_LIST_VIEW_H_
#define UKIVE_VIEWS_LIST_LIST_VIEW_H_

#include <list>
#include <memory>

#include "ukive/views/layout/view_group.h"
#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class ListLayouter;
    class OverlayScrollBar;
    class ViewHolderRecycler;

    class ListView : public ViewGroup, public ListDataSetChangedListener {
    public:
        explicit ListView(Window* w);
        ListView(Window* w, AttrsRef attrs);

        // ViewGroup
        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override;
        bool onInputEvent(InputEvent* e) override;
        void onDraw(Canvas* canvas) override;
        void onDrawOverChildren(Canvas* canvas) override;

        void setAdapter(ListAdapter* adapter);
        void setLayouter(ListLayouter* layouter);
        void scrollToPosition(int pos, int offset, bool smooth);

    private:
        int determineVerticalScroll(int dy);
        void offsetChildViewTopAndBottom(int dy);
        ListAdapter::ViewHolder* makeNewBindViewHolder(int adapter_pos, int view_index);
        void recycleViewHolder(ListAdapter::ViewHolder* holder);

        int findViewIndexFromStart(ListAdapter::ViewHolder* holder) const;
        int findViewIndexFromEnd(ListAdapter::ViewHolder* holder) const;

        int measureViewHolder(
            ListAdapter::ViewHolder* holder, int width);
        void layoutViewHolder(
            ListAdapter::ViewHolder* holder, int left, int top, int width, int height);

        void updateOverlayScrollBar();
        void recordCurPositionAndOffset();

        int fillTopChildViews(int dy);
        int fillBottomChildViews(int dy);

        void layoutAtPosition(bool cur);
        void directScrollToPosition(int pos, int offset, bool cur);
        void smoothScrollToPosition(int pos, int offset);

        void onScrollBarChanged(int dy);

        // ListScrollDelegate:
        //void OnScroll(float dx, float dy) OVERRIDE;

        // ListDataSetListener:
        void onDataSetChanged() override;
        void onItemRangeInserted(int start_pos, int length) override;
        void onItemRangeChanged(int start_pos, int length) override;
        void onItemRangeRemoved(int start_pos, int length) override;

        bool initial_layouted_;

        std::unique_ptr<ListAdapter> adapter_;
        std::unique_ptr<ListLayouter> layouter_;
        std::unique_ptr<OverlayScrollBar> scroll_bar_;
        std::unique_ptr<ViewHolderRecycler> recycler_;

        friend class GridListLayouter;
        friend class LinearListLayouter;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_VIEW_H_