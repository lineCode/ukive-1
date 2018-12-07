#ifndef UKIVE_VIEWS_LIST_LIST_ADAPTER_H_
#define UKIVE_VIEWS_LIST_LIST_ADAPTER_H_

#include "ukive/views/view.h"


namespace ukive {

    class View;
    class ViewGroup;

    class ListDataSetChangedListener {
    public:
        virtual ~ListDataSetChangedListener() = default;

        virtual void onDataSetChanged() = 0;
        virtual void onItemRangeInserted(int start_position, int length) { onDataSetChanged(); }
        virtual void onItemRangeChanged(int start_position, int length) { onDataSetChanged(); }
        virtual void onItemRangeRemoved(int start_position, int length) { onDataSetChanged(); }
    };

    class ListAdapter {
    public:
        class ViewHolder {
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

        void setListener(ListDataSetChangedListener* listener) { listener_ = listener; }

        void notifyDataChanged();
        void notifyItemChanged(int position);
        void notifyItemInserted(int position);
        void notifyItemRemoved(int position);
        void notifyItemRangeChanged(int start_position, int count);
        void notifyItemRangeInserted(int start_position, int count);
        void notifyItemRangeRemoved(int start_position, int count);

        virtual ViewHolder* onCreateViewHolder(ViewGroup* parent, int position) = 0;
        virtual void onBindViewHolder(ViewHolder* holder, int position) = 0;
        virtual int getItemId(int position) { return 0; }
        virtual int getItemCount() = 0;

    private:
        ListDataSetChangedListener* listener_;
    };

}

#endif  // UKIVE_VIEWS_LIST_LIST_ADAPTER_H_