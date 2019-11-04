#include "ukive/views/list/grid_list_layouter.h"

#include <algorithm>

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/view_holder_recycler.h"
#include "ukive/log.h"

namespace ukive {

    GridListLayouter::GridListLayouter()
        : col_count_(4),
          columns_(col_count_),
          cur_row_(0),
          cur_offset_in_row_(0) {
    }

    int GridListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        int pos = cur ? cur_row_ * col_count_ : 0;
        int offset = cur ? cur_offset_in_row_ : 0;

        if (pos % col_count_) {
            pos -= pos % col_count_;
        }

        auto bounds = parent_->getContentBounds();
        int item_count = adapter_->getItemCount();

        int index = 0;
        std::vector<int> indices(col_count_, 0);
        std::vector<int> heights(col_count_, 0);
        columns_.setVertical(bounds.top, bounds.bottom);
        columns_.setHorizontal(bounds.left, bounds.right);

        for (int i = pos; i < item_count; ++i) {
            int row = i / col_count_;
            int col = i % col_count_;
            if (heights[col] >= bounds.height() + offset) {
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

            int height = parent_->measureViewHolder(holder, child_width);
            parent_->layoutViewHolder(
                holder,
                columns_[col].getLeft(), bounds.top + heights[col] - offset,
                child_width, height);
            heights[col] += height;

            ++index;
            ++indices[col];

            bool is_filled = true;
            for (const auto h : heights) {
                if (h < bounds.height() + offset) {
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

    int GridListLayouter::onScrollToPosition(int pos, int offset, bool cur) {
        return 0;
    }

    int GridListLayouter::onSmoothScrollToPosition(int pos, int offset) {
        return 0;
    }

    int GridListLayouter::onFillTopChildren(int dy) {
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

        return dy;
    }

    int GridListLayouter::onFillBottomChildren(int dy) {
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
            auto new_holder = parent_->makeNewBindViewHolder(cur_adapter_position, cur_index);

            int height = parent_->measureViewHolder(new_holder, child_width);
            parent_->layoutViewHolder(
                new_holder,
                columns_[col].getLeft(), columns_[col].getHoldersBottom(),
                child_width, height);
            columns_[col].addHolder(new_holder);
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

        return dy;
    }

    int GridListLayouter::onFillLeftChildren(int dx) {
        return 0;
    }

    int GridListLayouter::onFillRightChildren(int dx) {
        return 0;
    }

    void GridListLayouter::onClear() {
        if (!isAvailable()) {
            return;
        }

        columns_.clear();
    }

    void GridListLayouter::computeTotalHeight(int* prev, int* next) {
        if (!isAvailable()) {
            *prev = 0;
            *next = 0;
            return;
        }

        auto item_count = adapter_->getItemCount();
        if (item_count == 0) {
            *prev = 0;
            *next = 0;
            return;
        }

        auto front_holder = columns_[0].getFront();
        if (!front_holder) {
            *prev = 0;
            *next = 0;
            return;
        }

        int prev_total_height = cur_offset_in_row_;
        bool cannot_determine_height = false;

        int child_height = front_holder->getMgdHeight();

        for (int i = 0; i < cur_row_; ++i) {
            prev_total_height += child_height;
        }

        int row_count = item_count / col_count_;
        if (item_count % col_count_) ++item_count;
        int next_total_height = -cur_offset_in_row_;
        for (int i = cur_row_; i < row_count; ++i) {
            next_total_height += child_height;
        }

        *prev = prev_total_height;
        *next = next_total_height;
    }

    ListAdapter::ViewHolder* GridListLayouter::findViewHolderFromView(View* v) {
        if (!isAvailable()) {
            return nullptr;
        }
        return columns_.findHolderFromView(v);
    }

    void GridListLayouter::recordCurPositionAndOffset() {
        if (!isAvailable()) {
            return;
        }

        auto holder = columns_[0].getFirstVisible();
        if (holder) {
            cur_row_ = holder->adapter_position / col_count_;
            cur_offset_in_row_ = parent_->getContentBounds().top - holder->getMgdTop();
        } else {
            cur_row_ = 0;
            cur_offset_in_row_ = 0;
        }
    }

    bool GridListLayouter::canScroll(Direction dir) const {
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

    bool GridListLayouter::canScrollToTop() const {
        int item_count = adapter_->getItemCount();
        return columns_.isAllAtTop() && columns_.isAllAtCeil(item_count);
    }

    bool GridListLayouter::canScrollToBottom() const {
        int item_count = adapter_->getItemCount();
        return columns_.isAllAtBottom() && columns_.isAllAtFloor(item_count);
    }

    bool GridListLayouter::canScrollToLeft() const {
        return false;
    }

    bool GridListLayouter::canScrollToRight() const {
        return false;
    }

}
