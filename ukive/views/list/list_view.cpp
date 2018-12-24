#include "list_view.h"

#include <map>
#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/window/window.h"
#include "ukive/views/list/view_holder_recycler.h"
#include "ukive/views/list/overlay_scroll_bar.h"


namespace ukive {

    ListView::ListView(Window* wnd)
        :ViewGroup(wnd),
        cur_position_(0),
        cur_offset_in_position_(0),
        initial_layouted_(false) {

        scroll_bar_ = std::make_unique<OverlayScrollBar>();
        recycler_ = std::make_unique<ViewHolderRecycler>(this);
    }


    void ListView::onLayout(
        bool changed, bool size_changed,
        int left, int top, int right, int bottom) {

        if (!initial_layouted_ || size_changed) {
            recordCurPositionAndOffset();
            locateToPosition(cur_position_, cur_offset_in_position_);

            scroll_bar_->setBounds(Rect(0, 0, getMeasuredWidth(), getMeasuredHeight()));
            updateOverlayScrollBar();
        }
    }

    bool ListView::onInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
        {
            int resDy = determineVerticalScroll(e->getMouseWheel() * 40);
            if (resDy == 0) {
                break;
            }
            offsetChildViewTopAndBottom(resDy);
            recordCurPositionAndOffset();
            updateOverlayScrollBar();
            break;
        }

        default:
            break;
        }

        return ViewGroup::onInputEvent(e);
    }

    void ListView::onDraw(Canvas* canvas) {
        ViewGroup::onDraw(canvas);
    }

    void ListView::onDrawOverChildren(Canvas* canvas) {
        ViewGroup::onDrawOverChildren(canvas);
        scroll_bar_->onDraw(canvas);
    }

    void ListView::setAdapter(ListAdapter* adapter) {
        if (adapter_.get() == adapter) {
            return;
        }

        if (adapter_) {
            recycler_->clearAll();
            removeAllViews();

            adapter_->setListener(nullptr);
        }

        if (adapter) {
            adapter_.reset(adapter);
            adapter_->setListener(this);
            locateToPosition(0);
        }

        invalidate();
    }


    void ListView::scrollToPosition(int position, int offset, bool smooth) {
        if (smooth) {
            smoothScrollToPosition(position, offset);
        } else {
            scrollToPosition(position, offset);
        }
    }

    int ListView::determineVerticalScroll(int dy) {
        if (dy > 0) {  //向上滚动
            dy = fillTopChildViews(dy);
            recycleBottomViews(dy);
        } else if (dy < 0) {  //向下滚动
            dy = fillBottomChildViews(dy);
            recycleTopViews(dy);
        }

        return dy;
    }

    void ListView::offsetChildViewTopAndBottom(int dy) {
        int size = getChildCount();
        for (int i = 0; i < size; ++i) {
            View* child = getChildAt(i);
            child->offsetTopAndBottom(dy);
        }
    }

    ListAdapter::ViewHolder* ListView::getBindViewHolderAt(int index, int i) {
        auto holder = recycler_->getVisible(index);
        int item_id = adapter_->getItemId(i);
        if (!holder) {
            holder = recycler_->reuse(item_id);
            if (!holder) {
                holder = adapter_->onCreateViewHolder(this, i);
                recycler_->addToParent(holder);
            }
        }

        holder->item_id = item_id;
        holder->adapter_position = i;
        adapter_->onBindViewHolder(holder, i);

        return holder;
    }

    ListAdapter::ViewHolder* ListView::getFirstVisibleVH() {
        int size = recycler_->getVisibleCount();
        for (int i = 0; i < size; ++i) {
            auto holder = recycler_->getVisible(i);
            View* item = holder->item_view;
            if (item->getBottom() > getContentBounds().top) {
                return holder;
            }
        }

        return nullptr;
    }

    ListAdapter::ViewHolder* ListView::getLastVisibleVH() {
        int size = recycler_->getVisibleCount();
        for (int i = size; i > 0; --i) {
            auto holder = recycler_->getVisible(i - 1);
            View* item = holder->item_view;
            if (item->getTop() < getContentBounds().bottom) {
                return holder;
            }
        }

        return nullptr;
    }

    void ListView::recycleTopViews(int offset) {
        int length = 0;
        int start_pos = 0;
        int size = recycler_->getVisibleCount();

        for (int i = 0; i < size; ++i) {
            auto holder = recycler_->getVisible(i);
            View* item = holder->item_view;
            if (item->getBottom() + offset > getContentBounds().top) {
                length = i;
                break;
            }
        }

        if (length > 0) {
            recycler_->recycleFromParent(start_pos, length);
        }
    }

    void ListView::recycleBottomViews(int offset) {
        int start_pos = 0;
        int size = recycler_->getVisibleCount();

        for (int i = size; i > 0; --i) {
            auto holder = recycler_->getVisible(i - 1);
            View* item = holder->item_view;
            if (item->getTop() + offset < getContentBounds().bottom) {
                start_pos = i;
                break;
            }
        }

        if (start_pos + 1 <= size) {
            recycler_->recycleFromParent(start_pos);
        }
    }

    void ListView::updateOverlayScrollBar() {
        auto count = adapter_->getItemCount();
        int prev_total_height = cur_offset_in_position_;
        bool cannot_determine_height = false;

        int child_height = getChildAt(0)->getHeight();

        // TODO: Calculate ScrollBar position.
        for (int i = 0; i < cur_position_; ++i) {
            prev_total_height += child_height;
        }

        int next_total_height = -cur_offset_in_position_;
        for (int i = cur_position_; i < count; ++i) {
            next_total_height += child_height;
        }

        int total_height = prev_total_height + next_total_height;
        float percent = static_cast<float>(prev_total_height) / (total_height - getHeight());
        percent = std::max(0.f, percent);
        percent = std::min(1.f, percent);

        scroll_bar_->update(total_height, percent);
    }

    void ListView::recordCurPositionAndOffset() {
        auto holder = getFirstVisibleVH();
        if (holder) {
            cur_position_ = holder->adapter_position;
            cur_offset_in_position_ = getContentBounds().top - holder->item_view->getTop();
        } else {
            cur_position_ = 0;
            cur_offset_in_position_ = 0;
        }
    }

    int ListView::fillTopChildViews(int dy) {
        auto top_holder = recycler_->getVisible(0);
        auto prev_holder = top_holder;
        if (!top_holder) {
            return 0;
        }

        auto bounds = getContentBounds();
        auto cur_adapter_position = top_holder->adapter_position;

        while (cur_adapter_position > 0 && bounds.top - prev_holder->item_view->getTop() < dy) {
            --cur_adapter_position;
            int item_id = adapter_->getItemId(cur_adapter_position);
            auto new_holder = recycler_->reuse(item_id, 0);
            if (new_holder == nullptr) {
                new_holder = adapter_->onCreateViewHolder(this, cur_adapter_position);
                recycler_->addToParent(new_holder, 0);
            }

            new_holder->item_id = item_id;
            new_holder->adapter_position = cur_adapter_position;
            adapter_->onBindViewHolder(new_holder, cur_adapter_position);

            new_holder->item_view->measure(bounds.width(), 0, EXACTLY, UNKNOWN);
            int height = new_holder->item_view->getMeasuredHeight();
            int left = bounds.left;
            int top = prev_holder->item_view->getTop() - height;
            new_holder->item_view->layout(left, top, left + bounds.width(), top + height);

            prev_holder = new_holder;
        }

        if (bounds.top - prev_holder->item_view->getTop() >= dy) {
            return dy;
        }

        if (cur_adapter_position == 0) {
            return std::max(bounds.top - prev_holder->item_view->getTop(), 0);
        }

        return 0;
    }

    int ListView::fillBottomChildViews(int dy) {
        int child_count = recycler_->getVisibleCount();
        if (child_count == 0) {
            return 0;
        }

        auto bottom_holder = recycler_->getVisible(child_count - 1);
        auto prev_holder = bottom_holder;
        if (!bottom_holder) {
            return 0;
        }

        auto bounds = getContentBounds();
        auto cur_adapter_position = bottom_holder->adapter_position;

        while (cur_adapter_position + 1 < adapter_->getItemCount() &&
            bounds.bottom - prev_holder->item_view->getBottom() > dy)
        {
            ++cur_adapter_position;
            int item_id = adapter_->getItemId(cur_adapter_position);
            auto new_holder = recycler_->reuse(item_id);
            if (!new_holder) {
                new_holder = adapter_->onCreateViewHolder(this, cur_adapter_position);
                recycler_->addToParent(new_holder);
            }

            new_holder->item_id = item_id;
            new_holder->adapter_position = cur_adapter_position;
            adapter_->onBindViewHolder(new_holder, cur_adapter_position);

            new_holder->item_view->measure(bounds.width(), 0, EXACTLY, UNKNOWN);
            int height = new_holder->item_view->getMeasuredHeight();
            int left = bounds.left;
            int top = prev_holder->item_view->getBottom();
            new_holder->item_view->layout(left, top, left + bounds.width(), top + height);

            prev_holder = new_holder;
        }

        if (bounds.bottom - prev_holder->item_view->getBottom() <= dy) {
            return dy;
        }

        if (cur_adapter_position == adapter_->getItemCount() - 1) {
            return std::min(bounds.bottom - prev_holder->item_view->getBottom(), 0);
        }

        return 0;
    }

    void ListView::locateToPosition(int pos, int offset) {
        if (!adapter_) {
            return;
        }

        Rect content_bound = getContentBounds();
        if (content_bound.empty()) {
            return;
        }

        initial_layouted_ = true;

        // scroll_animator_->Stop();

        int index = 0;
        int child_count = adapter_->getItemCount();

        int total_height = 0;
        int overflow_index = 0;
        int overflow_count = 2;
        bool full_child_reached = false;

        for (int i = pos; i < child_count; ++i, ++index) {
            auto holder = getBindViewHolderAt(index, i);

            holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
            int height = holder->item_view->getMeasuredHeight();
            int left = content_bound.left;
            int top = content_bound.top + total_height - offset;
            holder->item_view->layout(left, top, left + content_bound.width(), top + height);

            total_height += height;
            if (total_height >= content_bound.height() + offset) {
                if (full_child_reached) {
                    ++overflow_index;
                } else {
                    full_child_reached = true;
                }

                if (overflow_index >= overflow_count) {
                    ++index;
                    break;
                }
            }
        }

        auto holder = recycler_->getVisible(index - overflow_index);
        if (holder) {
            recycler_->recycleFromParent(index - overflow_index);
        }

        // 防止在列表大小变化时项目超出滑动范围。
        auto last_holder = getLastVisibleVH();
        auto first_holder = getFirstVisibleVH();
        if (last_holder && first_holder
            && last_holder->adapter_position + 1 == child_count)
        {
            bool can_scroll = ((first_holder->adapter_position == 0 &&
                content_bound.top - first_holder->item_view->getTop() > 0) ||
                first_holder->adapter_position > 0);
            if (can_scroll) {
                int bottom_dy = content_bound.bottom - last_holder->item_view->getBounds().bottom;
                if (bottom_dy > 0) {
                    int res_dy = determineVerticalScroll(bottom_dy);
                    if (res_dy != 0) {
                        offsetChildViewTopAndBottom(res_dy);
                    }
                }
            }
        }
    }

    void ListView::scrollToPosition(int pos, int offset) {
        if (!adapter_) {
            return;
        }

        Rect bounds = getContentBounds();
        if (bounds.empty()) {
            return;
        }

        auto child_count = adapter_->getItemCount();
        if (pos + 1 > child_count) {
            if (child_count > 0) {
                pos = child_count - 1;
            } else {
                pos = 0;
            }
            offset = 0;
        }

        // scroll_animator_->Stop();

        int i = pos;
        int index = 0;

        int total_height = 0;
        int diff = 0;
        bool full_child_reached = false;

        for (; i < child_count; ++i, ++index) {
            auto holder = getBindViewHolderAt(index, i);

            holder->item_view->measure(bounds.width(), 0, EXACTLY, UNKNOWN);
            int left = bounds.left;
            int top = total_height + bounds.top - offset;
            int height = holder->item_view->getMeasuredHeight();
            holder->item_view->layout(left, top, left + bounds.width(), top + height);

            total_height += height;
            diff = bounds.bottom - holder->item_view->getBottom();
            if (total_height >= bounds.height() + offset) {
                full_child_reached = true;
                ++index;
                break;
            }
        }

        auto start_holder = recycler_->getVisible(index);
        if (start_holder) {
            recycler_->recycleFromParent(index);
        }

        if (!full_child_reached && child_count > 0 && diff > 0) {
            int resDiff = fillTopChildViews(diff);
            offsetChildViewTopAndBottom(resDiff);
        }
    }

    void ListView::smoothScrollToPosition(int pos, int offset) {
        if (!adapter_) {
            return;
        }

        Rect content_bound = getContentBounds();
        if (content_bound.empty()) {
            return;
        }

        auto child_count = adapter_->getItemCount();
        if (child_count == 0) {
            return;
        }

        if (pos + 1 > child_count) {
            pos = child_count - 1;
            offset = 0;
        }

        // scroll_animator_->Stop();

        recordCurPositionAndOffset();

        int start_pos = cur_position_;
        int start_pos_offset = cur_offset_in_position_;
        int terminate_pos = pos;
        int terminate_pos_offset = offset;
        bool front = (start_pos <= terminate_pos);

        int i = start_pos;
        int index = 0;

        int height = 0;
        int total_height = 0;
        bool full_child_reached = false;

        for (; (front ? (i <= terminate_pos) : (i >= terminate_pos)); (front ? ++i : --i), ++index) {
            auto holder = getBindViewHolderAt(index, i);

            holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
            int left = content_bound.left;
            int top = total_height + content_bound.top + (front ? -offset : offset);
            height = holder->item_view->getMeasuredHeight();
            holder->item_view->layout(left, top, left + content_bound.width(), top + height);

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

        if (total_height != 0) {
            // scroll_animator_->Stop();
            // scroll_animator_->StartUniform(0, -total_height, 0, 500);
        }
    }

    void ListView::scrollByScrollBar(int dy) {
        int final_dy = determineVerticalScroll(dy);
        if (final_dy == 0) {
            return;
        }

        offsetChildViewTopAndBottom(final_dy);
        invalidate();
    }

    //////////////////////////////////////////////////////////////////
    // ScrollDelegate implementation:

    /*void ListView::OnScroll(float dx, float dy) {
        int resDy = determineVerticalScroll(static_cast<int>(dy));
        if (resDy == 0)
            return;
        offsetChildViewTopAndBottom(resDy);
        invalidate();
    }*/

    //////////////////////////////////////////////////////////////////
    // ListDataSetListener implementation:

    void ListView::onDataSetChanged() {
        recordCurPositionAndOffset();
        scrollToPosition(cur_position_, cur_offset_in_position_);
        invalidate();
    }

    void ListView::onItemRangeInserted(int start_pos, int length) {
        if (length > 0) {
            //TODO: post insert op.
        }
    }

    void ListView::onItemRangeChanged(int start_pos, int length) {
        if (length > 0) {
            //TODO: post change op.
        }
    }

    void ListView::onItemRangeRemoved(int start_pos, int length) {
        if (length > 0) {
            //TODO: post remove op.
        }
    }

}