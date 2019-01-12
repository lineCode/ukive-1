#include "ukive/views/title_bar/title_bar.h"

#include "ukive/views/text_view.h"
#include "ukive/views/button.h"
#include "ukive/window/window.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/drawable/shape_drawable.h"
#include "ukive/drawable/color_drawable.h"


namespace ukive {

    TitleBar::TitleBar(Window* w)
        : FrameLayout(w),
        title_tv_(nullptr)
    {
        setElevation(w->dpToPx(4));
        setBackground(new ColorDrawable(Color::Blue400));
        w->addStatusChangedListener(this);
        initViews();
    }

    TitleBar::~TitleBar() {
        getWindow()->removeStatusChangedListener(this);
    }

    void TitleBar::initViews() {
        using Rlp = RestraintLayoutParams;
        auto root_layout = new RestraintLayout(getWindow());
        root_layout->setLayoutParams(
            new RestraintLayoutParams(
                LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT));
        addView(root_layout);

        title_tv_ = new TextView(getWindow());
        title_tv_->setText(getWindow()->getTitle());
        title_tv_->setTextColor(Color::White);
        title_tv_->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        auto title_tv_lp = Rlp::Builder()
            .start(root_layout->getId(), Rlp::START, getWindow()->dpToPx(12))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(title_tv_, title_tv_lp);

        close_btn_ = new Button(getWindow());
        close_btn_->setText(L"Clo");
        close_btn_->setOnClickListener(this);
        //close_btn_->setBackground();
        auto close_btn_lp = Rlp::Builder()
            .end(root_layout->getId(), Rlp::END, getWindow()->dpToPx(8))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(close_btn_, close_btn_lp);

        max_btn_ = new Button(getWindow());
        max_btn_->setText(L"Max");
        max_btn_->setOnClickListener(this);
        auto max_btn_lp = Rlp::Builder()
            .end(close_btn_->getId(), Rlp::START, getWindow()->dpToPx(8))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(max_btn_, max_btn_lp);

        min_btn_ = new Button(getWindow());
        min_btn_->setText(L"Min");
        min_btn_->setOnClickListener(this);
        auto min_btn_lp = Rlp::Builder()
            .end(max_btn_->getId(), Rlp::START, getWindow()->dpToPx(8))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(min_btn_, min_btn_lp);
    }

    Drawable* TitleBar::getTitleButtonBackground() const {
        auto d = new ShapeDrawable(ShapeDrawable::Shape::OVAL);
        return d;
    }

    void TitleBar::onClick(View* v) {
        if (v == min_btn_) {
            getWindow()->minimize();
        } else if (v == max_btn_) {
            if (getWindow()->isMaximum()) {
                getWindow()->restore();
            } else {
                getWindow()->maximize();
            }
        } else if (v == close_btn_) {
            getWindow()->close();
        }
    }

    void TitleBar::onWindowTextChanged(const string16& text) {
        title_tv_->setText(text);
    }

    void TitleBar::onWindowIconChanged() {

    }

    void TitleBar::onWindowStatusChanged() {
        if (getWindow()->isMaximum()) {
            max_btn_->setText(L"Res");
        } else {
            max_btn_->setText(L"Max");
        }
    }

    HitPoint TitleBar::onNCHitTest(int x, int y) {
        if (min_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        if (max_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        if (close_btn_->getBounds().hit(x, y)) {
            return HitPoint::CLIENT;
        }
        return HitPoint::CAPTION;
    }

}
