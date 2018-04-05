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

        using Rlp = ukive::RestraintLayoutParams;

        auto layout = new ukive::RestraintLayout(this);
        layout->setLayoutParams(new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT));
        setContentView(layout);

        de_button_ = new ukive::Button(this);
        de_button_->setText(L"D2D Effect");
        de_button_->setTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        de_button_->setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        de_button_->setTextSize(14);
        de_button_->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        de_button_->setElevation(16);

        auto de_button_lp = Rlp::Builder(dpToPx(200), dpToPx(100))
            .start(layout->getId(), Rlp::START, dpToPx(100)).top(layout->getId())
            .bottom(layout->getId()).build();
        layout->addView(de_button_, de_button_lp);

        ce_button_ = new ukive::Button(this);
        ce_button_->setText(L"Custom Effect");
        ce_button_->setTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        ce_button_->setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        ce_button_->setTextSize(14);
        ce_button_->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        ce_button_->setElevation(0);

        auto ce_button_lp = Rlp::Builder(dpToPx(200), dpToPx(100))
            .start(de_button_->getId(), Rlp::END).top(layout->getId())
            .end(layout->getId()).bottom(layout->getId()).build();
        layout->addView(ce_button_, ce_button_lp);

        animator_ = new ukive::Animator(getAnimationManager());
        animator_->addVariable(0, 16, 0, 100);
        animator_->setOnValueChangedListener(0, this);
        animator_->startTransition(0, ukive::Transition::linearTransition(2, 256));
    }

    void ShadowWindow::onDrawCanvas(ukive::Canvas *canvas) {
        Window::onDrawCanvas(canvas);

        /*auto bounds = ce_button_->getBounds();
        effect_->SetValue(
            SHADOW_EFFECT_PROP_BOUNDS,
            D2D1::Vector4F(bounds.left, bounds.top, bounds.right, bounds.bottom));
        effect_->SetValue(
            SHADOW_EFFECT_PROP_OFFSET,
            D2D1::Vector2F(0, 0));
        effect_->SetValue(
            SHADOW_EFFECT_PROP_ALPHA, 0.36f);
        effect_->SetValue(
            SHADOW_EFFECT_PROP_ELEVATION, 16.f);
        effect_->SetValue(
            SHADOW_EFFECT_PROP_CORNER_RADIUS, dpToPx(2.f));*/

       //getRenderer()->getD2DDeviceContext()->DrawImage(effect_.get());
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

        de_button_->setElevation(newValue);
    }

    void ShadowWindow::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        int newValue, int previousValue) {
    }

}