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

        hr = getRenderer()->getD2DDeviceContext()->CreateEffect(CLSID_ShadowEffect, &mEffect);
        DCHECK(SUCCEEDED(hr));

        mAnimator = new ukive::Animator(getAnimationManager());
        mAnimator->start();

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

        Rlp* de_button_lp = Rlp::Builder(200, 100)
            .start(layout->getId(), Rlp::START, 100).top(layout->getId())
            .bottom(layout->getId()).build();
        layout->addView(de_button, de_button_lp);

        ce_button_ = new ukive::Button(this);
        ce_button_->setText(L"Custom Effect");
        ce_button_->setTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        ce_button_->setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        ce_button_->setTextSize(16);
        ce_button_->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        ce_button_->setElevation(0);

        Rlp* ce_button_lp = Rlp::Builder(200, 100)
            .start(de_button->getId(), Rlp::END).top(layout->getId())
            .end(layout->getId()).bottom(layout->getId()).build();
        layout->addView(ce_button_, ce_button_lp);
    }

    void ShadowWindow::onDrawCanvas(ukive::Canvas *canvas) {
        Window::onDrawCanvas(canvas);

        /*getRenderer()->getD2DDeviceContext()->DrawImage(
            mEffect.get(), D2D1::Point2F(ce_button_->getLeft(), ce_button_->getTop()));*/
    }

    void ShadowWindow::onDestroy() {
        Window::onDestroy();

        mAnimator->stop();
        delete mAnimator;
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