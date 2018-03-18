#include "view_holder_recycler.h"

#include "ukive/views/layout/view_group.h"
#include "ukive/utils/stl_utils.h"


namespace ukive {

    ViewHolderRecycler::ViewHolderRecycler(ViewGroup* parent)
        :parent_(parent) {}

    void ViewHolderRecycler::AddToParent(ListAdapter::ViewHolder* holder) {
        holder->recycled = false;
        visible_holders_.push_back(holder);
        parent_->addView(holder->item_view);
    }

    void ViewHolderRecycler::AddToParent(ListAdapter::ViewHolder* holder, size_t pos) {
        size_t index = 0;
        for (auto it = visible_holders_.begin();
            it != visible_holders_.end(); ++it) {
            if (index == pos) {
                holder->recycled = false;
                visible_holders_.insert(it, holder);
                parent_->addView(pos, holder->item_view);
                return;
            }

            ++index;
        }
    }

    void ViewHolderRecycler::AddToRecycler(ListAdapter::ViewHolder* holder) {
        holder->recycled = true;
        recycled_holders_[holder->item_id].push_back(holder);
    }

    void ViewHolderRecycler::RecycleFromParent(View* item_view) {
        for (auto it = visible_holders_.begin();
            it != visible_holders_.end(); ++it) {
            auto holder = *it;
            if (holder->item_view == item_view) {
                holder->recycled = true;
                recycled_holders_[holder->item_id].push_back(holder);
                visible_holders_.erase(it);
                parent_->removeView(item_view, false);
                return;
            }
        }
    }

    void ViewHolderRecycler::RecycleFromParent(size_t start_pos) {
        size_t index = 0;
        for (auto it = visible_holders_.begin();
            it != visible_holders_.end();) {
            if (index >= start_pos) {
                auto holder = *it;
                holder->recycled = true;
                recycled_holders_[holder->item_id].push_back(holder);
                it = visible_holders_.erase(it);
                parent_->removeView(holder->item_view, false);
            } else {
                ++it;
            }

            ++index;
        }
    }

    void ViewHolderRecycler::RecycleFromParent(size_t start_pos, size_t length) {
        if (length == 0) {
            return;
        }

        size_t index = 0;
        size_t length_index = 0;
        for (auto it = visible_holders_.begin();
            it != visible_holders_.end();) {
            if (index >= start_pos) {
                ListAdapter::ViewHolder *holder = *it;
                holder->recycled = true;
                recycled_holders_[holder->item_id].push_back(holder);
                it = visible_holders_.erase(it);
                parent_->removeView(holder->item_view, false);

                ++length_index;
                if (length_index == length) {
                    break;
                }
            } else {
                ++it;
            }

            ++index;
        }
    }

    ListAdapter::ViewHolder* ViewHolderRecycler::Reuse(int item_id) {
        if (recycled_holders_[item_id].empty()) {
            return nullptr;
        }

        auto holder = recycled_holders_[item_id].back();
        recycled_holders_[item_id].pop_back();

        AddToParent(holder);

        return holder;
    }

    ListAdapter::ViewHolder* ViewHolderRecycler::Reuse(int item_id, size_t pos) {
        if (recycled_holders_[item_id].empty()) {
            return nullptr;
        }

        auto holder = recycled_holders_[item_id].back();
        recycled_holders_[item_id].pop_back();

        AddToParent(holder, pos);

        return holder;
    }

    ListAdapter::ViewHolder* ViewHolderRecycler::GetVisible(size_t pos) {
        size_t index = 0;
        for (auto it = visible_holders_.begin();
            it != visible_holders_.end(); ++it) {
            if (index == pos) {
                return *it;
            }
            ++index;
        }

        return nullptr;
    }

    size_t ViewHolderRecycler::GetVisibleCount() {
        return visible_holders_.size();
    }

    size_t ViewHolderRecycler::GetRecycledCount(int item_id) {
        return recycled_holders_[item_id].size();
    }

    void ViewHolderRecycler::ClearAll() {
        STLDeleteElements(&visible_holders_);

        for (auto it = recycled_holders_.begin();
            it != recycled_holders_.end(); ++it) {
            STLDeleteElements(&it->second);
        }

        recycled_holders_.clear();
    }

}