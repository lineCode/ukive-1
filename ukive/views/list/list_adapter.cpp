#include "list_adapter.h"

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    // ListAdapter::ViewHolder
    ListAdapter::ViewHolder::ViewHolder(View* v)
        : item_view(v),
          item_id(-1),
          adapter_position(-1),
          recycled(false) {
    }

    ListAdapter::ViewHolder::~ViewHolder() {
        if (recycled) {
            delete item_view;
        }
    }

    int ListAdapter::ViewHolder::getMgdLeft() const {
        return item_view->getLeft() -
            item_view->getLayoutParams()->left_margin - ex_margins.left;
    }

    int ListAdapter::ViewHolder::getMgdTop() const {
        return item_view->getTop() -
            item_view->getLayoutParams()->top_margin - ex_margins.top;
    }

    int ListAdapter::ViewHolder::getMgdRight() const {
        return item_view->getRight() +
            item_view->getLayoutParams()->right_margin + ex_margins.right;
    }

    int ListAdapter::ViewHolder::getMgdBottom() const {
        return item_view->getBottom() +
            item_view->getLayoutParams()->bottom_margin + ex_margins.bottom;
    }

    int ListAdapter::ViewHolder::getMgdWidth() const {
        return item_view->getWidth() +
            item_view->getLayoutParams()->left_margin +
            item_view->getLayoutParams()->right_margin +
            ex_margins.left +
            ex_margins.right;
    }

    int ListAdapter::ViewHolder::getMgdHeight() const {
        return item_view->getHeight() +
            item_view->getLayoutParams()->top_margin +
            item_view->getLayoutParams()->bottom_margin +
            ex_margins.top +
            ex_margins.bottom;
    }


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