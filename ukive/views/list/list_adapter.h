#ifndef UKIVE_VIEWS_LIST_LIST_ADAPTER_H_
#define UKIVE_VIEWS_LIST_LIST_ADAPTER_H_

#include "ukive/views/view.h"


namespace ukive {

    class View;
    class ViewGroup;

    class ListDataSetChangedListener
    {
    public:
        virtual ~ListDataSetChangedListener() = default;

        virtual void OnDataSetChanged() = 0;
        virtual void OnItemRangeInserted(size_t start_position, size_t length) { OnDataSetChanged(); }
        virtual void OnItemRangeChanged(size_t start_position, size_t length) { OnDataSetChanged(); }
        virtual void OnItemRangeRemoved(size_t start_position, size_t length) { OnDataSetChanged(); }
    };

    class ListAdapter
    {
    public:
        class ViewHolder
        {
        public:
            View* item_view;
            int item_id;
            int adapter_position;
            bool recycled;

            ViewHolder(View* v);
            virtual ~ViewHolder();
        };

        ListAdapter();
        virtual ~ListAdapter() = default;

        void setListener(ListDataSetChangedListener *listener) { listener_ = listener; }

        void notifyDataChanged();
        void notifyItemChanged(int position);
        void notifyItemInserted(int position);
        void notifyItemRemoved(int position);
        void notifyItemRangeChanged(int start_position, int count);
        void notifyItemRangeInserted(int start_position, int count);
        void notifyItemRangeRemoved(int start_position, int count);

        virtual ViewHolder *onCreateViewHolder(ViewGroup *parent, int position) = 0;
        virtual void onBindViewHolder(ViewHolder *holder, int position) = 0;
        virtual int getItemId(int position) { return 0; }
        virtual size_t getItemCount() = 0;

    private:
        ListDataSetChangedListener *listener_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_ADAPTER_H_