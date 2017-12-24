#ifndef UKIVE_VIEWS_LIST_LIST_VIEW_H_
#define UKIVE_VIEWS_LIST_LIST_VIEW_H_

#include <list>
#include <memory>

#include "ukive/views/layout/view_group.h"
#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class ListView;

    class ViewRecycler
    {
    public:
        ViewRecycler(ViewGroup *parent)
            :parent_(parent) {}

        void AddViewHolder(ListAdapter::ViewHolder *holder);
        void AddViewHolder(ListAdapter::ViewHolder *holder, size_t pos);
        void AddRecycledViewHolder(ListAdapter::ViewHolder *holder);
        void RecycleViewHolder(View *item_view);
        void RecycleViewHolders(size_t start_pos);
        void RecycleViewHolders(size_t start_pos, size_t length);
        ListAdapter::ViewHolder* ReuseViewHolder();
        ListAdapter::ViewHolder* ReuseViewHolder(size_t pos);
        ListAdapter::ViewHolder* GetVisibleViewHolder(size_t pos);

        size_t Size();
        void ClearAll();

    private:
        ViewGroup *parent_;

        std::list<ListAdapter::ViewHolder*> visible_view_holder_;
        std::vector<ListAdapter::ViewHolder*> recycled_view_holder_;
    };

    class ListOperator
    {
    public:
        enum OpType {
            INSERT,
            CHANGE,
            REMOVE
        };

        struct Operation {
            OpType op;
            size_t start_position;
            size_t length;
        };

        ListOperator(ListView *view) {}

        void AddOp(OpType op, size_t start_position, size_t length) {}

    private:
        std::vector<Operation> op_list_;
    };


    class ListView : public ViewGroup, public ListDataSetChangedListener
    {
    public:
        ListView(Window *w);

        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
        bool onInputEvent(InputEvent *e) override;

        void setAdapter(ListAdapter *adapter);
        void ScrollToPosition(size_t position, int offset, bool smooth);

    private:
        enum OpType {
            INSERT,
            CHANGE,
            REMOVE
        };

        struct Operation {
            OpType op;
            size_t start_position;
            size_t length;
        };

        void initListView();

        int DetermineVerticalScroll(int dy);

        void OffsetChildViewTopAndBottom(int dy);

        ListAdapter::ViewHolder* GetFirstVisibleViewHolder();
        ListAdapter::ViewHolder* GetLastVisibleViewHolder();

        void RecycleTopViews(int offset);
        void RecycleBottomViews(int offset);

        void RecordCurPositionAndOffset();

        int FillTopChildViews(int dy);
        int FillBottomChildViews(int dy);

        void LocateToPosition(size_t position, int offset = 0);
        void ScrollToPosition(size_t position, int offset = 0);
        void SmoothScrollToPosition(size_t position, int offset = 0);

        // Overriden from ListScrollDelegate:
        //void OnScroll(float dx, float dy) OVERRIDE;

        // Overriden from ListDataSetListener:
        void OnDataSetChanged() override;
        void OnItemRangeInserted(size_t start_position, size_t length) override;
        void OnItemRangeChanged(size_t start_position, size_t length) override;
        void OnItemRangeRemoved(size_t start_position, size_t length) override;

        void AddOp(OpType op, size_t start_position, size_t length);
        void ProcessOp();

        size_t cur_position_;
        int cur_offset_in_position_;
        bool initial_layouted_;

        std::unique_ptr<ListAdapter> list_adapter_;

        ViewRecycler *view_recycler_;

        std::vector<Operation> op_list_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_VIEW_H_