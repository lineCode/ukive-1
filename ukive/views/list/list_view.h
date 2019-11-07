#ifndef UKIVE_VIEWS_LIST_LIST_VIEW_H_
#define UKIVE_VIEWS_LIST_LIST_VIEW_H_

#include <memory>

#include "ukive/animation/scroller.h"
#include "ukive/event/velocity_calculator.h"
#include "ukive/views/layout/view_group.h"
#include "ukive/views/list/list_adapter.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class ListLayouter;
    class OverlayScrollBar;
    class ViewHolderRecycler;


    class ListItemRecycledListener {
    public:
        virtual ~ListItemRecycledListener() = default;

        virtual void onChildRecycled(ListAdapter::ViewHolder* holder) = 0;
    };

    class ListItemSelectedListener {
    public:
        virtual ~ListItemSelectedListener() = default;

        virtual void onItemSelected(ListAdapter::ViewHolder* holder) = 0;
    };


    class ListView : public ViewGroup, public ListDataSetChangedListener, public OnClickListener {
    public:
        explicit ListView(Window* w);
        ListView(Window* w, AttrsRef attrs);

        void setAdapter(ListAdapter* adapter);
        void setLayouter(ListLayouter* layouter);
        void scrollToPosition(int pos, int offset, bool smooth);

        void setItemSelectedListener(ListItemSelectedListener* l);
        void setChildRecycledListener(ListItemRecycledListener* l);

    protected:
        // ViewGroup
        void onMeasure(
            int width, int height,
            int width_mode, int height_mode) override;
        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override;
        bool onInterceptInputEvent(InputEvent* e) override;
        bool onInputEvent(InputEvent* e) override;
        void onDraw(Canvas* canvas) override;
        void onDrawOverChildren(Canvas* canvas) override;
        void onComputeScroll() override;

        // View
        void requestLayout() override;

        // OnClickListener
        void onClick(View* v) override;

    private:
        using ViewHolder = ListAdapter::ViewHolder;

        struct SizeCache {
            bool available = false;
            int width = 0;
            int height = 0;
        };

        void freezeLayout();
        void unfreezeLayout();
        void resetSizeCache();

        bool processVerticalScroll(int dy);
        int determineVerticalScroll(int dy);
        void offsetChildViewTopAndBottom(int dy);

        bool getCachedSize(int pos, int* width, int* height);
        ViewHolder* makeNewBindViewHolder(int adapter_pos, int view_index);
        void recycleViewHolder(ViewHolder* holder);

        int findViewIndexFromStart(ViewHolder* holder) const;
        int findViewIndexFromEnd(ViewHolder* holder) const;

        int measureViewHolder(ViewHolder* holder, int width);
        void layoutViewHolder(ViewHolder* holder, int left, int top, int width, int height);

        void updateOverlayScrollBar();
        void recordCurPositionAndOffset();

        int fillTopChildViews(int dy);
        int fillBottomChildViews(int dy);

        void layoutAtPosition(bool cur);
        void directScrollToPosition(int pos, int offset, bool cur);
        void smoothScrollToPosition(int pos, int offset);

        void onScrollBarChanged(int dy);

        // ListDataSetListener:
        void onDataSetChanged() override;
        void onItemRangeInserted(int start_pos, int length) override;
        void onItemRangeChanged(int start_pos, int length) override;
        void onItemRangeRemoved(int start_pos, int length) override;

        bool initial_layouted_;

        bool is_mouse_down_ = false;

        int prev_touch_x_ = 0;
        int prev_touch_y_ = 0;
        int start_touch_x_ = 0;
        int start_touch_y_ = 0;
        bool is_touch_down_ = false;

        Scroller scroller_;
        VelocityCalculator velocity_calculator_;

        std::unique_ptr<ListAdapter> adapter_;
        std::unique_ptr<ListLayouter> layouter_;
        std::unique_ptr<OverlayScrollBar> scroll_bar_;
        std::unique_ptr<ViewHolderRecycler> recycler_;

        ListItemRecycledListener* recycled_listener_ = nullptr;
        ListItemSelectedListener* selected_listener_ = nullptr;

        bool is_frozen_layout_ = false;
        std::vector<SizeCache> size_cache_;

        friend class FlowListLayouter;
        friend class GridListLayouter;
        friend class LinearListLayouter;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_VIEW_H_