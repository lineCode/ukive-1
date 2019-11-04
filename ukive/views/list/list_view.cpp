#include "list_view.h"

#include <algorithm>

#include "ukive/log.h"
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
          initial_layouted_(false),
          scroller_(w)
    {
        scroll_bar_ = std::make_unique<OverlayScrollBar>();
        scroll_bar_->registerScrollHandler(std::bind(&ListView::onScrollBarChanged, this, std::placeholders::_1));
        scroll_bar_->setScrollBarWidth(w->dpToPxX(8));
        scroll_bar_->setScrollBarMinWidth(w->dpToPxX(16));

        recycler_ = std::make_unique<ViewHolderRecycler>(this);

        setTouchCapturable(true);
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

    bool ListView::onInterceptInputEvent(InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_DOWN:
            if (scroll_bar_->onMousePressed({ e->getX(), e->getY() })) {
                return true;
            }
            break;

        case InputEvent::EVM_MOVE:
            if (scroll_bar_->isInScrollBar({ e->getX(), e->getY() })) {
                return true;
            }
            break;

        case InputEvent::EVT_DOWN:
            start_touch_x_ = e->getX();
            start_touch_y_ = e->getY();
            is_touch_down_ = true;
            break;

        case InputEvent::EVT_UP:
        case InputEvent::EV_CANCEL:
        case InputEvent::EV_LEAVE_VIEW:
            is_touch_down_ = false;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - start_touch_x_;
                int dy = e->getY() - start_touch_y_;
                if (dx * dx + dy * dy > 12 * 12) {
                    start_touch_x_ = e->getX();
                    start_touch_y_ = e->getY();
                    return true;
                }
            }
            break;

        default:
            break;
        }
        return false;
    }

    bool ListView::onInputEvent(InputEvent* e) {
        bool result = ViewGroup::onInputEvent(e);
        if (e->isTouchEvent()) {
            velocity_calculator_.onInputEvent(e);
        }

        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
        {
            int wheel = e->getMouseWheel();
            if (wheel == 0 || !layouter_->canScroll(wheel > 0 ? ListLayouter::TOP : ListLayouter::BOTTOM)) {
                break;
            }

            result = true;
            if (std::abs(wheel) % WHEEL_DELTA) {
                scroller_.finish();
                scroller_.inertia(
                    0, 0, 0, getWindow()->dpToPxY(20 * wheel), true);
                invalidate();
            } else {
                scroller_.inertia(
                    0, 0, 0, getWindow()->dpToPxY(2 * wheel), true);
                invalidate();
            }
            break;
        }

        case InputEvent::EVM_DOWN:
            result = is_mouse_down_ = scroll_bar_->onMousePressed({ e->getX(), e->getY() });
            invalidate();
            break;

        case InputEvent::EVM_MOVE:
            if (is_mouse_down_) {
                result = true;
                scroll_bar_->onMouseDragged({ e->getX(), e->getY() });
                invalidate();
            } else if (scroll_bar_->isInScrollBar({ e->getX(), e->getY() })) {
                result = true;
            } else {
                invalidateInterceptStatus();
            }
            break;

        case InputEvent::EVM_UP:
            is_mouse_down_ = false;
            break;

        case InputEvent::EVT_DOWN:
            prev_touch_x_ = start_touch_x_ = e->getX();
            prev_touch_y_ = start_touch_y_ = e->getY();
            is_touch_down_ = true;
            result = true;
            break;

        case InputEvent::EVT_UP:
        {
            is_touch_down_ = false;

            float vy = velocity_calculator_.getVelocityY();
            if (vy == 0 || !layouter_->canScroll(vy > 0 ? ListLayouter::TOP : ListLayouter::BOTTOM)) {
                break;
            }
            result = true;

            /*DLOG(Log::INFO) << "EVT_UP | vx=" << velocity_calculator_.getVelocityX()
                << " vy=" << velocity_calculator_.getVelocityY();*/

            scroller_.inertia(
                0, 0,
                velocity_calculator_.getVelocityX(), vy);
            invalidate();
            break;
        }

        case InputEvent::EV_CANCEL:
        case InputEvent::EV_LEAVE_VIEW:
            is_touch_down_ = false;
            break;

        case InputEvent::EVT_MOVE:
            if (is_touch_down_) {
                int dx = e->getX() - prev_touch_x_;
                int dy = e->getY() - prev_touch_y_;
                processVerticalScroll(dy);
                result = true;

                prev_touch_x_ = e->getX();
                prev_touch_y_ = e->getY();
            }
            break;

        default:
            break;
        }

        return result;
    }

    void ListView::onDraw(Canvas* canvas) {
        ViewGroup::onDraw(canvas);
    }

    void ListView::onDrawOverChildren(Canvas* canvas) {
        ViewGroup::onDrawOverChildren(canvas);
        scroll_bar_->onDraw(canvas);
    }

    void ListView::onComputeScroll() {
        if (scroller_.compute()) {
            auto dy = scroller_.getDeltaY();
            if (dy == 0) {
                return;
            }

            if (!processVerticalScroll(dy)) {
                if (!layouter_->canScroll(dy > 0 ? ListLayouter::TOP : ListLayouter::BOTTOM)) {
                    scroller_.finish();
                }
            }
            invalidate();
        }
    }

    void ListView::onClick(View* v) {
        if (selected_listener_) {
            auto holder = layouter_->findViewHolderFromView(v);
            if (holder) {
                selected_listener_->onItemSelected(holder);
            }
        }
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
            layouter_->bind(this, adapter);
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
        if (layouter_) {
            layouter_->bind(this, adapter_.get());
        }
    }

    void ListView::scrollToPosition(int position, int offset, bool smooth) {
        if (smooth) {
            smoothScrollToPosition(position, offset);
        } else {
            directScrollToPosition(position, offset, false);
        }
    }

    void ListView::setItemSelectedListener(ListItemSelectedListener* l) {
        selected_listener_ = l;
    }

    void ListView::setChildRecycledListener(ListItemRecycledListener* l) {
        recycled_listener_ = l;
    }

    bool ListView::processVerticalScroll(int dy) {
        int res_dy = determineVerticalScroll(dy);
        if (res_dy == 0) {
            return false;
        }
        offsetChildViewTopAndBottom(res_dy);
        recordCurPositionAndOffset();
        updateOverlayScrollBar();
        return true;
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
        new_holder->item_view->setOnClickListener(this);
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
        if (!layouter_) {
            return;
        }

        int prev, next;
        layouter_->computeTotalHeight(&prev, &next);
        int total_height = prev + next;
        int prev_total_height = prev;

        float percent = static_cast<float>(prev_total_height) / (total_height - getHeight());
        percent = std::max(0.f, percent);
        percent = std::min(1.f, percent);

        scroll_bar_->update(total_height, percent);
    }

    void ListView::recordCurPositionAndOffset() {
        if (layouter_) {
            layouter_->recordCurPositionAndOffset();
        }
    }

    int ListView::fillTopChildViews(int dy) {
        if (!layouter_) {
            return 0;
        }
        return layouter_->onFillTopChildren(dy);
    }

    int ListView::fillBottomChildViews(int dy) {
        if (!layouter_) {
            return 0;
        }
        return layouter_->onFillBottomChildren(dy);
    }

    void ListView::layoutAtPosition(bool cur) {
        if (!layouter_) {
            return;
        }

        auto content_bound = getContentBounds();
        if (content_bound.empty()) {
            return;
        }

        initial_layouted_ = true;
        // scroll_animator_->Stop();

        int diff = layouter_->onLayoutAtPosition(cur);
        if (diff > 0) {
            diff = determineVerticalScroll(diff);
            if (diff != 0) {
                offsetChildViewTopAndBottom(diff);
            }
        }
    }

    void ListView::directScrollToPosition(int pos, int offset, bool cur) {
        if (!layouter_) {
            return;
        }
        auto bounds = getContentBounds();
        if (bounds.empty()) {
            return;
        }

        // scroll_animator_->Stop();

        int diff = layouter_->onScrollToPosition(pos, offset, cur);
        if (diff != 0) {
            diff = fillTopChildViews(diff);
            if (diff != 0) {
                offsetChildViewTopAndBottom(diff);
            }
        }
    }

    void ListView::smoothScrollToPosition(int pos, int offset) {
        if (!layouter_) {
            return;
        }
        auto content_bound = getContentBounds();
        if (content_bound.empty()) {
            return;
        }

        // scroll_animator_->Stop();

        recordCurPositionAndOffset();

        int total_height = layouter_->onSmoothScrollToPosition(pos, offset);
        if (total_height != 0) {
            // scroll_animator_->Stop();
            // scroll_animator_->StartUniform(0, -total_height, 0, 500);
        }
    }

    void ListView::onScrollBarChanged(int dy) {
        int prev, next;
        layouter_->computeTotalHeight(&prev, &next);
        int final_dy = determineVerticalScroll(prev - dy);
        if (final_dy == 0) {
            return;
        }

        offsetChildViewTopAndBottom(final_dy);
        recordCurPositionAndOffset();
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