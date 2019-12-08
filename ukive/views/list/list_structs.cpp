#include "ukive/views/list/list_structs.h"

#include <algorithm>

#include "utils/log.h"
#include "utils/stl_utils.h"


namespace ukive {

    // Column
    Column::Column()
        : top_(0),
          bottom_(0),
          left_(0),
          right_(0) {
    }

    void Column::setVertical(int top, int bottom) {
        top_ = top;
        bottom_ = bottom;
    }

    void Column::setHorizontal(int left, int right) {
        left_ = left;
        right_ = right;
    }

    void Column::addHolder(ListAdapter::ViewHolder* holder) {
        holders_.push_back(holder);
    }

    void Column::addHolder(ListAdapter::ViewHolder* holder, int index) {
        holders_.insert(holders_.begin() + index, holder);
    }

    void Column::removeHolder(int index) {
        holders_.erase(holders_.begin() + index);
    }

    void Column::removeHolders(int start) {
        holders_.erase(holders_.begin() + start, holders_.end());
    }

    void Column::removeHolders(int start, int length) {
        int end = start + length;
        holders_.erase(holders_.begin() + start, holders_.begin() + end);
    }

    void Column::clear() {
        holders_.clear();
    }

    int Column::getLeft() const {
        return left_;
    }

    int Column::getRight() const {
        return right_;
    }

    int Column::getWidth() const {
        return right_ - left_;
    }

    int Column::getHoldersTop() const {
        if (!holders_.empty()) {
            return holders_.front()->getMgdTop();
        }
        return top_;
    }

    int Column::getHoldersBottom() const {
        if (!holders_.empty()) {
            return holders_.back()->getMgdBottom();
        }
        return top_;
    }

    int Column::getHolderCount() const {
        return utl::STLCInt(holders_.size());
    }

    int Column::getFinalScroll(int dy) const {
        if (dy > 0) {
            if (top_ - getHoldersTop() < dy) {
                dy = std::max(top_ - getHoldersTop(), 0);
            }
        } else if (dy < 0) {
            if (bottom_ - getHoldersBottom() > dy) {
                dy = std::min(bottom_ - getHoldersBottom(), 0);
            }
        }
        return dy;
    }

    int Column::getIndexOfHolder(int adapter_pos) const {
        int index = 0;
        for (auto it = holders_.cbegin(); it != holders_.cend(); ++it, ++index) {
            if ((*it)->adapter_position == adapter_pos) {
                return index;
            }
            if ((*it)->adapter_position > adapter_pos) {
                break;
            }
        }
        return -1;
    }

    int Column::getIndexOfFirstVisible(int dy) const {
        int index = 0;
        for (auto it = holders_.cbegin(); it != holders_.cend(); ++it, ++index) {
            if ((*it)->getMgdBottom() + dy > top_) {
                return index;
            }
        }
        return -1;
    }

    int Column::getIndexOfLastVisible(int dy) const {
        int index = getHolderCount() - 1;
        for (auto it = holders_.crbegin(); it != holders_.crend(); ++it, --index) {
            if ((*it)->getMgdTop() + dy < bottom_) {
                return index;
            }
        }
        return -1;
    }

    ListAdapter::ViewHolder* Column::getFront() const {
        if (!holders_.empty()) {
            return holders_.front();
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* Column::getRear() const {
        if (!holders_.empty()) {
            return holders_.back();
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* Column::getHolder(int index) const {
        if (index >= 0 && index < utl::STLCInt(holders_.size())) {
            return holders_[index];
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* Column::getFirstVisible(int dy) const {
        for (auto it = holders_.cbegin(); it != holders_.cend(); ++it) {
            if ((*it)->getMgdBottom() + dy > top_) {
                return *it;
            }
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* Column::getLastVisible(int dy) const {
        for (auto it = holders_.crbegin(); it != holders_.crend(); ++it) {
            if ((*it)->getMgdTop() + dy < bottom_) {
                return *it;
            }
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* Column::getHolderByPos(int adapter_pos) const {
        for (auto it = holders_.cbegin(); it != holders_.cend(); ++it) {
            if ((*it)->adapter_position == adapter_pos) {
                return *it;
            }
            if ((*it)->adapter_position > adapter_pos) {
                break;
            }
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* Column::findAndInsertHolder(int start_index, int item_id) {
        if (start_index < 0 || start_index >= utl::STLCInt(holders_.size())) {
            return nullptr;
        }

        auto holder = holders_[start_index];
        if (holder->item_id == item_id) {
            return holder;
        }

        ++start_index;
        ListAdapter::ViewHolder* target = nullptr;
        for (auto it = holders_.begin() + start_index; it != holders_.end(); ++it) {
            if ((*it)->item_id == item_id) {
                target = *it;
                holders_.erase(it);
                break;
            }
        }

        if (target) {
            addHolder(target, start_index);
        }
        return target;
    }

    ListAdapter::ViewHolder* Column::findHolderFromView(View* v) const {
        for (const auto& holder : holders_) {
            if (holder->item_view == v) {
                return holder;
            }
        }
        return nullptr;
    }

    bool Column::atTop() const {
        if (holders_.empty() || holders_.front()->getMgdTop() == top_) {
            return true;
        }
        return false;
    }

    bool Column::atBottom() const {
        if (holders_.empty() || holders_.back()->getMgdBottom() == bottom_) {
            return true;
        }
        return false;
    }

    bool Column::isTopFilled(int dy) const {
        if (!holders_.empty()) {
            if (holders_.front()->getMgdTop() + dy <= top_) {
                return true;
            }
        }
        return false;
    }

    bool Column::isBottomFilled(int dy) const {
        if (!holders_.empty()) {
            if (holders_.back()->getMgdBottom() + dy >= bottom_) {
                return true;
            }
        }
        return false;
    }


    // ColumnCollection
    ColumnCollection::ColumnCollection(int col_count)
        : col_count_(col_count) {
          columns_.assign(col_count, {});
    }

    Column& ColumnCollection::operator[](int col) {
        return columns_[col];
    }

    void ColumnCollection::setVertical(int top, int bottom) {
        for (auto& col : columns_) {
            col.setVertical(top, bottom);
        }
    }

    void ColumnCollection::setHorizontal(int left, int right) {
        int cur_left = left;
        int child_width = (right - left) / static_cast<float>(col_count_);
        int remained_width = (right - left) - col_count_ * child_width;
        for (int i = 0; i < col_count_; ++i) {
            if (remained_width > 0) {
                columns_[i].setHorizontal(cur_left, cur_left + child_width + 1);
                cur_left += child_width + 1;
                --remained_width;
            } else {
                columns_[i].setHorizontal(cur_left, cur_left + child_width);
                cur_left += child_width;
            }
        }
    }

    void ColumnCollection::clear() {
        for (auto& col : columns_) {
            col.clear();
        }
    }

    int ColumnCollection::getFinalScroll(int dy) const {
        int final_dy = 0;
        for (auto& col : columns_) {
            auto tmp_dy = col.getFinalScroll(dy);
            if (std::abs(tmp_dy) > std::abs(final_dy)) {
                final_dy = tmp_dy;
            }
        }
        return final_dy;
    }

    ListAdapter::ViewHolder* ColumnCollection::getFirst() const {
        return columns_[0].getFront();
    }

    ListAdapter::ViewHolder* ColumnCollection::getLast() const {
        int count = columns_[0].getHolderCount();
        if (count <= 0) {
            return nullptr;
        }
        for (int i = col_count_ - 1; i >= 0; --i) {
            auto holder = columns_[i].getHolder(count - 1);
            if (holder) {
                return holder;
            }
        }
        return nullptr;
    }

    ListAdapter::ViewHolder* ColumnCollection::getTopStart() const {
        int index = 0;
        int max_adapter_pos = 0;
        ListAdapter::ViewHolder* start_holder = nullptr;
        for (const auto& col : columns_) {
            auto front = col.getFront();
            if (front) {
                if (!start_holder) {
                    max_adapter_pos = front->adapter_position - index;
                    start_holder = front;
                } else {
                    if (front->adapter_position - index > max_adapter_pos) {
                        max_adapter_pos = front->adapter_position - index;
                        start_holder = front;
                    }
                }
            }
            ++index;
        }
        return start_holder;
    }

    ListAdapter::ViewHolder* ColumnCollection::getBottomStart() const {
        int index = col_count_ - 1;
        int min_adapter_pos = 0;
        ListAdapter::ViewHolder* start_holder = nullptr;
        for (auto it = columns_.crbegin(); it != columns_.crend(); ++it) {
            auto rear = (*it).getRear();
            if (rear) {
                if (!start_holder) {
                    min_adapter_pos = rear->adapter_position - index;
                    start_holder = rear;
                } else {
                    if (rear->adapter_position - index < min_adapter_pos) {
                        min_adapter_pos = rear->adapter_position - index;
                        start_holder = rear;
                    }
                }
            }
            --index;
        }
        return start_holder;
    }

    ListAdapter::ViewHolder* ColumnCollection::getHolderByPos(int adapter_pos) const {
        //int row = adapter_pos / columns_.size();
        int col = adapter_pos % columns_.size();
        return columns_[col].getHolderByPos(adapter_pos);
    }

    ListAdapter::ViewHolder* ColumnCollection::getTopmost() const {
        int top = 0;
        ListAdapter::ViewHolder* result = nullptr;
        for (const auto& c : columns_) {
            auto front = c.getFront();
            if (!front) {
                continue;
            }

            if (!result || front->getMgdTop() < top) {
                top = front->getMgdTop();
                result = front;
            }
        }
        return result;
    }

    ListAdapter::ViewHolder* ColumnCollection::getBottomost() const {
        int bottom = 0;
        ListAdapter::ViewHolder* result = nullptr;
        for (const auto& c : columns_) {
            auto rear = c.getRear();
            if (!rear) {
                continue;
            }

            if (!result || rear->getMgdBottom() > bottom) {
                bottom = rear->getMgdBottom();
                result = rear;
            }
        }
        return result;
    }

    ListAdapter::ViewHolder* ColumnCollection::findHolderFromView(View* v) const {
        for (const auto& c : columns_) {
            auto holder = c.findHolderFromView(v);
            if (holder) {
                return holder;
            }
        }
        return nullptr;
    }

    bool ColumnCollection::isAllAtTop() const {
        for (const auto& c : columns_) {
            if (!c.atTop()) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isAllAtBottom() const {
        for (const auto& c : columns_) {
            if (!c.atBottom()) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isTopFilled(int dy) const {
        for (const auto& c : columns_) {
            if (!c.isTopFilled(dy)) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isBottomFilled(int dy) const {
        for (const auto& c : columns_) {
            if (!c.isBottomFilled(dy)) {
                return false;
            }
        }
        return true;
    }

    bool ColumnCollection::isTopFilled2(int dy) const {
        bool fill = false;
        int count = columns_[0].getHolderCount();
        for (const auto& c : columns_) {
            if (!fill && c.isTopFilled(dy)) {
                fill = true;
            }
            if (c.getHolderCount() != count) {
                return false;
            }
        }
        return fill;
    }

    bool ColumnCollection::isBottomFilled2(int dy) const {
        bool fill = false;
        int count = columns_[0].getHolderCount();
        for (const auto& c : columns_) {
            if (!fill && c.isBottomFilled(dy)) {
                fill = true;
            }
            if (c.getHolderCount() != count) {
                return false;
            }
        }
        return fill;
    }

    bool ColumnCollection::isAllAtCeil(int item_count) const {
        for (const auto& c : columns_) {
            auto front = c.getFront();
            if (front) {
                if (front->adapter_position >= col_count_) {
                    return false;
                }
            }
        }
        return true;
    }

    bool ColumnCollection::isAllAtFloor(int item_count) const {
        for (const auto& c : columns_) {
            auto rear = c.getRear();
            if (rear) {
                if (rear->adapter_position + col_count_ < item_count) {
                    return false;
                }
            }
        }
        return true;
    }

}