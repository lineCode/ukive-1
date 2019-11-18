#include "ukive/views/list/linear_list_layouter.h"

#include <algorithm>

#include "ukive/log.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/view_holder_recycler.h"


namespace ukive {

    LinearListLayouter::LinearListLayouter()
        : cur_position_(0),
          cur_offset_in_position_(0) {
    }

    void LinearListLayouter::onMeasureAtPosition(bool cur, int width, int height) {
        if (!isAvailable()) {
            return;
        }

        parent_->freezeLayout();

        int index = 0;
        int total_height = 0;
        int item_count = adapter_->getItemCount();

        int pos = cur ? cur_position_ : 0;
        int offset = cur ? cur_offset_in_position_ : 0;

        for (int i = pos; i < item_count; ++i, ++index) {
            if (total_height >= height + offset) {
                break;
            }

            auto holder = column_.findAndInsertHolder(index, adapter_->getItemId(i));
            if (!holder) {
                holder = parent_->makeNewBindViewHolder(i, index);
                column_.addHolder(holder, index);
            } else {
                holder->adapter_position = i;
                adapter_->onBindViewHolder(holder, i);
            }

            int cur_height = parent_->measureViewHolder(holder, width);
            total_height += cur_height;
        }

        for (int i = index; i < column_.getHolderCount(); ++i) {
            parent_->recycleViewHolder(column_.getHolder(i));
        }
        column_.removeHolders(index);

        parent_->unfreezeLayout();
    }

    int LinearListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        parent_->freezeLayout();

        int index = 0;
        int total_height = 0;
        int item_count = adapter_->getItemCount();
        auto bounds = parent_->getContentBounds();

        int pos = cur ? cur_position_ : 0;
        int offset = cur ? cur_offset_in_position_ : 0;

        column_.setVertical(bounds.top, bounds.bottom);

        for (int i = pos; i < item_count; ++i, ++index) {
            if (total_height >= bounds.height() + offset) {
                break;
            }

            auto holder = column_.getHolder(index);
            DCHECK(holder);

            int height = holder->item_view->getMeasuredHeight() + holder->getVertMargins();
            parent_->layoutViewHolder(
                holder,
                bounds.left,  bounds.top + total_height - offset,
                bounds.width(), height);
            total_height += height;
        }

        parent_->unfreezeLayout();

        // 防止列表大小变化时项目超出滑动范围。
        auto last_holder = column_.getLastVisible();
        auto first_holder = column_.getFirstVisible();
        if (last_holder && first_holder && last_holder->adapter_position + 1 == item_count) {
            bool can_scroll = (first_holder->adapter_position > 0 ||
                bounds.top - first_holder->getMgdTop() > 0);
            if (can_scroll) {
                return bounds.bottom - last_holder->getMgdBottom();
            }
        }

        return 0;
    }

    int LinearListLayouter::onScrollToPosition(int pos, int offset, bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        int index = 0;
        int total_height = 0;
        int item_count = adapter_->getItemCount();
        auto bounds = parent_->getContentBounds();

        pos = cur ? cur_position_ : pos;
        offset = cur ? cur_offset_in_position_ : offset;

        if (pos + 1 > item_count) {
            if (item_count > 0) {
                pos = item_count - 1;
            } else {
                pos = 0;
            }
            offset = 0;
        }

        int diff = 0;
        bool full_child_reached = false;

        parent_->freezeLayout();

        for (int i = pos; i < item_count; ++i, ++index) {
            auto holder = column_.findAndInsertHolder(index, adapter_->getItemId(i));
            if (!holder || holder->item_id != adapter_->getItemId(i)) {
                holder = parent_->makeNewBindViewHolder(i, index);
                column_.addHolder(holder, index);
            } else {
                holder->adapter_position = i;
                adapter_->onBindViewHolder(holder, i);
            }

            int height = parent_->measureViewHolder(holder, bounds.width());
            parent_->layoutViewHolder(
                holder,
                bounds.left, bounds.top + total_height - offset,
                bounds.width(), height);
            total_height += height;

            diff = bounds.bottom - holder->getMgdBottom();
            if (total_height >= bounds.height() + offset) {
                full_child_reached = true;
                ++index;
                break;
            }
        }

        for (int i = index; i < column_.getHolderCount(); ++i) {
            parent_->recycleViewHolder(column_.getHolder(i));
        }
        column_.removeHolders(index);

        parent_->unfreezeLayout();

        if (!full_child_reached && item_count > 0 && diff > 0) {
            return diff;
        }

        return 0;
    }

    int LinearListLayouter::onSmoothScrollToPosition(int pos, int offset) {
        if (!isAvailable()) {
            return 0;
        }

        auto item_count = adapter_->getItemCount();
        if (item_count == 0) {
            return 0;
        }
        if (pos + 1 > item_count) {
            pos = item_count - 1;
            offset = 0;
        }

        int start_pos = cur_position_;
        int start_pos_offset = cur_offset_in_position_;
        int terminate_pos = pos;
        int terminate_pos_offset = offset;
        bool front = (start_pos <= terminate_pos);
        auto bounds = parent_->getContentBounds();

        int i = start_pos;
        int index = 0;

        int total_height = 0;
        bool full_child_reached = false;

        parent_->freezeLayout();

        for (; (front ? (i <= terminate_pos) : (i >= terminate_pos)); (front ? ++i : --i), ++index) {
            auto holder = column_.findAndInsertHolder(index, adapter_->getItemId(i));
            if (!holder || holder->item_id != adapter_->getItemId(i)) {
                holder = parent_->makeNewBindViewHolder(i, index);
                column_.addHolder(holder, index);
            } else {
                holder->adapter_position = i;
                adapter_->onBindViewHolder(holder, i);
            }

            int height = parent_->measureViewHolder(holder, bounds.width());
            parent_->layoutViewHolder(
                holder,
                bounds.left, bounds.top + total_height + (front ? -offset : offset),
                bounds.width(), height);

            if (front) {
                if (i == terminate_pos) {
                    height = terminate_pos_offset;
                }
                if (i == start_pos) {
                    height -= start_pos_offset;
                }
                if (i != start_pos && i != terminate_pos) {
                    height = front ? height : -height;
                }
            } else {
                if (i == start_pos) {
                    height = start_pos_offset;
                }
                if (i == terminate_pos) {
                    height -= terminate_pos_offset;
                }
                if (i != start_pos && i != terminate_pos) {
                    height = front ? height : -height;
                }
            }

            total_height += height;
        }

        parent_->unfreezeLayout();

        return -total_height;
    }

    int LinearListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto top_holder = column_.getFront();
        if (!top_holder) {
            return 0;
        }

        parent_->freezeLayout();

        auto bounds = parent_->getContentBounds();
        auto cur_adapter_position = top_holder->adapter_position;

        while (cur_adapter_position > 0 && !column_.isTopFilled(dy)) {
            --cur_adapter_position;

            auto new_holder = parent_->makeNewBindViewHolder(cur_adapter_position, 0);
            int height = parent_->measureViewHolder(new_holder, bounds.width());
            parent_->layoutViewHolder(
                new_holder,
                bounds.left, column_.getHoldersTop() - height,
                bounds.width(), height);
            column_.addHolder(new_holder, 0);
        }

        dy = column_.getFinalScroll(dy);

        int index = column_.getIndexOfLastVisible(dy);
        if (index != -1) {
            ++index;
            for (int i = index; i < column_.getHolderCount(); ++i) {
                parent_->recycleViewHolder(column_.getHolder(i));
            }
            column_.removeHolders(index);
        }

        parent_->unfreezeLayout();
        return dy;
    }

    int LinearListLayouter::onFillBottomChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto bottom_holder = column_.getRear();
        if (!bottom_holder) {
            return 0;
        }

        parent_->freezeLayout();

        auto bounds = parent_->getContentBounds();
        auto cur_adapter_position = bottom_holder->adapter_position;

        while (cur_adapter_position + 1 < adapter_->getItemCount() && !column_.isBottomFilled(dy)) {
            ++cur_adapter_position;

            auto new_holder = parent_->makeNewBindViewHolder(cur_adapter_position, parent_->getChildCount());
            int height = parent_->measureViewHolder(new_holder, bounds.width());
            parent_->layoutViewHolder(
                new_holder,
                bounds.left, column_.getHoldersBottom(),
                bounds.width(), height);
            column_.addHolder(new_holder);
        }

        dy = column_.getFinalScroll(dy);

        int index = column_.getIndexOfFirstVisible(dy);
        if (index != -1) {
            for (int i = 0; i < index; ++i) {
                parent_->recycleViewHolder(column_.getHolder(i));
            }
            column_.removeHolders(0, index - 0);
        }

        parent_->unfreezeLayout();
        return dy;
    }

    int LinearListLayouter::onFillLeftChildren(int dx) {
        return 0;
    }

    int LinearListLayouter::onFillRightChildren(int dx) {
        return 0;
    }

    void LinearListLayouter::onClear() {
        if (!isAvailable()) {
            return;
        }

        column_.clear();
        cur_position_ = 0;
        cur_offset_in_position_ = 0;
    }

    void LinearListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }

        auto holder = column_.getFirstVisible();
        if (holder) {
            cur_position_ = holder->adapter_position;
            cur_offset_in_position_ = parent_->getContentBounds().top - holder->getMgdTop();
        } else {
            cur_position_ = 0;
            cur_offset_in_position_ = 0;
        }
    }

    void LinearListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *prev = 0;
            *next = 0;
            return;
        }

        auto count = adapter_->getItemCount();
        if (count == 0) {
            *prev = 0;
            *next = 0;
            return;
        }

        auto front_holder = column_.getFront();
        if (!front_holder) {
            *prev = 0;
            *next = 0;
            return;
        }

        int prev_total_height = cur_offset_in_position_;
        int child_height = front_holder->getMgdHeight();

        for (int i = 0; i < cur_position_; ++i) {
            prev_total_height += child_height;
        }

        int next_total_height = -cur_offset_in_position_;
        for (int i = cur_position_; i < count; ++i) {
            next_total_height += child_height;
        }

        *prev = prev_total_height;
        *next = next_total_height;
    }

    ListAdapter::ViewHolder* LinearListLayouter::findViewHolderFromView(View* v) {
        if (!isAvailable()) {
            return nullptr;
        }
        return column_.findHolderFromView(v);
    }

    bool LinearListLayouter::canScroll(Direction dir) const {
        if (!isAvailable()) {
            return false;
        }

        bool result = false;
        if (dir & TOP) {
            result |= canScrollToTop();
        }
        if (dir & BOTTOM) {
            result |= canScrollToBottom();
        }
        if (dir & LEFT) {
            result |= canScrollToLeft();
        }
        if (dir & RIGHT) {
            result |= canScrollToRight();
        }
        return result;
    }

    bool LinearListLayouter::canScrollToTop() const {
        auto top_holder = column_.getFront();
        if (!top_holder) {
            return false;
        }
        if (top_holder->adapter_position == 0 && column_.atTop()) {
            return false;
        }
        return true;
    }

    bool LinearListLayouter::canScrollToBottom() const {
        auto bottom_holder = column_.getRear();
        if (!bottom_holder) {
            return false;
        }
        if (bottom_holder->adapter_position + 1 == adapter_->getItemCount() && column_.atBottom()) {
            return false;
        }
        return true;
    }

    bool LinearListLayouter::canScrollToLeft() const {
        return false;
    }

    bool LinearListLayouter::canScrollToRight() const {
        return false;
    }

}