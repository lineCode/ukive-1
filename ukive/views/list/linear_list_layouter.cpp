#include "ukive/views/list/linear_list_layouter.h"

#include <algorithm>

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/view_holder_recycler.h"


namespace ukive {

    LinearListLayouter::LinearListLayouter()
        : cur_position_(0),
          cur_offset_in_position_(0) {
    }

    int LinearListLayouter::onLayoutAtPosition(
        ListView* parent, ListAdapter* adapter, bool cur)
    {
        int index = 0;
        int total_height = 0;
        int item_count = adapter->getItemCount();
        auto bounds = parent->getContentBounds();

        int pos = cur ? cur_position_ : 0;
        int offset = cur ? cur_offset_in_position_ : 0;

        column_.setVertical(bounds.top, bounds.bottom);

        for (int i = pos; i < item_count; ++i, ++index) {
            if (total_height >= bounds.height() + offset) {
                break;
            }

            auto holder = column_.findAndInsertHolder(index, adapter->getItemId(i));
            if (!holder) {
                holder = parent->makeNewBindViewHolder(i, index);
                column_.addHolder(holder, index);
            } else {
                holder->adapter_position = i;
                adapter->onBindViewHolder(holder, i);
            }

            int height = parent->measureViewHolder(holder, bounds.width());
            parent->layoutViewHolder(
                holder,
                bounds.left,  bounds.top + total_height - offset,
                bounds.width(), height);
            total_height += height;
        }

        for (int i = index; i < column_.getHolderCount(); ++i) {
            parent->recycleViewHolder(column_.getHolder(i));
        }
        column_.removeHolders(index);

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

    int LinearListLayouter::onScrollToPosition(
        ListView* parent, ListAdapter* adapter, int pos, int offset, bool cur)
    {
        int index = 0;
        int total_height = 0;
        int item_count = adapter->getItemCount();
        auto bounds = parent->getContentBounds();

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

        for (int i = pos; i < item_count; ++i, ++index) {
            auto holder = column_.findAndInsertHolder(index, adapter->getItemId(i));
            if (!holder || holder->item_id != adapter->getItemId(i)) {
                holder = parent->makeNewBindViewHolder(i, index);
                column_.addHolder(holder, index);
            } else {
                holder->adapter_position = i;
                adapter->onBindViewHolder(holder, i);
            }

            int height = parent->measureViewHolder(holder, bounds.width());
            parent->layoutViewHolder(
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
            parent->recycleViewHolder(column_.getHolder(i));
        }
        column_.removeHolders(index);

        if (!full_child_reached && item_count > 0 && diff > 0) {
            return diff;
        }

        return 0;
    }

    int LinearListLayouter::onSmoothScrollToPosition(
        ListView* parent, ListAdapter* adapter, int pos, int offset)
    {
        auto item_count = adapter->getItemCount();
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
        auto bounds = parent->getContentBounds();

        int i = start_pos;
        int index = 0;

        int total_height = 0;
        bool full_child_reached = false;

        for (; (front ? (i <= terminate_pos) : (i >= terminate_pos)); (front ? ++i : --i), ++index) {
            auto holder = column_.findAndInsertHolder(index, adapter->getItemId(i));
            if (!holder || holder->item_id != adapter->getItemId(i)) {
                holder = parent->makeNewBindViewHolder(i, index);
                column_.addHolder(holder, index);
            } else {
                holder->adapter_position = i;
                adapter->onBindViewHolder(holder, i);
            }

            int height = parent->measureViewHolder(holder, bounds.width());
            parent->layoutViewHolder(
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

        return -total_height;
    }

    int LinearListLayouter::onFillTopChildren(ListView* parent, ListAdapter* adapter, int dy) {
        auto top_holder = column_.getFront();
        if (!top_holder) {
            return 0;
        }

        auto bounds = parent->getContentBounds();
        auto cur_adapter_position = top_holder->adapter_position;

        while (cur_adapter_position > 0 && !column_.isTopFilled(dy)) {
            --cur_adapter_position;

            auto new_holder = parent->makeNewBindViewHolder(cur_adapter_position, 0);
            int height = parent->measureViewHolder(new_holder, bounds.width());
            parent->layoutViewHolder(
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
                parent->recycleViewHolder(column_.getHolder(i));
            }
            column_.removeHolders(index);
        }

        return dy;
    }

    int LinearListLayouter::onFillBottomChildren(ListView* parent, ListAdapter* adapter, int dy) {
        auto bottom_holder = column_.getRear();
        if (!bottom_holder) {
            return 0;
        }

        auto bounds = parent->getContentBounds();
        auto cur_adapter_position = bottom_holder->adapter_position;

        while (cur_adapter_position + 1 < adapter->getItemCount() && !column_.isBottomFilled(dy)) {
            ++cur_adapter_position;

            auto new_holder = parent->makeNewBindViewHolder(cur_adapter_position, parent->getChildCount());
            int height = parent->measureViewHolder(new_holder, bounds.width());
            parent->layoutViewHolder(
                new_holder,
                bounds.left, column_.getHoldersBottom(),
                bounds.width(), height);
            column_.addHolder(new_holder);
        }

        dy = column_.getFinalScroll(dy);

        int index = column_.getIndexOfFirstVisible(dy);
        if (index != -1) {
            for (int i = 0; i < index; ++i) {
                parent->recycleViewHolder(column_.getHolder(i));
            }
            column_.removeHolders(0, index - 0);
        }

        return dy;
    }

    int LinearListLayouter::onFillLeftChildren(ListView* parent, ListAdapter* adapter, int dx) {
        return 0;
    }

    int LinearListLayouter::onFillRightChildren(ListView* parent, ListAdapter* adapter, int dx) {
        return 0;
    }

    void LinearListLayouter::onClear() {
        column_.clear();
        cur_position_ = 0;
        cur_offset_in_position_ = 0;
    }

    void LinearListLayouter::recordCurPositionAndOffset(ListView* parent) {
        auto holder = column_.getFirstVisible();
        if (holder) {
            cur_position_ = holder->adapter_position;
            cur_offset_in_position_ = parent->getContentBounds().top - holder->getMgdTop();
        } else {
            cur_position_ = 0;
            cur_offset_in_position_ = 0;
        }
    }

    std::pair<int, int> LinearListLayouter::computeTotalHeight(ListView* parent, ListAdapter* adapter) {
        auto count = adapter->getItemCount();
        if (count == 0) {
            return { 0, 0 };
        }

        auto front_holder = column_.getFront();
        if (!front_holder) {
            return { 0, 0 };
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

        return { prev_total_height, next_total_height };
    }

}