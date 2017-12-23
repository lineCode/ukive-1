#include "list_adapter.h"


namespace ukive {

    void ListAdapter::notifyDataChanged() {
        if (listener_) {
            listener_->OnDataSetChanged();
        }
    }

    void ListAdapter::notifyItemChanged(int position) {
        if (listener_) {
            listener_->OnItemRangeChanged(position, 1);
        }
    }

    void ListAdapter::notifyItemInserted(int position) {
        if (listener_) {
            listener_->OnItemRangeInserted(position, 1);
        }
    }

    void ListAdapter::notifyItemRemoved(int position) {
        if (listener_) {
            listener_->OnItemRangeRemoved(position, 1);
        }
    }

    void ListAdapter::notifyItemRangeChanged(int start_position, int count) {
        if (listener_) {
            listener_->OnItemRangeChanged(start_position, count);
        }
    }

    void ListAdapter::notifyItemRangeInserted(int start_position, int count) {
        if (listener_) {
            listener_->OnItemRangeInserted(start_position, count);
        }
    }

    void ListAdapter::notifyItemRangeRemoved(int start_position, int count) {
        if (listener_) {
            listener_->OnItemRangeRemoved(start_position, count);
        }
    }

}