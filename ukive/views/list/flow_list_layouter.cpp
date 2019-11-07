#include "ukive/views/list/flow_list_layouter.h"

#include <algorithm>

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/view_holder_recycler.h"
#include "ukive/log.h"

namespace ukive {

    FlowListLayouter::FlowListLayouter()
        : col_count_(4),
          columns_(col_count_)
    {
        cur_records_.resize(col_count_, {0, 0, false});
    }

    void FlowListLayouter::onMeasureAtPosition(bool cur, int width, int height) {
        if (!isAvailable()) {
            return;
        }

        int pos = calPreferredCurPos();
        int item_count = adapter_->getItemCount();

        std::vector<int> indices(col_count_, 0);
        std::vector<int> heights(col_count_, 0);
        columns_.setVertical(0, height);
        columns_.setHorizontal(0, width);

        parent_->freezeLayout();

        for (auto& r : cur_records_) {
            if (!r.is_null) {
                r.cur_offset = std::max(r.cur_offset, 0);
            }
        }

        int index = 0;
        for (int i = pos; i < item_count; ++i) {
            int row = i / col_count_;
            int col = i % col_count_;
            const auto& record = cur_records_[col];
            if (record.is_null) {
                continue;
            }
            if (record.cur_row > row) {
                continue;
            }
            if (heights[col] >= height + record.cur_offset) {
                continue;
            }

            auto holder = columns_[col].findAndInsertHolder(indices[col], adapter_->getItemId(i));
            if (!holder) {
                holder = parent_->makeNewBindViewHolder(i, index);
                columns_[col].addHolder(holder, indices[col]);
            } else {
                holder->adapter_position = i;
                adapter_->onBindViewHolder(holder, i);
            }

            int child_width = columns_[col].getWidth();

            int cur_height = parent_->measureViewHolder(holder, child_width);
            heights[col] += cur_height;

            ++index;
            ++indices[col];

            bool is_filled = true;
            for (int j = 0; j < col_count_; ++j) {
                if (heights[j] < height + cur_records_[j].cur_offset) {
                    is_filled = false;
                    break;
                }
            }
            if (is_filled) {
                break;
            }
        }

        for (int i = 0; i < col_count_; ++i) {
            for (int j = indices[i]; j < columns_[i].getHolderCount(); ++j) {
                parent_->recycleViewHolder(columns_[i].getHolder(j));
            }
            columns_[i].removeHolders(indices[i]);
        }

        parent_->unfreezeLayout();
    }

    int FlowListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        int pos = calPreferredCurPos();
        int item_count = adapter_->getItemCount();
        auto bounds = parent_->getContentBounds();

        int index = 0;
        std::vector<int> indices(col_count_, 0);
        std::vector<int> heights(col_count_, 0);
        columns_.setVertical(bounds.top, bounds.bottom);
        columns_.setHorizontal(bounds.left, bounds.right);

        parent_->freezeLayout();

        for (auto& r : cur_records_) {
            if (!r.is_null) {
                r.cur_offset = std::max(r.cur_offset, 0);
            }
        }

        for (int i = pos; i < item_count; ++i) {
            int row = i / col_count_;
            int col = i % col_count_;
            const auto& record = cur_records_[col];
            if (record.is_null) {
                continue;
            }
            if (record.cur_row > row) {
                continue;
            }
            if (heights[col] >= bounds.height() + record.cur_offset) {
                continue;
            }

            auto holder = columns_[col].getHolder(indices[col]);
            DCHECK(holder);

            int child_width = columns_[col].getWidth();

            int height = holder->item_view->getMeasuredHeight() + holder->getVertMargins();
            parent_->layoutViewHolder(
                holder,
                columns_[col].getLeft(), bounds.top + heights[col] - record.cur_offset,
                child_width, height);
            heights[col] += height;

            ++index;
            ++indices[col];

            bool is_filled = true;
            for (int j = 0; j < col_count_; ++j) {
                if (heights[j] < bounds.height() + cur_records_[j].cur_offset) {
                    is_filled = false;
                    break;
                }
            }
            if (is_filled) {
                break;
            }
        }

        parent_->unfreezeLayout();

        // 防止列表大小变化时项目超出滑动范围。
        if (columns_.isAllAtFloor(item_count)) {
            bool is_at_ceil = columns_.isAllAtCeil(item_count);
            bool can_scroll = !is_at_ceil;
            if (is_at_ceil) {
                auto topmost = columns_.getTopmost();
                if (topmost) {
                    can_scroll = (bounds.top - topmost->getMgdTop() > 0);
                }
            }
            if (can_scroll) {
                return bounds.bottom - columns_.getBottomost()->getMgdBottom();
            }
        }

        return 0;
    }

    int FlowListLayouter::onScrollToPosition(int pos, int offset, bool cur) {
        return 0;
    }

    int FlowListLayouter::onSmoothScrollToPosition(int pos, int offset) {
        return 0;
    }

    int FlowListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto top_holder = columns_.getTopStart();
        if (!top_holder) {
            return 0;
        }
        auto bounds = parent_->getContentBounds();
        auto cur_adapter_position = top_holder->adapter_position;
        int cur_index = parent_->findViewIndexFromStart(top_holder);

        parent_->freezeLayout();

        while (cur_adapter_position > 0 && !columns_.isTopFilled(dy)) {
            --cur_adapter_position;
            int row = cur_adapter_position / col_count_;
            int col = cur_adapter_position % col_count_;
            if (columns_[col].isTopFilled(dy)) {
                continue;
            }

            auto cur_holder = columns_.getHolderByPos(cur_adapter_position);
            if (cur_holder) {
                cur_index = parent_->findViewIndexFromStart(cur_holder);
                continue;
            }

            DCHECK(cur_index != -1);

            int child_width = columns_[col].getWidth();
            auto new_holder = parent_->makeNewBindViewHolder(cur_adapter_position, cur_index);

            int height = parent_->measureViewHolder(new_holder, child_width);
            parent_->layoutViewHolder(
                new_holder,
                columns_[col].getLeft(), columns_[col].getHoldersTop() - height,
                child_width, height);
            columns_[col].addHolder(new_holder, 0);
        }

        dy = columns_.getFinalScroll(dy);

        for (int i = 0; i < col_count_; ++i) {
            int index = columns_[i].getIndexOfLastVisible(dy);
            if (index != -1) {
                ++index;
                for (int j = index; j < columns_[i].getHolderCount(); ++j) {
                    parent_->recycleViewHolder(columns_[i].getHolder(j));
                }
                columns_[i].removeHolders(index);
            }
        }

        parent_->unfreezeLayout();

        return dy;
    }

    int FlowListLayouter::onFillBottomChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto bottom_holder = columns_.getBottomStart();
        if (!bottom_holder) {
            return 0;
        }
        auto bounds = parent_->getContentBounds();
        auto cur_adapter_position = bottom_holder->adapter_position;
        int cur_index = parent_->findViewIndexFromEnd(bottom_holder);

        parent_->freezeLayout();

        while (cur_adapter_position + 1 < adapter_->getItemCount() && !columns_.isBottomFilled(dy)) {
            ++cur_adapter_position;
            int row = cur_adapter_position / col_count_;
            int col = cur_adapter_position % col_count_;
            if (columns_[col].isBottomFilled(dy)) {
                continue;
            }

            auto cur_holder = columns_.getHolderByPos(cur_adapter_position);
            if (cur_holder) {
                cur_index = parent_->findViewIndexFromEnd(cur_holder);
                continue;
            }

            DCHECK(cur_index != -1);

            int child_width = columns_[col].getWidth();
            auto new_holder = parent_->makeNewBindViewHolder(cur_adapter_position, cur_index + 1);

            int height = parent_->measureViewHolder(new_holder, child_width);
            parent_->layoutViewHolder(
                new_holder,
                columns_[col].getLeft(), columns_[col].getHoldersBottom(),
                child_width, height);
            columns_[col].addHolder(new_holder);
            ++cur_index;
        }

        dy = columns_.getFinalScroll(dy);

        for (int i = 0; i < col_count_; ++i) {
            int index = columns_[i].getIndexOfFirstVisible(dy);
            if (index != -1) {
                for (int j = 0; j < index; ++j) {
                    parent_->recycleViewHolder(columns_[i].getHolder(j));
                }
                columns_[i].removeHolders(0, index - 0);
            }
        }

        parent_->unfreezeLayout();

        return dy;
    }

    int FlowListLayouter::onFillLeftChildren(int dx) {
        return 0;
    }

    int FlowListLayouter::onFillRightChildren(int dx) {
        return 0;
    }

    void FlowListLayouter::onClear() {
        if (!isAvailable()) {
            return;
        }

        columns_.clear();
    }

    void FlowListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *next = *prev = 0;
            return;
        }

        auto item_count = adapter_->getItemCount();
        if (item_count == 0) {
            *next = *prev = 0;
            return;
        }

        int max_col = 0;
        int max_col_count = 0;
        int max_col_height = 0;
        for (int i = 0; i < col_count_; ++i) {
            int avg_height = 0;
            int count = columns_[i].getHolderCount();
            for (int j = 0; j < count; ++j) {
                auto holder = columns_[i].getHolder(j);
                avg_height += holder->getMgdHeight();
            }

            if (avg_height > max_col_height) {
                max_col = i;
                max_col_height = avg_height;
                max_col_count = count;
            }
        }
        int child_height;
        if (max_col_count == 0) {
            child_height = 0;
        } else {
            child_height = max_col_height / max_col_count;
        }

        int cur_row = cur_records_[max_col].cur_row;
        int cur_offset_in_row = cur_records_[max_col].cur_offset;

        int i;
        int prev_total_height = cur_offset_in_row;
        for (i = 0; i < cur_row; ++i) {
            int width, height;
            int pos = col_count_ * i + max_col;
            if (false && parent_->getCachedSize(pos, &width, &height)) {
                prev_total_height += height;
            } else {
                prev_total_height += child_height;
            }
        }

        int row_count = item_count / col_count_;
        if (item_count % col_count_) {
            ++row_count;
        }

        int next_total_height = -cur_offset_in_row;
        for (; i < row_count; ++i) {
            int width, height;
            int pos = col_count_ * i + max_col;
            if (false && parent_->getCachedSize(pos, &width, &height)) {
                next_total_height += height;
            } else {
                next_total_height += child_height;
            }
        }

        *prev = prev_total_height;
        *next = next_total_height;
    }

    ListAdapter::ViewHolder* FlowListLayouter::findViewHolderFromView(View* v) {
        if (!isAvailable()) {
            return nullptr;
        }
        return columns_.findHolderFromView(v);
    }

    void FlowListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }
        auto bounds = parent_->getContentBounds();
        if (bounds.empty()) {
            return;
        }

        for (int i = 0; i < col_count_; ++i) {
            Record record;
            auto holder = columns_[i].getFirstVisible();
            if (!holder) {
                holder = columns_[i].getRear();
            }
            if (holder) {
                record.is_null = false;
                record.cur_row = holder->adapter_position / col_count_;
                record.cur_offset = bounds.top - holder->getMgdTop();
                //DCHECK(record.cur_offset >= 0);
            }
            cur_records_[i] = record;
        }
    }

    bool FlowListLayouter::canScroll(Direction dir) const {
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

    int FlowListLayouter::calPreferredCurPos() const {
        int index = 0;
        int pos = std::numeric_limits<int>::max();
        for (const auto& r : cur_records_) {
            if (r.is_null) {
                ++index;
                continue;
            }
            pos = std::min(pos, r.cur_row * col_count_ + index);
            if (pos == 0) {
                break;
            }
            ++index;
        }
        return pos;
    }

    bool FlowListLayouter::canScrollToTop() const {
        int item_count = adapter_->getItemCount();
        return !(columns_.isAllAtTop() && columns_.isAllAtCeil(item_count));
    }

    bool FlowListLayouter::canScrollToBottom() const {
        int item_count = adapter_->getItemCount();
        return !(columns_.isAllAtBottom() && columns_.isAllAtFloor(item_count));
    }

    bool FlowListLayouter::canScrollToLeft() const {
        return false;
    }

    bool FlowListLayouter::canScrollToRight() const {
        return false;
    }

}
