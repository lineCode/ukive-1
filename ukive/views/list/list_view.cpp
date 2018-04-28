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
        view_recycler_ = std::make_unique<ViewHolderRecycler>(this);
    }


    void ListView::onLayout(
        bool changed, bool size_changed,
        int left, int top, int right, int bottom) {

        if (!initial_layouted_ || size_changed) {
            recordCurPositionAndOffset();
            locateToPosition(cur_position_, cur_offset_in_position_);

            scroll_bar_->setBounds(
                std::move(Rect(0, 0, getMeasuredWidth(), getMeasuredHeight())));

            updateOverlayScrollBar();
        }
    }

    bool ListView::onInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
            int resDy = determineVerticalScroll(e->getMouseWheel() * 40);
            if (resDy == 0) {
                break;
            }
            offsetChildViewTopAndBottom(resDy);
            recordCurPositionAndOffset();
            updateOverlayScrollBar();
            break;
        }

        return ViewGroup::onInputEvent(e);
    }

    void ListView::onDraw(Canvas* canvas) {
        ViewGroup::onDraw(canvas);
        scroll_bar_->onDraw(canvas);
    }


    void ListView::setAdapter(ListAdapter* adapter) {
        if (list_adapter_.get() == adapter) {
            return;
        }

        if (list_adapter_) {
            view_recycler_->clearAll();
            removeAllViews();

            list_adapter_->setListener(nullptr);
        }

        list_adapter_.reset(adapter);
        list_adapter_->setListener(this);

        locateToPosition(0);
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
        int resDeltaY = 0;
        if (dy > 0) {  //向上滚动
            resDeltaY = fillTopChildViews(dy);
            recycleBottomViews(resDeltaY);
        } else if (dy < 0) {  //向下滚动
            resDeltaY = fillBottomChildViews(dy);
            recycleTopViews(resDeltaY);
        }

        return resDeltaY;
    }

    void ListView::offsetChildViewTopAndBottom(int dy) {
        int size = getChildCount();
        for (int i = 0; i < size; ++i) {
            View* child = getChildAt(i);
            child->offsetTopAndBottom(dy);
        }
    }

    ListAdapter::ViewHolder* ListView::getFirstVisibleViewHolder() {
        int size = view_recycler_->getVisibleCount();
        for (int i = 0; i < size; ++i) {
            auto holder = view_recycler_->getVisible(i);
            View* item = holder->item_view;
            if (item->getBottom() > getContentBoundsInThis().top) {
                return holder;
            }
        }

        return nullptr;
    }

    ListAdapter::ViewHolder* ListView::getLastVisibleViewHolder() {
        int size = view_recycler_->getVisibleCount();
        for (int i = size; i > 0; --i) {
            auto holder = view_recycler_->getVisible(i - 1);
            View* item = holder->item_view;
            if (item->getTop() < getContentBoundsInThis().bottom) {
                return holder;
            }
        }

        return nullptr;
    }

    void ListView::recycleTopViews(int offset) {
        int length = 0;
        int start_pos = 0;
        int size = view_recycler_->getVisibleCount();

        for (int i = 0; i < size; ++i) {
            auto holder = view_recycler_->getVisible(i);
            View* item = holder->item_view;
            if (item->getBottom() + offset > getContentBoundsInThis().top) {
                length = i;
                break;
            }
        }

        if (length > 0) {
            view_recycler_->recycleFromParent(start_pos, length);
        }
    }

    void ListView::recycleBottomViews(int offset) {
        int start_pos = 0;
        int size = view_recycler_->getVisibleCount();

        for (int i = size; i > 0; --i) {
            auto holder = view_recycler_->getVisible(i - 1);
            View* item = holder->item_view;
            if (item->getTop() + offset < getContentBoundsInThis().bottom) {
                start_pos = i;
                break;
            }
        }

        if (start_pos + 1 <= size) {
            view_recycler_->recycleFromParent(start_pos);
        }
    }

    void ListView::updateOverlayScrollBar() {
        auto count = list_adapter_->getItemCount();
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
        auto holder = getFirstVisibleViewHolder();
        if (holder) {
            cur_position_ = holder->adapter_position;
            cur_offset_in_position_ = getContentBoundsInThis().top - holder->item_view->getTop();
        } else {
            cur_position_ = 0;
            cur_offset_in_position_ = 0;
        }
    }

    int ListView::fillTopChildViews(int dy) {
        int resDeltaY = 0;
        auto top_holder = view_recycler_->getVisible(0);
        if (top_holder) {
            Rect content_bound = getContentBoundsInThis();
            int top_diff = content_bound.top - top_holder->item_view->getTop();
            if (top_diff > 0 && top_diff >= dy) {
                resDeltaY = dy;
            } else if (top_diff > 0 && top_holder->adapter_position == 0) {
                resDeltaY = top_diff;
            } else {
                int cur_top = top_holder->item_view->getTop();
                int total_height = 0;
                int cur_adapter_position = top_holder->adapter_position;

                while (total_height + top_diff < dy
                    && cur_adapter_position > 0)
                {
                    --cur_adapter_position;
                    int item_id = list_adapter_->getItemId(cur_adapter_position);
                    auto new_holder = view_recycler_->reuse(item_id, 0);
                    if (new_holder == nullptr) {
                        new_holder = list_adapter_->onCreateViewHolder(this, cur_adapter_position);
                        view_recycler_->addToParent(new_holder, 0);
                    }

                    new_holder->item_id = item_id;
                    new_holder->adapter_position = cur_adapter_position;
                    list_adapter_->onBindViewHolder(new_holder, cur_adapter_position);

                    new_holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
                    int height = new_holder->item_view->getMeasuredHeight();
                    int left = content_bound.left;
                    int top = cur_top - height;
                    new_holder->item_view->layout(left, top, left + content_bound.width(), top + height);
                    total_height += height;
                    cur_top -= height;
                }

                if (top_diff >= 0) {
                    if (total_height + top_diff < dy) {
                        resDeltaY = total_height + top_diff;
                    } else {
                        resDeltaY = dy;
                    }
                }
            }
        }

        return resDeltaY;
    }

    int ListView::fillBottomChildViews(int dy) {
        int resDeltaY = 0;
        int vis_view_count = view_recycler_->getVisibleCount();
        if (vis_view_count == 0) {
            return 0;
        }

        auto bottom_holder = view_recycler_->getVisible(vis_view_count - 1);
        if (bottom_holder) {
            Rect content_bound = getContentBoundsInThis();
            int bottom_diff = bottom_holder->item_view->getBottom() - content_bound.bottom;
            if (bottom_diff > 0 && -bottom_diff <= dy) {
                resDeltaY = dy;
            } else if (bottom_diff > 0
                && bottom_holder->adapter_position + 1 == list_adapter_->getItemCount()) {
                resDeltaY = -bottom_diff;
            } else {
                int cur_bottom = bottom_holder->item_view->getBottom();
                int total_height = 0;
                int cur_adapter_position = bottom_holder->adapter_position;

                while (-(bottom_diff + total_height) > dy
                    && cur_adapter_position + 1 < list_adapter_->getItemCount())
                {
                    ++cur_adapter_position;
                    int item_id = list_adapter_->getItemId(cur_adapter_position);
                    auto new_holder = view_recycler_->reuse(item_id);
                    if (!new_holder) {
                        new_holder = list_adapter_->onCreateViewHolder(this, cur_adapter_position);
                        view_recycler_->addToParent(new_holder);
                    }

                    new_holder->item_id = item_id;
                    new_holder->adapter_position = cur_adapter_position;
                    list_adapter_->onBindViewHolder(new_holder, cur_adapter_position);

                    new_holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
                    int height = new_holder->item_view->getMeasuredHeight();
                    int left = content_bound.left;
                    int top = cur_bottom;
                    new_holder->item_view->layout(left, top, left + content_bound.width(), top + height);
                    total_height += height;
                    cur_bottom += height;
                }

                if (bottom_diff >= 0) {
                    if (-(bottom_diff + total_height) > dy) {
                        resDeltaY = -(bottom_diff + total_height);
                    } else {
                        resDeltaY = dy;
                    }
                }
            }
        }

        return resDeltaY;
    }

    void ListView::locateToPosition(int position, int offset) {
        if (!list_adapter_) {
            return;
        }

        Rect content_bound = getContentBoundsInThis();
        if (content_bound.empty()) {
            return;
        }

        initial_layouted_ = true;

        // scroll_animator_->Stop();

        int i = position;
        int index = 0;
        int child_count = list_adapter_->getItemCount();

        int total_height = 0;
        int overflow_index = 0;
        int overflow_count = 2;
        bool full_child_reached = false;

        for (; i < child_count; ++i, ++index) {
            auto holder = view_recycler_->getVisible(index);
            int item_id = list_adapter_->getItemId(i);
            if (!holder) {
                holder = view_recycler_->reuse(item_id);
                if (!holder) {
                    holder = list_adapter_->onCreateViewHolder(this, i);
                    view_recycler_->addToParent(holder);
                }
            }

            holder->item_id = item_id;
            holder->adapter_position = i;
            list_adapter_->onBindViewHolder(holder, i);

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

        auto holder = view_recycler_->getVisible(index - overflow_index);
        if (holder) {
            view_recycler_->recycleFromParent(index - overflow_index);
        }

        // 防止在列表大小变化时项目超出滑动范围。
        auto last_holder = getLastVisibleViewHolder();
        auto first_holder = getFirstVisibleViewHolder();
        if (last_holder && first_holder
            && last_holder->adapter_position + 1 == child_count) {
            int topDy = content_bound.top - first_holder->item_view->getBounds().top;
            if (topDy >= 0) {
                int bottomDy = content_bound.bottom - last_holder->item_view->getBounds().bottom;
                if (bottomDy > 0) {
                    int resDy = determineVerticalScroll(bottomDy);
                    if (resDy != 0) {
                        offsetChildViewTopAndBottom(resDy);
                    }
                }
            }
        }
    }

    void ListView::scrollToPosition(int position, int offset) {
        if (!list_adapter_) {
            return;
        }

        Rect content_bound = getContentBoundsInThis();
        if (content_bound.empty()) {
            return;
        }

        auto child_count = list_adapter_->getItemCount();
        if (position + 1 > child_count) {
            if (child_count > 0) {
                position = child_count - 1;
            } else {
                position = 0;
            }
            offset = 0;
        }

        // scroll_animator_->Stop();

        int i = position;
        int index = 0;

        int total_height = 0;
        int diff = 0;
        bool full_child_reached = false;

        for (; i < child_count; ++i, ++index) {
            auto holder = view_recycler_->getVisible(index);
            int item_id = list_adapter_->getItemId(i);
            if (!holder) {
                holder = view_recycler_->reuse(item_id);
                if (!holder) {
                    holder = list_adapter_->onCreateViewHolder(this, i);
                    view_recycler_->addToParent(holder);
                }
            }

            holder->item_id = item_id;
            holder->adapter_position = i;
            list_adapter_->onBindViewHolder(holder, i);

            holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
            int left = content_bound.left;
            int top = total_height + content_bound.top - offset;
            int height = holder->item_view->getMeasuredHeight();
            holder->item_view->layout(left, top, left + content_bound.width(), top + height);

            total_height += height;
            diff = content_bound.bottom - holder->item_view->getBottom();
            if (total_height >= content_bound.height() + offset) {
                full_child_reached = true;
                ++index;
                break;
            }
        }

        auto start_holder = view_recycler_->getVisible(index);
        if (start_holder) {
            view_recycler_->recycleFromParent(index);
        }

        if (!full_child_reached && child_count > 0 && diff > 0) {
            int resDiff = fillTopChildViews(diff);
            offsetChildViewTopAndBottom(resDiff);
        }
    }

    void ListView::smoothScrollToPosition(int position, int offset) {
        if (!list_adapter_) {
            return;
        }

        Rect content_bound = getContentBoundsInThis();
        if (content_bound.empty()) {
            return;
        }

        auto child_count = list_adapter_->getItemCount();
        if (child_count == 0) {
            return;
        }

        if (position + 1 > child_count) {
            position = child_count - 1;
            offset = 0;
        }

        // scroll_animator_->Stop();

        recordCurPositionAndOffset();

        int start_pos = cur_position_;
        int start_pos_offset = cur_offset_in_position_;
        int terminate_pos = position;
        int terminate_pos_offset = offset;
        bool front = (start_pos <= terminate_pos);

        int i = start_pos;
        int index = 0;

        int height = 0;
        int total_height = 0;
        bool full_child_reached = false;

        for (; (front ? (i <= terminate_pos) : (i >= terminate_pos)); (front ? ++i : --i), ++index) {
            auto holder = view_recycler_->getVisible(index);
            int item_id = list_adapter_->getItemId(i);
            if (!holder) {
                holder = view_recycler_->reuse(item_id);
                if (!holder) {
                    holder = list_adapter_->onCreateViewHolder(this, i);
                    view_recycler_->addToParent(holder);
                }
            }

            holder->item_id = item_id;
            holder->adapter_position = i;
            list_adapter_->onBindViewHolder(holder, i);

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

    void ListView::onItemRangeInserted(int start_position, int length) {
        if (length > 0) {
            //TODO: post insert op.
        }
    }

    void ListView::onItemRangeChanged(int start_position, int length) {
        if (length > 0) {
            //TODO: post change op.
        }
    }

    void ListView::onItemRangeRemoved(int start_position, int length) {
        if (length > 0) {
            //TODO: post remove op.
        }
    }

}