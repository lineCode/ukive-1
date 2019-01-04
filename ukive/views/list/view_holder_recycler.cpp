#include "view_holder_recycler.h"

#include "ukive/log.h"
#include "ukive/views/layout/view_group.h"
#include "ukive/utils/stl_utils.h"


namespace ukive {

    ViewHolderRecycler::ViewHolderRecycler(ViewGroup* parent)
        :parent_(parent) {}

    void ViewHolderRecycler::addToParent(ListAdapter::ViewHolder* holder) {
        DCHECK(holder);

        holder->recycled = false;
        parent_->addView(holder->item_view);
    }

    void ViewHolderRecycler::addToParent(ListAdapter::ViewHolder* holder, int pos) {
        DCHECK(holder && pos >= 0);

        holder->recycled = false;
        parent_->addView(pos, holder->item_view);
    }

    void ViewHolderRecycler::addToRecycler(ListAdapter::ViewHolder* holder) {
        DCHECK(holder);

        holder->recycled = true;
        recycled_holders_[holder->item_id].push_back(holder);
    }

    void ViewHolderRecycler::recycleFromParent(ListAdapter::ViewHolder* holder) {
        DCHECK(holder);

        holder->recycled = true;
        recycled_holders_[holder->item_id].push_back(holder);
        parent_->removeView(holder->item_view, false);
    }

    ListAdapter::ViewHolder* ViewHolderRecycler::reuse(int item_id) {
        if (recycled_holders_[item_id].empty()) {
            return nullptr;
        }

        auto holder = recycled_holders_[item_id].back();
        recycled_holders_[item_id].pop_back();

        addToParent(holder);

        return holder;
    }

    ListAdapter::ViewHolder* ViewHolderRecycler::reuse(int item_id, int pos) {
        DCHECK(pos >= 0);

        if (recycled_holders_[item_id].empty()) {
            return nullptr;
        }

        auto holder = recycled_holders_[item_id].back();
        recycled_holders_[item_id].pop_back();

        addToParent(holder, pos);

        return holder;
    }

    int ViewHolderRecycler::getRecycledCount(int item_id) {
        return recycled_holders_[item_id].size();
    }

    void ViewHolderRecycler::clear() {
        for (auto& pair : recycled_holders_) {
            STLDeleteElements(&pair.second);
        }

        recycled_holders_.clear();
    }

}