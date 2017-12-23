#include "list_view.h"

#include <map>

#include "ukive/event/input_event.h"
#include "ukive/window/window.h"


namespace ukive {

    //////////////////////////////////////////////////////////////////
    // ViewRecycler

    void ViewRecycler::AddViewHolder(ListAdapter::ViewHolder *holder) {
        holder->recycled = false;
        visible_view_holder_.push_back(holder);
        parent_->addView(holder->item_view);
    }

    void ViewRecycler::AddViewHolder(ListAdapter::ViewHolder *holder, size_t pos) {
        size_t index = 0;
        for (auto it = visible_view_holder_.begin();
            it != visible_view_holder_.end(); ++it) {
            if (index == pos) {
                holder->recycled = false;
                visible_view_holder_.insert(it, holder);
                parent_->addView(pos, holder->item_view, nullptr);
                return;
            }

            ++index;
        }
    }

    void ViewRecycler::AddRecycledViewHolder(ListAdapter::ViewHolder *holder) {
        holder->recycled = true;
        recycled_view_holder_.push_back(holder);
    }

    void ViewRecycler::RecycleViewHolder(View *item_view) {
        for (auto it = visible_view_holder_.begin();
            it != visible_view_holder_.end(); ++it)
        {
            ListAdapter::ViewHolder *holder = *it;
            if (holder->item_view == item_view)
            {
                holder->recycled = true;
                recycled_view_holder_.push_back(holder);
                visible_view_holder_.erase(it);
                parent_->removeView(item_view, false);
                return;
            }
        }
    }

    void ViewRecycler::RecycleViewHolders(size_t start_pos) {
        size_t index = 0;
        for (auto it = visible_view_holder_.begin();
            it != visible_view_holder_.end();)
        {
            if (index >= start_pos)
            {
                ListAdapter::ViewHolder *holder = *it;
                holder->recycled = true;
                recycled_view_holder_.push_back(holder);
                it = visible_view_holder_.erase(it);
                parent_->removeView(holder->item_view, false);
            }
            else
                ++it;

            ++index;
        }
    }

    void ViewRecycler::RecycleViewHolders(size_t start_pos, size_t length) {
        if (length == 0)
            return;

        size_t index = 0;
        size_t length_index = 0;
        for (auto it = visible_view_holder_.begin();
            it != visible_view_holder_.end();)
        {
            if (index >= start_pos)
            {
                ListAdapter::ViewHolder *holder = *it;
                holder->recycled = true;
                recycled_view_holder_.push_back(holder);
                it = visible_view_holder_.erase(it);
                parent_->removeView(holder->item_view, false);

                ++length_index;
                if (length_index == length)
                    break;
            }
            else
                ++it;

            ++index;
        }
    }

    ListAdapter::ViewHolder* ViewRecycler::ReuseViewHolder() {
        if (recycled_view_holder_.empty())
            return nullptr;

        ListAdapter::ViewHolder *holder = recycled_view_holder_.back();
        recycled_view_holder_.pop_back();

        AddViewHolder(holder);

        return holder;
    }

    ListAdapter::ViewHolder* ViewRecycler::ReuseViewHolder(size_t pos) {
        if (recycled_view_holder_.empty())
            return nullptr;

        ListAdapter::ViewHolder *holder = recycled_view_holder_.back();
        recycled_view_holder_.pop_back();

        AddViewHolder(holder, pos);

        return holder;
    }

    ListAdapter::ViewHolder* ViewRecycler::GetVisibleViewHolder(size_t pos)
    {
        size_t index = 0;
        for (auto it = visible_view_holder_.begin();
            it != visible_view_holder_.end(); ++it)
        {
            if (index == pos)
                return *it;
            ++index;
        }

        return NULL;
    }

    size_t ViewRecycler::Size()
    {
        return visible_view_holder_.size();
    }

    void ViewRecycler::ClearAll()
    {
        for (auto it = visible_view_holder_.begin();
            it != visible_view_holder_.end(); ++it) {
            delete *it;
        }

        for (auto it = recycled_view_holder_.begin();
            it != recycled_view_holder_.end(); ++it) {
            delete *it;
        }

        visible_view_holder_.clear();
        recycled_view_holder_.clear();
    }

    //////////////////////////////////////////////////////////////////
    // ListView

    ListView::ListView(Window *wnd)
        :ViewGroup(wnd),
        cur_position_(0),
        cur_offset_in_position_(0)
    {
        initListView();
    }

    ListView::ListView(Window *wnd, int id)
        : ViewGroup(wnd, id),
        cur_position_(0),
        cur_offset_in_position_(0)
    {
        initListView();
    }

    ListView::~ListView()
    {
    }

    void ListView::initListView()
    {
        list_adapter_ = nullptr;
        view_recycler_ = new ViewRecycler(this);
        initial_layouted_ = false;
    }

    void ListView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom) {
        if (!initial_layouted_ || sizeChanged) {
            RecordCurPositionAndOffset();
            LocateToPosition(cur_position_, cur_offset_in_position_);
        }
    }

    bool ListView::onInputEvent(InputEvent *e) {
        switch (e->getEvent()) {
        case InputEvent::EVM_WHEEL:
            int resDy = DetermineVerticalScroll(e->getMouseWheel()*40);
            if (resDy == 0)
                break;
            OffsetChildViewTopAndBottom(resDy);
            break;
        }

        return ViewGroup::onInputEvent(e);
    }


    void ListView::setAdapter(ListAdapter *adapter)
    {
        if (list_adapter_.get() == adapter)
            return;

        if (list_adapter_) {
            view_recycler_->ClearAll();
            removeAllViews();

            list_adapter_->setListener(nullptr);
        }

        list_adapter_.reset(adapter);
        list_adapter_->setListener(this);

        LocateToPosition(0);
        invalidate();
    }


    void ListView::ScrollToPosition(size_t position, int offset, bool smooth) {
        if (smooth)
            SmoothScrollToPosition(position, offset);
        else
            ScrollToPosition(position, offset);
    }

    int ListView::DetermineVerticalScroll(int dy) {
        int resDeltaY = 0;
        if (dy > 0) {  //向上滚动
            resDeltaY = FillTopChildViews(dy);
            RecycleBottomViews(resDeltaY);
        }
        else if (dy < 0) {  //向下滚动
            resDeltaY = FillBottomChildViews(dy);
            RecycleTopViews(resDeltaY);
        }

        return resDeltaY;
    }

    void ListView::OffsetChildViewTopAndBottom(int dy)
    {
        int size = getChildCount();
        for (int i = 0; i < size; ++i)
        {
            View *child = getChildAt(i);
            child->offsetTopAndBottom(dy);
        }
    }

    ListAdapter::ViewHolder* ListView::GetFirstVisibleViewHolder() {
        size_t size = view_recycler_->Size();
        for (size_t i = 0; i < size; ++i) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(i);
            View *item = holder->item_view;
            if (item->getBottom() > getContentBoundsInThis().top)
                return holder;
        }

        return nullptr;
    }

    ListAdapter::ViewHolder* ListView::GetLastVisibleViewHolder() {
        size_t size = view_recycler_->Size();
        for (size_t i = size; i > 0; --i) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(i - 1);
            View *item = holder->item_view;
            if (item->getTop() < getContentBoundsInThis().bottom)
                return holder;
        }

        return nullptr;
    }

    void ListView::RecycleTopViews(int offset) {
        size_t length = 0;
        size_t start_pos = 0;
        size_t size = view_recycler_->Size();

        for (size_t i = 0; i < size; ++i) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(i);
            View *item = holder->item_view;
            if (item->getBottom() + offset > getContentBoundsInThis().top)
            {
                length = i;
                break;
            }
        }

        if (length > 0) {
            view_recycler_->RecycleViewHolders(start_pos, length);
        }
    }

    void ListView::RecycleBottomViews(int offset) {
        size_t start_pos = 0;
        size_t size = view_recycler_->Size();

        for (size_t i = size; i > 0; --i) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(i - 1);
            View *item = holder->item_view;
            if (item->getTop() + offset < getContentBoundsInThis().bottom)
            {
                start_pos = i;
                break;
            }
        }

        if (start_pos + 1 <= size)
            view_recycler_->RecycleViewHolders(start_pos);
    }

    void ListView::RecordCurPositionAndOffset() {
        ListAdapter::ViewHolder *holder = GetFirstVisibleViewHolder();
        if (holder) {
            cur_position_ = holder->adapter_position;
            cur_offset_in_position_ = getContentBoundsInThis().top - holder->item_view->getTop();
        }
        else {
            cur_position_ = 0;
            cur_offset_in_position_ = 0;
        }
    }

    int ListView::FillTopChildViews(int dy) {
        int resDeltaY = 0;
        ListAdapter::ViewHolder *top_holder = view_recycler_->GetVisibleViewHolder(0);
        if (top_holder) {
            Rect content_bound = getContentBoundsInThis();
            int top_diff = content_bound.top - top_holder->item_view->getTop();
            if (top_diff > 0 && top_diff >= dy)
                resDeltaY = dy;
            else if (top_diff > 0 && top_holder->adapter_position == 0)
                resDeltaY = top_diff;
            else {
                int cur_top = top_holder->item_view->getTop();
                int total_height = 0;
                size_t cur_adapter_position = top_holder->adapter_position;

                while (total_height + top_diff < dy
                    && cur_adapter_position > 0)
                {
                    --cur_adapter_position;
                    ListAdapter::ViewHolder *new_holder = view_recycler_->ReuseViewHolder(0);
                    if (new_holder == nullptr) {
                        new_holder = list_adapter_->onCreateViewHolder(this, cur_adapter_position);
                        view_recycler_->AddViewHolder(new_holder, 0);
                    }

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
                    if (total_height + top_diff < dy)
                        resDeltaY = total_height + top_diff;
                    else
                        resDeltaY = dy;
                }
            }
        }

        return resDeltaY;
    }

    int ListView::FillBottomChildViews(int dy) {
        int resDeltaY = 0;
        size_t vis_view_count = view_recycler_->Size();
        if (vis_view_count == 0)
            return 0;

        ListAdapter::ViewHolder *bottom_holder = view_recycler_->GetVisibleViewHolder(vis_view_count - 1);
        if (bottom_holder) {
            Rect content_bound = getContentBoundsInThis();
            int bottom_diff = bottom_holder->item_view->getBottom() - content_bound.bottom;
            if (bottom_diff > 0 && -bottom_diff <= dy)
                resDeltaY = dy;
            else if (bottom_diff > 0 && bottom_holder->adapter_position + 1 == list_adapter_->getItemCount())
                resDeltaY = -bottom_diff;
            else {
                int cur_bottom = bottom_holder->item_view->getBottom();
                int total_height = 0;
                size_t cur_adapter_position = bottom_holder->adapter_position;

                while (-(bottom_diff + total_height) > dy
                    && cur_adapter_position + 1 < list_adapter_->getItemCount())
                {
                    ++cur_adapter_position;
                    ListAdapter::ViewHolder *new_holder = view_recycler_->ReuseViewHolder();
                    if (new_holder == nullptr)
                    {
                        new_holder = list_adapter_->onCreateViewHolder(this, cur_adapter_position);
                        view_recycler_->AddViewHolder(new_holder);
                    }

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
                    if (-(bottom_diff + total_height) > dy)
                        resDeltaY = -(bottom_diff + total_height);
                    else
                        resDeltaY = dy;
                }
            }
        }

        return resDeltaY;
    }

    void ListView::LocateToPosition(size_t position, int offset) {
        if (!list_adapter_)
            return;

        Rect content_bound = getContentBoundsInThis();
        if (content_bound.empty())
            return;

        initial_layouted_ = true;

        // scroll_animator_->Stop();

        size_t i = position;
        size_t index = 0;
        size_t child_count = list_adapter_->getItemCount();

        int total_height = 0;
        size_t overflow_index = 0;
        size_t overflow_count = 2;
        bool full_child_reached = false;

        for (; i < child_count; ++i, ++index) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(index);
            if (holder == nullptr) {
                holder = view_recycler_->ReuseViewHolder();
                if (holder == nullptr) {
                    holder = list_adapter_->onCreateViewHolder(this, i);
                    view_recycler_->AddViewHolder(holder);
                }
            }

            holder->adapter_position = i;
            list_adapter_->onBindViewHolder(holder, i);

            holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
            int height = holder->item_view->getMeasuredHeight();
            int left = content_bound.left;
            int top = content_bound.top + total_height - offset;
            holder->item_view->layout(left, top, left + content_bound.width(), top + height);

            total_height += height;
            if (total_height >= content_bound.height() + offset) {
                if (full_child_reached)
                    ++overflow_index;
                else
                    full_child_reached = true;

                if (overflow_index >= overflow_count) {
                    ++index;
                    break;
                }
            }
        }

        ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(index - overflow_index);
        if (holder) {
            view_recycler_->RecycleViewHolders(index - overflow_index);
        }

        //防止在列表大小变化时项目超出滑动范围。
        ListAdapter::ViewHolder *last_holder = GetLastVisibleViewHolder();
        ListAdapter::ViewHolder *first_holder = GetFirstVisibleViewHolder();
        if (last_holder && first_holder
            && last_holder->adapter_position + 1 == child_count) {
            int topDy = content_bound.top - first_holder->item_view->getBounds().top;
            if (topDy >= 0)
            {
                int bottomDy = content_bound.bottom - last_holder->item_view->getBounds().bottom;
                if (bottomDy > 0) {
                    int resDy = DetermineVerticalScroll(bottomDy);
                    if (resDy != 0)
                        OffsetChildViewTopAndBottom(resDy);
                }
            }
        }
    }

    void ListView::ScrollToPosition(size_t position, int offset) {
        if (!list_adapter_)
            return;

        Rect content_bound = getContentBoundsInThis();
        if (content_bound.empty())
            return;

        size_t child_count = list_adapter_->getItemCount();
        if (position + 1 > child_count) {
            if (child_count > 0)
                position = child_count - 1;
            else
                position = 0;
            offset = 0;
        }

        // scroll_animator_->Stop();

        size_t i = position;
        size_t index = 0;

        int total_height = 0;
        int diff = 0;
        bool full_child_reached = false;

        for (; i < child_count; ++i, ++index) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(index);
            if (holder == nullptr) {
                holder = view_recycler_->ReuseViewHolder();
                if (holder == nullptr) {
                    holder = list_adapter_->onCreateViewHolder(this, i);
                    view_recycler_->AddViewHolder(holder);
                }
            }

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

        ListAdapter::ViewHolder *start_holder = view_recycler_->GetVisibleViewHolder(index);
        if (start_holder)
            view_recycler_->RecycleViewHolders(index);

        if (!full_child_reached && child_count > 0 && diff > 0) {
            int resDiff = FillTopChildViews(diff);
            OffsetChildViewTopAndBottom(resDiff);
        }
    }

    void ListView::SmoothScrollToPosition(size_t position, int offset) {
        if (!list_adapter_)
            return;

        Rect content_bound = getContentBoundsInThis();
        if (content_bound.empty())
            return;

        size_t child_count = list_adapter_->getItemCount();
        if (child_count == 0)
            return;
        if (position + 1 > child_count) {
            position = child_count - 1;
            offset = 0;
        }

        // scroll_animator_->Stop();

        RecordCurPositionAndOffset();

        size_t start_pos = cur_position_;
        int start_pos_offset = cur_offset_in_position_;
        size_t terminate_pos = position;
        int terminate_pos_offset = offset;
        bool front = (start_pos <= terminate_pos);

        size_t i = start_pos;
        size_t index = 0;

        int height = 0;
        int total_height = 0;
        bool full_child_reached = false;

        for (; (front ? (i <= terminate_pos) : (i >= terminate_pos)); (front ? ++i : --i), ++index) {
            ListAdapter::ViewHolder *holder = view_recycler_->GetVisibleViewHolder(index);
            if (holder == nullptr) {
                holder = view_recycler_->ReuseViewHolder();
                if (holder == nullptr) {
                    holder = list_adapter_->onCreateViewHolder(this, i);
                    view_recycler_->AddViewHolder(holder);
                }
            }

            holder->adapter_position = i;
            list_adapter_->onBindViewHolder(holder, i);

            holder->item_view->measure(content_bound.width(), 0, EXACTLY, UNKNOWN);
            int left = content_bound.left;
            int top = total_height + content_bound.top + (front ? -offset : offset);
            height = holder->item_view->getMeasuredHeight();
            holder->item_view->layout(left, top, left + content_bound.width(), top + height);

            if (front) {
                if (i == terminate_pos)
                    height = terminate_pos_offset;
                if (i == start_pos)
                    height -= start_pos_offset;
                if (i != start_pos && i != terminate_pos)
                    height = front ? height : -height;
            }
            else {
                if (i == start_pos)
                    height = start_pos_offset;
                if (i == terminate_pos)
                    height -= terminate_pos_offset;
                if (i != start_pos && i != terminate_pos)
                    height = front ? height : -height;
            }

            total_height += height;
        }

        if (total_height != 0) {
            // scroll_animator_->Stop();
            // scroll_animator_->StartUniform(0, -total_height, 0, 500);
        }
    }

    //////////////////////////////////////////////////////////////////
    // ScrollDelegate implementation:

    /*void ListView::OnScroll(float dx, float dy) {
        int resDy = DetermineVerticalScroll(static_cast<int>(dy));
        if (resDy == 0)
            return;
        OffsetChildViewTopAndBottom(resDy);
        invalidate();
    }*/

    //////////////////////////////////////////////////////////////////
    // ListDataSetListener implementation:

    void ListView::OnDataSetChanged() {
        RecordCurPositionAndOffset();
        ScrollToPosition(cur_position_, cur_offset_in_position_);
        invalidate();
    }

    void ListView::OnItemRangeInserted(size_t start_position, size_t length) {
        if (length > 0) {
            AddOp(OpType::INSERT, start_position, length);
            //TODO: post insert op.
        }
    }

    void ListView::OnItemRangeChanged(size_t start_position, size_t length) {
        if (length > 0) {
            AddOp(OpType::CHANGE, start_position, length);
            //TODO: post change op.
        }
    }

    void ListView::OnItemRangeRemoved(size_t start_position, size_t length) {
        if (length > 0) {
            AddOp(OpType::REMOVE, start_position, length);
            //TODO: post remove op.
        }
    }

    void ListView::AddOp(OpType op, size_t start_position, size_t length) {
        ListAdapter::ViewHolder *th = view_recycler_->GetVisibleViewHolder(0);
        if (th) {
            if (start_position + length <= th->adapter_position) {
                return;
            }
        }

        Operation operation;
        operation.op = op;
        operation.start_position = start_position;
        operation.length = length;
        op_list_.push_back(operation);
    }

    void ListView::ProcessOp() {
        std::map<size_t, OpType> op_map;

        for (auto it = op_list_.begin();
            it != op_list_.end(); ++it) {
            Operation operation = *it;

            for (auto i = 0; i < operation.length; ++i) {
                op_map[operation.start_position + i] = operation.op;
            }

            switch (operation.op) {
            case INSERT:
                break;
            case CHANGE:
                break;
            case REMOVE:
                break;
            }
        }
    }

}