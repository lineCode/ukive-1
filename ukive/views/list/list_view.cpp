#include "list_view.h"

#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/window/window.h"
#include "ukive/views/list/view_holder_recycler.h"
#include "ukive/views/list/overlay_scroll_bar.h"
#include "ukive/views/list/list_layouter.h"
#include "ukive/views/layout/layout_params.h"


namespace ukive {

    ListView::ListView(Window* w)
        : ListView(w, {}) {}

    ListView::ListView(Window* w, AttrsRef attrs)
        : ViewGroup(w, attrs),
          initial_layouted_(false)
    {
        scroll_bar_ = std::make_unique<OverlayScrollBar>();
        recycler_ = std::make_unique<ViewHolderRecycler>(this);
    }

    void ListView::onLayout(
        bool changed, bool size_changed,
        int left, int top, int right, int bottom)
    {
        if (!initial_layouted_ || size_changed || force_layout_) {
            force_layout_ = false;
            recordCurPositionAndOffset();
            layoutAtPosition(true);

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
            recycler_->clear();
            removeAllViews();

            adapter_->setListener(nullptr);
        }

        if (layouter_) {
            layouter_->onClear();
        }

        if (adapter) {
            adapter_.reset(adapter);
            adapter_->setListener(this);
            layoutAtPosition(false);
        }

        invalidate();
    }

    void ListView::setLayouter(ListLayouter* layouter) {
        layouter_.reset(layouter);
    }

    void ListView::scrollToPosition(int position, int offset, bool smooth) {
        if (smooth) {
            smoothScrollToPosition(position, offset);
        } else {
            directScrollToPosition(position, offset, false);
        }
    }

    void ListView::setChildRecycledListener(ListItemRecycledListener* l) {
        recycled_listener_ = l;
    }

    int ListView::determineVerticalScroll(int dy) {
        if (dy > 0) {  // 向上滚动，当前页面内容下沉
            dy = fillTopChildViews(dy);
        } else if (dy < 0) {  // 向下滚动，当前页面内容上升
            dy = fillBottomChildViews(dy);
        }

        return dy;
    }

    void ListView::offsetChildViewTopAndBottom(int dy) {
        int size = getChildCount();
        for (int i = 0; i < size; ++i) {
            auto child = getChildAt(i);
            child->offsetTopAndBottom(dy);
        }
    }

    ListAdapter::ViewHolder* ListView::makeNewBindViewHolder(int adapter_pos, int view_index) {
        int item_id = adapter_->getItemId(adapter_pos);
        auto new_holder = recycler_->reuse(item_id, view_index);
        if (!new_holder) {
            new_holder = adapter_->onCreateViewHolder(this, adapter_pos);
            recycler_->addToParent(new_holder, view_index);
        }

        new_holder->item_id = item_id;
        new_holder->adapter_position = adapter_pos;
        adapter_->onBindViewHolder(new_holder, adapter_pos);
        return new_holder;
    }

    void ListView::recycleViewHolder(ListAdapter::ViewHolder* holder) {
        if (recycled_listener_) {
            recycled_listener_->onChildRecycled(holder);
        }
        recycler_->recycleFromParent(holder);
    }

    int ListView::findViewIndexFromStart(ListAdapter::ViewHolder* holder) const {
        for (int i = 0; i < getChildCount(); ++i) {
            if (getChildAt(i) == holder->item_view) {
                return i;
            }
        }
        return -1;
    }

    int ListView::findViewIndexFromEnd(ListAdapter::ViewHolder* holder) const {
        for (int i = getChildCount() - 1; i >= 0; --i) {
            if (getChildAt(i) == holder->item_view) {
                return i;
            }
        }
        return -1;
    }

    int ListView::measureViewHolder(ListAdapter::ViewHolder* holder, int width) {
        auto child_lp = holder->item_view->getLayoutParams();
        int child_height;
        int child_height_mode;
        getChildMeasure(
            0, View::UNKNOWN, 0,
            child_lp->height, &child_height, &child_height_mode);

        int width_margin = child_lp->left_margin +
            child_lp->right_margin +
            holder->ex_margins.left +
            holder->ex_margins.right;

        int height_margin = child_lp->top_margin +
            child_lp->bottom_margin +
            holder->ex_margins.top +
            holder->ex_margins.bottom;

        width = std::max(width - width_margin, 0);

        holder->item_view->measure(width, child_height, View::EXACTLY, child_height_mode);
        return holder->item_view->getMeasuredHeight() + height_margin;
    }

    void ListView::layoutViewHolder(
        ListAdapter::ViewHolder* holder, int left, int top, int width, int height)
    {
        auto child_lp = holder->item_view->getLayoutParams();
        holder->item_view->layout(
            left + child_lp->left_margin + holder->ex_margins.left,
            top + child_lp->top_margin + holder->ex_margins.top,
            left + width - child_lp->right_margin - holder->ex_margins.right,
            top + height - child_lp->bottom_margin - holder->ex_margins.bottom);
    }

    void ListView::updateOverlayScrollBar() {
        if (!adapter_ || !layouter_) {
            return;
        }

        auto height_pair = layouter_->computeTotalHeight(this, adapter_.get());
        int total_height = height_pair.first + height_pair.second;
        int prev_total_height = height_pair.first;

        float percent = static_cast<float>(prev_total_height) / (total_height - getHeight());
        percent = std::max(0.f, percent);
        percent = std::min(1.f, percent);

        scroll_bar_->update(total_height, percent);
    }

    void ListView::recordCurPositionAndOffset() {
        if (layouter_) {
            layouter_->recordCurPositionAndOffset(this);
        }
    }

    int ListView::fillTopChildViews(int dy) {
        if (!adapter_ || !layouter_) {
            return 0;
        }
        return layouter_->onFillTopChildren(this, adapter_.get(), dy);
    }

    int ListView::fillBottomChildViews(int dy) {
        if (!adapter_ || !layouter_) {
            return 0;
        }
        return layouter_->onFillBottomChildren(this, adapter_.get(), dy);
    }

    void ListView::layoutAtPosition(bool cur) {
        if (!adapter_ || !layouter_) {
            return;
        }

        auto content_bound = getContentBounds();
        if (content_bound.empty()) {
            return;
        }

        initial_layouted_ = true;
        // scroll_animator_->Stop();

        int diff = layouter_->onLayoutAtPosition(this, adapter_.get(), cur);
        if (diff > 0) {
            diff = determineVerticalScroll(diff);
            if (diff != 0) {
                offsetChildViewTopAndBottom(diff);
            }
        }
    }

    void ListView::directScrollToPosition(int pos, int offset, bool cur) {
        if (!adapter_ || !layouter_) {
            return;
        }
        auto bounds = getContentBounds();
        if (bounds.empty()) {
            return;
        }

        // scroll_animator_->Stop();

        int diff = layouter_->onScrollToPosition(this, adapter_.get(), pos, offset, cur);
        if (diff != 0) {
            diff = fillTopChildViews(diff);
            if (diff != 0) {
                offsetChildViewTopAndBottom(diff);
            }
        }
    }

    void ListView::smoothScrollToPosition(int pos, int offset) {
        if (!adapter_ || !layouter_) {
            return;
        }
        auto content_bound = getContentBounds();
        if (content_bound.empty()) {
            return;
        }

        // scroll_animator_->Stop();

        recordCurPositionAndOffset();

        int total_height = layouter_->onSmoothScrollToPosition(this, adapter_.get(), pos, offset);
        if (total_height != 0) {
            // scroll_animator_->Stop();
            // scroll_animator_->StartUniform(0, -total_height, 0, 500);
        }
    }

    void ListView::onScrollBarChanged(int dy) {
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
        directScrollToPosition(0, 0, true);
        requestLayout();
        invalidate();
        force_layout_ = true;
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