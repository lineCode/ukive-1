#include "ukive/views/list/grid_list_layouter.h"

#include <algorithm>

#include "utils/log.h"

#include "ukive/views/list/list_view.h"
#include "ukive/views/list/view_holder_recycler.h"


namespace ukive {

    GridListLayouter::GridListLayouter()
        : col_count_(4),
          columns_(col_count_)
    {
    }

    void GridListLayouter::onMeasureAtPosition(bool cur, int width, int height) {
        if (!isAvailable()) {
            return;
        }

        int cur_row = cur ? cur_row_ : 0;
        int offset = cur ? cur_offset_ : 0;
        int item_count = adapter_->getItemCount();

        columns_.setVertical(0, height);
        columns_.setHorizontal(0, width);

        parent_->freezeLayout();

        int row_index = 0;
        int view_index = 0;
        int total_height = 0;
        int cur_col = 0;
        for (int row = cur_row; row * col_count_ < item_count; ++row) {
            int col;
            int row_height = 0;
            for (col = 0; col < col_count_; ++col) {
                int pos = row * col_count_ + col;
                if (pos >= item_count) {
                    break;
                }

                auto holder = columns_[col].findAndInsertHolder(row_index, adapter_->getItemId(pos));
                if (!holder) {
                    holder = parent_->makeNewBindViewHolder(pos, view_index);
                    columns_[col].addHolder(holder, row_index);
                } else {
                    holder->adapter_position = pos;
                    adapter_->onBindViewHolder(holder, pos);
                }

                int child_width = columns_[col].getWidth();

                int cur_height = parent_->measureViewHolder(holder, child_width);
                row_height = std::max(cur_height, row_height);

                ++view_index;
            }

            if (col > 0 && col < col_count_) {
                cur_col = col;
            } else {
                cur_col = 0;
            }

            ++row_index;
            total_height += row_height;

            if (total_height >= height + offset) {
                break;
            }
        }

        for (int i = 0; i < col_count_; ++i) {
            int start = row_index;
            if (cur_col > 0 && i >= cur_col) {
                --start;
            }

            int count = columns_[i].getHolderCount();
            for (int j = start; j < count; ++j) {
                parent_->recycleViewHolder(columns_[i].getHolder(j));
            }
            columns_[i].removeHolders(start);
        }

        parent_->unfreezeLayout();
    }

    int GridListLayouter::onLayoutAtPosition(bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        int cur_row = cur ? cur_row_ : 0;
        int offset = cur ? cur_offset_ : 0;
        int item_count = adapter_->getItemCount();
        auto bounds = parent_->getContentBounds();

        columns_.setVertical(bounds.top, bounds.bottom);
        columns_.setHorizontal(bounds.left, bounds.right);

        parent_->freezeLayout();

        int row_index = 0;
        int view_index = 0;
        int total_height = 0;
        for (int row = cur_row; row * col_count_ < item_count; ++row) {
            int row_height = 0;
            for (int col = 0; col < col_count_; ++col) {
                int pos = row * col_count_ + col;
                if (pos >= item_count) {
                    break;
                }

                auto holder = columns_[col].getHolder(row_index);
                DCHECK(holder);

                int child_width = columns_[col].getWidth();

                int height = holder->item_view->getMeasuredHeight() + holder->getVertMargins();
                parent_->layoutViewHolder(
                    holder,
                    columns_[col].getLeft(), bounds.top + total_height - offset,
                    child_width, height);
                row_height = std::max(height, row_height);

                ++view_index;
            }

            ++row_index;
            total_height += row_height;

            if (total_height >= bounds.height() + offset) {
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

    int GridListLayouter::onScrollToPosition(int pos, int offset, bool cur) {
        if (!isAvailable()) {
            return 0;
        }

        int item_count = adapter_->getItemCount();
        auto bounds = parent_->getContentBounds();

        int cur_row = cur ? cur_row_ : pos / col_count_;
        offset = cur ? cur_offset_ : offset;

        int row_count = (item_count + (col_count_ - 1)) / col_count_;
        if (cur_row >= row_count) {
            if (row_count > 0) {
                cur_row = row_count - 1;
            } else {
                cur_row = 0;
            }
            offset = 0;
        }

        int diff = 0;
        bool full_child_reached = false;

        parent_->freezeLayout();

        int row_index = 0;
        int view_index = 0;
        int total_height = 0;
        int cur_col = 0;
        for (int row = cur_row; row * col_count_ < item_count; ++row) {
            int col;
            int row_height = 0;
            for (col = 0; col < col_count_; ++col) {
                int i_pos = row * col_count_ + col;
                if (i_pos >= item_count) {
                    break;
                }

                auto holder = columns_[col].findAndInsertHolder(row_index, adapter_->getItemId(i_pos));
                if (!holder) {
                    holder = parent_->makeNewBindViewHolder(i_pos, view_index);
                    columns_[col].addHolder(holder, row_index);
                } else {
                    holder->adapter_position = i_pos;
                    adapter_->onBindViewHolder(holder, i_pos);
                }

                int child_width = columns_[col].getWidth();

                int cur_height = parent_->measureViewHolder(holder, child_width);
                parent_->layoutViewHolder(
                    holder,
                    columns_[col].getLeft(), bounds.top + total_height - offset,
                    child_width, cur_height);
                row_height = std::max(cur_height, row_height);

                ++view_index;
            }

            if (col > 0 && col < col_count_) {
                cur_col = col;
            } else {
                cur_col = 0;
            }

            ++row_index;
            total_height += row_height;

            diff = bounds.bottom - (bounds.top + total_height - offset);
            if (total_height >= bounds.height() + offset) {
                full_child_reached = true;
                break;
            }
        }

        for (int i = 0; i < col_count_; ++i) {
            int start = row_index;
            if (cur_col > 0 && i >= cur_col) {
                --start;
            }

            int count = columns_[i].getHolderCount();
            for (int j = start; j < count; ++j) {
                parent_->recycleViewHolder(columns_[i].getHolder(j));
            }
            columns_[i].removeHolders(start);
        }

        parent_->unfreezeLayout();

        if (!full_child_reached && row_count > 0 && diff > 0) {
            return diff;
        }

        return 0;
    }

    int GridListLayouter::onSmoothScrollToPosition(int pos, int offset) {
        return 0;
    }

    int GridListLayouter::onFillTopChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto top_holder = columns_.getFirst();
        if (!top_holder) {
            return 0;
        }
        auto cur_adapter_position = top_holder->adapter_position;

        parent_->freezeLayout();

        while (cur_adapter_position > 0 && !columns_.isTopFilled2(dy)) {
            --cur_adapter_position;
            int row = cur_adapter_position / col_count_;
            int col = cur_adapter_position % col_count_;

            DCHECK(col == col_count_ - 1);

            int tmp_ap = cur_adapter_position;
            std::vector<ListAdapter::ViewHolder*> tmps;

            int max_height = 0;
            for (int i = col_count_ - 1; i >= 0; --i) {
                int child_width = columns_[i].getWidth();
                auto new_holder = parent_->makeNewBindViewHolder(tmp_ap, 0);
                max_height = std::max(max_height, parent_->measureViewHolder(new_holder, child_width));
                tmps.push_back(new_holder);

                --tmp_ap;
            }

            int cur_top = columns_[0].getHoldersTop() - max_height;
            for (int i = col_count_ - 1; i >= 0; --i) {
                auto new_holder = tmps[col_count_ - 1 - i];
                int child_width = columns_[i].getWidth();
                int height = new_holder->item_view->getMeasuredHeight() + new_holder->getVertMargins();
                parent_->layoutViewHolder(
                    new_holder,
                    columns_[i].getLeft(), cur_top,
                    child_width, height);
                columns_[i].addHolder(new_holder, 0);

                if (i > 0) {
                    --cur_adapter_position;
                }
            }
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

    int GridListLayouter::onFillBottomChildren(int dy) {
        if (!isAvailable()) {
            return 0;
        }

        auto bottom_holder = columns_.getLast();
        if (!bottom_holder) {
            return 0;
        }
        auto cur_adapter_position = bottom_holder->adapter_position;

        parent_->freezeLayout();

        while (cur_adapter_position + 1 < adapter_->getItemCount() && !columns_.isBottomFilled2(dy)) {
            ++cur_adapter_position;
            int row = cur_adapter_position / col_count_;
            int col = cur_adapter_position % col_count_;

            DCHECK(col == 0);

            int prev_bottom = 0;
            if (row > 0) {
                int prev_index = columns_[col].getHolderCount() - 1;
                if (prev_index >= 0) {
                    for (int i = 0; i < col_count_; ++i) {
                        prev_bottom = std::max(prev_bottom, columns_[i].getHolder(prev_index)->getMgdBottom());
                    }
                }
            }

            for (int i = col; i < col_count_; ++i) {
                int child_width = columns_[i].getWidth();
                auto new_holder = parent_->makeNewBindViewHolder(cur_adapter_position, parent_->getChildCount());

                int height = parent_->measureViewHolder(new_holder, child_width);
                parent_->layoutViewHolder(
                    new_holder,
                    columns_[i].getLeft(), prev_bottom,
                    child_width, height);
                columns_[i].addHolder(new_holder);

                if (i < col_count_ - 1) {
                    ++cur_adapter_position;
                    if (cur_adapter_position >= adapter_->getItemCount()) {
                        break;
                    }
                }
            }
        }

        dy = columns_.getFinalScroll(dy);

        int min_col = 0;
        int min_adapter_pos = std::numeric_limits<int>::max();
        for (int i = 0; i < col_count_; ++i) {
            auto holder = columns_[i].getFirstVisible(dy);
            if (holder->adapter_position < min_adapter_pos) {
                min_col = i;
                min_adapter_pos = holder->adapter_position;
            }
        }

        int index = columns_[min_col].getIndexOfFirstVisible(dy) - 1;
        if (index >= 0) {
            for (int i = 0; i < col_count_; ++i) {
                for (int j = 0; j < index; ++j) {
                    parent_->recycleViewHolder(columns_[i].getHolder(j));
                }
                columns_[i].removeHolders(0, index - 0);
            }
        }

        parent_->unfreezeLayout();

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
            *next = *prev = 0;
            return;
        }

        auto item_count = adapter_->getItemCount();
        if (item_count == 0) {
            *next = *prev = 0;
            return;
        }

        int avg_height = 0;
        int count = columns_[0].getHolderCount();
        for (int i = 0; i < count; ++i) {
            int row_height = 0;
            for (int j = 0; j < col_count_; ++j) {
                auto holder = columns_[j].getHolder(i);
                if (holder) {
                    row_height = std::max(row_height, holder->getMgdHeight());
                }
            }
            avg_height += row_height;
        }

        int child_height;
        if (count == 0) {
            child_height = 0;
        } else {
            child_height = avg_height / count;
        }

        int i;
        int prev_total_height = cur_offset_;
        for (i = 0; i < cur_row_; ++i) {
            int width, height;
            int pos = col_count_ * i;
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

        int next_total_height = -cur_offset_;
        for (; i < row_count; ++i) {
            int width, height;
            int pos = col_count_ * i;
            if (false && parent_->getCachedSize(pos, &width, &height)) {
                next_total_height += height;
            } else {
                next_total_height += child_height;
            }
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
        auto bounds = parent_->getContentBounds();
        if (bounds.empty()) {
            return;
        }

        int tmp_ap = 0;
        int tmp_offset = 0;
        bool is_first = true;
        for (int i = 0; i < col_count_; ++i) {
            auto holder = columns_[i].getFirstVisible();
            if (holder) {
                if (is_first) {
                    tmp_ap = holder->adapter_position;
                    tmp_offset = bounds.top - holder->getMgdTop();
                    is_first = false;
                } else {
                    if (holder->adapter_position < tmp_ap) {
                        tmp_ap = holder->adapter_position;
                        tmp_offset = bounds.top - holder->getMgdTop();
                    }
                }
            }
        }

        cur_row_ = tmp_ap / col_count_;
        cur_offset_ = tmp_offset;
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
        return !(columns_.isAllAtTop() && columns_.isAllAtCeil(item_count));
    }

    bool GridListLayouter::canScrollToBottom() const {
        int item_count = adapter_->getItemCount();
        return !(columns_.isAllAtBottom() && columns_.isAllAtFloor(item_count));
    }

    bool GridListLayouter::canScrollToLeft() const {
        return false;
    }

    bool GridListLayouter::canScrollToRight() const {
        return false;
    }

}
