#include "ukive/views/title_bar/title_bar.h"

#include <random>

#include "ukive/views/text_view.h"
#include "ukive/views/button.h"
#include "ukive/window/window.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/drawable/shape_drawable.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/drawable/layer_drawable.h"
#include "ukive/drawable/bitmap_drawable.h"
#include "ukive/graphics/bitmap_factory.h"


namespace ukive {

    TitleBar::TitleBar(Window* w)
        :TitleBar(w, {}) {}

    TitleBar::TitleBar(Window* w, AttrsRef attrs)
        : FrameLayout(w, attrs),
          title_tv_(nullptr)
    {
        auto color = Color::Blue800;
        color.a = 0.5f;

        std::vector<uint32_t> img_colors;

        std::random_device rd;
        std::default_random_engine en(rd());
        std::uniform_int_distribution<int> user_dist(192, 224);
        std::normal_distribution<float> norm_dist;

        for (int h = 0; h < 100; ++h) {
            for (int width = 0; width < 100; ++width) {
                int alpha = user_dist(en);
                //int alpha = 160 + norm_dist(en) * 32;
                img_colors.push_back((uint32_t(alpha) << 24) | 0x7F7F7F);
            }
        }
        auto img_data_ptr = reinterpret_cast<unsigned char*>(img_colors.data());
        auto bmp = ukive::BitmapFactory::create(w, 100, 100, img_data_ptr);

        auto bmp_drawable = new BitmapDrawable(bmp);
        bmp_drawable->setExtendMode(BitmapDrawable::Wrap);
        auto layer_drawable = new LayerDrawable();
        layer_drawable->addDrawable(bmp_drawable);
        layer_drawable->addDrawable(new ColorDrawable(color));

        setElevation(w->dpToPxX(4));
        setBackground(layer_drawable);
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
            .start(root_layout->getId(), Rlp::START, getWindow()->dpToPxX(12))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(title_tv_, title_tv_lp);

        int btn_padding = getWindow()->dpToPxX(8);

        close_btn_ = new Button(getWindow());
        close_btn_->setText(L"");
        close_btn_->setPadding(btn_padding, 0, btn_padding, 0);
        close_btn_->setOnClickListener(this);
        close_btn_->setButtonColor(Color::Red500);
        close_btn_->setButtonShape(ShapeDrawable::OVAL);
        auto close_btn_lp = Rlp::Builder()
            .end(root_layout->getId(), Rlp::END, getWindow()->dpToPxX(8))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(close_btn_, close_btn_lp);

        max_btn_ = new Button(getWindow());
        max_btn_->setText(L"");
        max_btn_->setOnClickListener(this);
        max_btn_->setPadding(btn_padding, 0, btn_padding, 0);
        max_btn_->setButtonColor(Color::Yellow500);
        max_btn_->setButtonShape(ShapeDrawable::OVAL);
        auto max_btn_lp = Rlp::Builder()
            .end(close_btn_->getId(), Rlp::START, getWindow()->dpToPxX(8))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(max_btn_, max_btn_lp);

        min_btn_ = new Button(getWindow());
        min_btn_->setText(L"");
        min_btn_->setOnClickListener(this);
        min_btn_->setPadding(btn_padding, 0, btn_padding, 0);
        min_btn_->setButtonColor(Color::Green500);
        min_btn_->setButtonShape(ShapeDrawable::OVAL);
        auto min_btn_lp = Rlp::Builder()
            .end(max_btn_->getId(), Rlp::START, getWindow()->dpToPxX(8))
            .top(root_layout->getId(), Rlp::TOP)
            .bottom(root_layout->getId(), Rlp::BOTTOM).build();
        root_layout->addView(min_btn_, min_btn_lp);
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
            //max_btn_->setText(L"Res");
        } else {
            //max_btn_->setText(L"Max");
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
