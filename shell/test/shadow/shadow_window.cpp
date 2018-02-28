#include "shadow_window.h"

#include "ukive/log.h"
#include "ukive/application.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/button.h"

#include "shadow_effect.h"


namespace shell {

    void ShadowWindow::onCreate() {
        Window::onCreate();

        setBackgroundColor(ukive::Color::White);

        auto d2d_factory = ukive::Application::getGraphicDeviceManager()->getD2DFactory();
        HRESULT hr = ShadowEffect::Register(d2d_factory.get());
        DCHECK(SUCCEEDED(hr));

        hr = getRenderer()->getD2DDeviceContext()->CreateEffect(CLSID_ShadowEffect, &effect_);
        DCHECK(SUCCEEDED(hr));

        animator_ = new ukive::Animator(getAnimationManager());
        animator_->start();

        using Rlp = ukive::RestraintLayoutParams;

        auto layout = new ukive::RestraintLayout(this);
        layout->setLayoutParams(new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT));
        setContentView(layout);

        auto de_button = new ukive::Button(this);
        de_button->setText(L"D2D Effect");
        de_button->setTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        de_button->setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        de_button->setTextSize(16);
        de_button->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        de_button->setElevation(0);

        auto de_button_lp = Rlp::Builder(dpToPx(200), dpToPx(100))
            .start(layout->getId(), Rlp::START, dpToPx(100)).top(layout->getId())
            .bottom(layout->getId()).build();
        layout->addView(de_button, de_button_lp);

        ce_button_ = new ukive::Button(this);
        ce_button_->setText(L"Custom Effect");
        ce_button_->setTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        ce_button_->setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        ce_button_->setTextSize(16);
        ce_button_->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        ce_button_->setElevation(0);

        auto ce_button_lp = Rlp::Builder(dpToPx(200), dpToPx(100))
            .start(de_button->getId(), Rlp::END).top(layout->getId())
            .end(layout->getId()).bottom(layout->getId()).build();
        layout->addView(ce_button_, ce_button_lp);
    }

    void ShadowWindow::onDrawCanvas(ukive::Canvas *canvas) {
        Window::onDrawCanvas(canvas);

        auto bounds = ce_button_->getBounds();
        effect_->SetValue(
            SHADOW_EFFECT_PROP_BOUNDS,
            D2D1::Vector4F(bounds.left, bounds.top, bounds.right, bounds.bottom));
        effect_->SetValue(
            SHADOW_EFFECT_PROP_OFFSET,
            D2D1::Vector2F(0, 4));
        effect_->SetValue(
            SHADOW_EFFECT_PROP_ALPHA, 0.38f);
        effect_->SetValue(
            SHADOW_EFFECT_PROP_ELEVATION, dpToPx(8));
        effect_->SetValue(
            SHADOW_EFFECT_PROP_CORNER_RADIUS, dpToPx(16.f));

        getRenderer()->getD2DDeviceContext()->DrawImage(effect_.get());
    }

    void ShadowWindow::onDestroy() {
        Window::onDestroy();

        animator_->stop();
        delete animator_;
    }

    bool ShadowWindow::onInputEvent(ukive::InputEvent *e) {
        return Window::onInputEvent(e);
    }

    void ShadowWindow::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        double newValue, double previousValue) {
    }

    void ShadowWindow::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        int newValue, int previousValue) {
    }

}