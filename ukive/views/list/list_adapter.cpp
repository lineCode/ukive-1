#include "list_adapter.h"


namespace ukive {

    ////////////////////////////////////////////////////////////////
    // ListAdapter::ViewHolder

    ListAdapter::ViewHolder::ViewHolder(View* v)
        :item_view(v),
        item_id(-1),
        adapter_position(-1),
        recycled(false) {}

    ListAdapter::ViewHolder::~ViewHolder() {
        if (recycled) {
            delete item_view;
        }
    }


    ////////////////////////////////////////////////////////////////
    // ListAdapter

    ListAdapter::ListAdapter()
        :listener_(nullptr) {}

    void ListAdapter::notifyDataChanged() {
        if (listener_) {
            listener_->onDataSetChanged();
        }
    }

    void ListAdapter::notifyItemChanged(int position) {
        if (listener_) {
            listener_->onItemRangeChanged(position, 1);
        }
    }

    void ListAdapter::notifyItemInserted(int position) {
        if (listener_) {
            listener_->onItemRangeInserted(position, 1);
        }
    }

    void ListAdapter::notifyItemRemoved(int position) {
        if (listener_) {
            listener_->onItemRangeRemoved(position, 1);
        }
    }

    void ListAdapter::notifyItemRangeChanged(int start_position, int count) {
        if (listener_) {
            listener_->onItemRangeChanged(start_position, count);
        }
    }

    void ListAdapter::notifyItemRangeInserted(int start_position, int count) {
        if (listener_) {
            listener_->onItemRangeInserted(start_position, count);
        }
    }

    void ListAdapter::notifyItemRangeRemoved(int start_position, int count) {
        if (listener_) {
            listener_->onItemRangeRemoved(start_position, count);
        }
    }

}