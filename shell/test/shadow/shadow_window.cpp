#include "shadow_window.h"

#include "utils/log.h"

#include "ukive/application.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/button.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/graphics/direct3d/effects/shadow_effect.h"
#include "ukive/animation/interpolator.h"

#define RADIUS 4
#define BACKGROUND_SIZE 100


namespace shell {

    ShadowWindow::ShadowWindow()
        : ce_button_(nullptr),
          d3d_effect_(nullptr) {}

    void ShadowWindow::onCreate() {
        Window::onCreate();

        setBackgroundColor(ukive::Color::White);

        ukive::Canvas canvas(BACKGROUND_SIZE, BACKGROUND_SIZE);
        canvas.beginDraw();
        canvas.clear();
        canvas.fillRect(ukive::RectF(0, 0, BACKGROUND_SIZE, BACKGROUND_SIZE), ukive::Color::Green400);
        //canvas.fillCircle(BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 4.f, ukive::Color::Blue200);
        canvas.endDraw();
        content_bmp_ = canvas.extractBitmap();

        d3d_effect_ = new ukive::ShadowEffect();
        d3d_effect_->setRadius(RADIUS);
        d3d_effect_->setContent(canvas.getTexture().get());

        d3d_effect_->draw();

        shadow_bmp_ = d3d_effect_->getOutput(getCanvas()->getRT());

        using Rlp = ukive::RestraintLayoutParams;

        auto layout = new ukive::RestraintLayout(this);
        layout->setLayoutParams(new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT));
        setContentView(layout);

        ce_button_ = new ukive::Button(this);
        ce_button_->setText(L"Shadow Effect");
        ce_button_->setTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        ce_button_->setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        ce_button_->setTextSize(12);
        ce_button_->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        ce_button_->setElevation(18);

        auto ce_button_lp = Rlp::Builder(dpToPxX(100), dpToPxX(50))
            .start(layout->getId()).top(layout->getId())
            .end(layout->getId()).bottom(layout->getId()).build();
        layout->addView(ce_button_, ce_button_lp);

        animator_.setInitValue(RADIUS);
        animator_.setListener(this);
        animator_.setDuration(4000);
        animator_.setInterpolator(new ukive::LinearInterpolator(256));
        //animator_.start();
    }

    void ShadowWindow::onPreDrawCanvas(ukive::Canvas* canvas) {
        animator_.update();

        Window::onPreDrawCanvas(canvas);

        canvas->save();
        canvas->translate(-RADIUS, -RADIUS);

        canvas->drawBitmap(100, 10, shadow_bmp_.get());

        canvas->restore();

        canvas->drawBitmap(100, 10, content_bmp_.get());
    }

    void ShadowWindow::onDestroy() {
        animator_.stop();

        Window::onDestroy();
    }

    bool ShadowWindow::onInputEvent(ukive::InputEvent* e) {
        return Window::onInputEvent(e);
    }

    void ShadowWindow::onAnimationProgress(ukive::Animator* animator) {
        d3d_effect_->setRadius(animator->getCurValue());
        d3d_effect_->draw();

        shadow_bmp_ = d3d_effect_->getOutput(getCanvas()->getRT());
        invalidate();
    }

}