#include "ukive/views/spinner_view/spinner_view.h"

#include <algorithm>

#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/animation/animation_director.h"
#include "ukive/animation/view_animator.h"
#include "ukive/drawable/shape_drawable.h"
#include "ukive/event/input_event.h"
#include "ukive/window/window.h"
#include "ukive/utils/weak_bind.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/spinner_view/spinner_list_adapter.h"


namespace ukive {

    // SpinnerView
    SpinnerView::SpinnerView(Window* w)
        : SpinnerView(w, {}) {
    }

    SpinnerView::SpinnerView(Window* w, AttrsRef attrs)
        : ViewGroup(w, attrs),
          text_view_(nullptr),
          button_(nullptr),
          weak_ref_nest_(this)
    {
        initViews();
    }

    SpinnerView::~SpinnerView() {
    }

    void SpinnerView::initViews() {
        // TextView
        text_view_ = new TextView(getWindow());
        text_view_->setText(L"SpinnerView");
        auto tv_lp = new LinearLayoutParams(LayoutParams::FIT_CONTENT, LayoutParams::FIT_CONTENT);
        addView(text_view_, tv_lp);

        // DropdownButton
        button_ = new DropdownButton(getWindow());
        button_->setOnClickListener(this);
        auto btn_lp = new LinearLayoutParams(LayoutParams::FIT_CONTENT, LayoutParams::MATCH_PARENT);
        addView(button_, btn_lp);

        adapter_ = new SpinnerListAdapter();

        list_view_ = new ListView(getWindow());
        list_view_->setLayouter(new LinearListLayouter());
        list_view_->setAdapter(adapter_);
        list_view_->setItemSelectedListener(this);

        auto shapeDrawable = new ShapeDrawable(ShapeDrawable::RECT);
        shapeDrawable->setRadius(2.f);
        shapeDrawable->setSolidEnable(true);
        shapeDrawable->setSolidColor(Color::White);

        inner_window_ = std::make_shared<InnerWindow>(getWindow());
        inner_window_->setElevation(getWindow()->dpToPxX(2.f));
        inner_window_->setContentView(list_view_);
        inner_window_->setOutsideTouchable(false);
        inner_window_->setDismissByTouchOutside(true);
        inner_window_->setBackground(shapeDrawable);
        inner_window_->setHeight(getWindow()->dpToPxX(100));
        inner_window_->setEventListener(this);

        min_dropdown_width_ = getWindow()->dpToPxX(100);
    }

    void SpinnerView::addItem(const string16& title) {
        adapter_->addItem(title);
    }

    void SpinnerView::measureViews(int width, int height, int width_mode, int height_mode) {
        int padding_w = getPaddingLeft() + getPaddingRight();
        int padding_h = getPaddingTop() + getPaddingBottom();

        // DropdownButton
        auto db_lp = button_->getLayoutParams();

        int db_width, db_wm;
        int db_margin_w = db_lp->left_margin + db_lp->right_margin;
        getChildMeasure(width, width_mode, padding_w + db_margin_w, LayoutParams::FIT_CONTENT, &db_width, &db_wm);

        int db_height, db_hm;
        int db_margin_h = db_lp->top_margin + db_lp->bottom_margin;
        getChildMeasure(height, height_mode, padding_h + db_margin_h, LayoutParams::FIT_CONTENT, &db_height, &db_hm);

        button_->measure(db_width, db_height, db_wm, db_hm);

        // TextView
        auto tv_lp = text_view_->getLayoutParams();

        int tv_width, tv_wm;
        int tv_margin_w = tv_lp->left_margin + tv_lp->right_margin;
        getChildMeasure(
            width - button_->getMeasuredWidth() - db_margin_w, width_mode,
            padding_w + tv_margin_w, LayoutParams::MATCH_PARENT, &tv_width, &tv_wm);

        int tv_height, tv_hm;
        int tv_margin_h = tv_lp->top_margin + tv_lp->bottom_margin;
        getChildMeasure(height, height_mode, padding_h + tv_margin_h, LayoutParams::FIT_CONTENT, &tv_height, &tv_hm);

        text_view_->measure(tv_width, tv_height, tv_wm, tv_hm);
    }

    void SpinnerView::onMeasure(int width, int height, int width_mode, int height_mode) {
        measureViews(width, height, width_mode, height_mode);

        int f_width, f_height;
        int padding_w = getPaddingLeft() + getPaddingRight();
        int padding_h = getPaddingTop() + getPaddingBottom();

        auto db_lp = button_->getLayoutParams();
        int db_w = button_->getMeasuredWidth() + db_lp->left_margin + db_lp->right_margin;
        int db_h = button_->getMeasuredHeight() + db_lp->top_margin + db_lp->bottom_margin;

        auto tv_lp = text_view_->getLayoutParams();
        int tv_w = text_view_->getMeasuredWidth() + tv_lp->left_margin + tv_lp->right_margin;
        int tv_h = text_view_->getMeasuredHeight() + tv_lp->top_margin + tv_lp->bottom_margin;

        switch (width_mode) {
        case EXACTLY:
            f_width = width;
            break;

        case FIT:
            f_width = std::min(width, padding_w + db_w + tv_w);
            break;

        case UNKNOWN:
        default:
            f_width = padding_w + db_w + tv_w;
            break;
        }

        switch (height_mode) {
        case EXACTLY:
            f_height = height;
            break;

        case FIT:
            f_height = std::min(height, padding_h + std::max(tv_h, db_h));
            break;

        case UNKNOWN:
        default:
            f_height = padding_h + std::max(tv_h, db_h);
            break;
        }

        setMeasuredSize(f_width, f_height);
    }

    void SpinnerView::onLayout(bool changed, bool size_changed, int left, int top, int right, int bottom) {
        auto tv_lp = text_view_->getLayoutParams();
        int tv_h = text_view_->getMeasuredHeight() + tv_lp->top_margin + tv_lp->bottom_margin;
        int tv_x = tv_lp->left_margin + getPaddingLeft();

        auto db_lp = button_->getLayoutParams();
        int db_h = button_->getMeasuredHeight() + db_lp->top_margin + db_lp->bottom_margin;
        int db_x = tv_x + text_view_->getMeasuredWidth() + db_lp->left_margin;

        if (tv_h >= db_h) {
            int tv_y = tv_lp->top_margin + getPaddingTop();
            text_view_->layout(
                tv_x, tv_y,
                tv_x + text_view_->getMeasuredWidth(),
                tv_y + text_view_->getMeasuredHeight());

            int db_y = getPaddingTop() + (tv_h - db_h) / 2;
            button_->layout(
                db_x, db_y,
                db_x + button_->getMeasuredWidth(),
                db_y + db_lp->top_margin + button_->getMeasuredHeight());
        } else {
            int db_y = db_lp->top_margin + getPaddingTop();
            button_->layout(
                db_x, db_y,
                db_x + button_->getMeasuredWidth(),
                db_y + button_->getMeasuredHeight());

            int tv_y = getPaddingTop() + (db_h - tv_h) / 2;
            text_view_->layout(
                tv_x, tv_y,
                tv_x + text_view_->getMeasuredWidth(),
                tv_y + tv_lp->top_margin + text_view_->getMeasuredHeight());
        }
    }

    void SpinnerView::show(int x, int y, int width) {
        if (!is_finished_) {
            return;
        }

        is_finished_ = false;

        // 异步打开TextActionMode菜单，以防止在输入事件处理流程中
        // 打开菜单时出现问题。
        getWindow()->getCycler()->post(
            weakref_bind(&SpinnerView::showAsync, weak_ref_nest_.getRef(), x, y, width));
    }


    void SpinnerView::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        list_view_->setEnabled(false);
        inner_window_->markDismissing();

        // 异步关闭TextActionMode菜单，以防止在输入事件处理流程中
        // 关闭菜单时出现问题。
        std::weak_ptr<InnerWindow> ptr = inner_window_;
        inner_window_->getDecorView()->animate()->
            setDuration(100)->alpha(0.f)->setFinishedHandler(
                [ptr](AnimationDirector* director)
        {
            auto window = ptr.lock();
            if (window) {
                window->dismiss();
            }
        })->start();
    }

    void SpinnerView::showAsync(int x, int y, int width) {
        int center_x = 0, center_y = 0;
        int window_width = getWindow()->getClientWidth();
        if (x + width > window_width) {
            center_x = width;
            center_y = 0;
            x -= width;
        }

        int height = inner_window_->getHeight();

        inner_window_->dismiss();

        inner_window_->setWidth(width);
        inner_window_->show(x, y);
        inner_window_->getDecorView()->animate()->setDuration(150)->
            rectReveal(center_x, center_y, width, width, 0, height)->start();
        list_view_->setEnabled(true);

        auto last_input_view = getWindow()->getLastInputView();
        if (last_input_view) {
            InputEvent e;
            e.setEvent(InputEvent::EV_CANCEL);
            last_input_view->dispatchInputEvent(&e);
        }
    }

    void SpinnerView::onClick(View* v) {
        if (v == button_) {
            if (is_finished_) {
                auto bounds = getBoundsInWindow();
                auto width = std::max(bounds.width(), min_dropdown_width_);
                show(bounds.left, bounds.bottom, width);
            } else {
                close();
            }
        }
    }

    void SpinnerView::onRequestDismissByTouchOutside() {
        close();
    }

    void SpinnerView::onItemSelected(ListAdapter::ViewHolder* holder) {
        int pos = holder->adapter_position;
        if (pos >= 0 && pos < adapter_->getItemCount()) {
            text_view_->setText(adapter_->getItemData(pos));
            close();
        }
    }

}