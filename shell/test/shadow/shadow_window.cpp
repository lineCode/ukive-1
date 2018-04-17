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
#include "ukive/graphics/bitmap.h"
#include "ukive/graphics/direct3d/effects/shadow_effect.h"

#define RADIUS 4
#define BACKGROUND_SIZE 100


namespace shell {

    void ShadowWindow::onCreate() {
        Window::onCreate();

        setBackgroundColor(ukive::Color::White);

        ukive::Canvas canvas(BACKGROUND_SIZE, BACKGROUND_SIZE);
        canvas.beginDraw();
        canvas.clear();
        canvas.fillRect(ukive::RectF(0, 0, BACKGROUND_SIZE, BACKGROUND_SIZE), ukive::Color::Green400);
        //canvas.fillCircle(BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 4.f, ukive::Color::Blue200);
        canvas.endDraw();
        content_bmp_ = canvas.extractBitmap()->getNative();

        d3d_effect_ = new ukive::ShadowEffect();
        d3d_effect_->setRadius(RADIUS);
        d3d_effect_->setContent(canvas.getTexture().get());

        d3d_effect_->draw();

        shadow_bmp_ = d3d_effect_->getOutput(getRenderer()->getRenderTarget().get());

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
        ce_button_->setElevation(1);

        auto ce_button_lp = Rlp::Builder(dpToPx(100), dpToPx(50))
            .start(layout->getId()).top(layout->getId())
            .end(layout->getId()).bottom(layout->getId()).build();
        layout->addView(ce_button_, ce_button_lp);

        animator_ = new ukive::Animator(getAnimationManager());
        animator_->addVariable(0, RADIUS, 0, 1000);
        animator_->setOnValueChangedListener(0, this);
        //animator_->startTransition(0, ukive::Transition::linearTransition(4, 256));
    }

    void ShadowWindow::onDrawCanvas(ukive::Canvas* canvas) {
        Window::onDrawCanvas(canvas);

        canvas->save();
        canvas->translate(-RADIUS, -RADIUS);

        ukive::Bitmap s_bmp(shadow_bmp_);
        canvas->drawBitmap(100, 10, &s_bmp);

        canvas->restore();

        ukive::Bitmap c_bmp(content_bmp_);
        canvas->drawBitmap(100, 10, &c_bmp);
    }

    void ShadowWindow::onDestroy() {
        Window::onDestroy();

        animator_->stop();
        delete animator_;
    }

    bool ShadowWindow::onInputEvent(ukive::InputEvent* e) {
        return Window::onInputEvent(e);
    }

    void ShadowWindow::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard* storyboard,
        IUIAnimationVariable* variable,
        double newValue, double previousValue) {
    }

    void ShadowWindow::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard* storyboard,
        IUIAnimationVariable* variable,
        int newValue, int previousValue) {

        d3d_effect_->setRadius(newValue);
        d3d_effect_->draw();

        D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        shadow_bmp_ = d3d_effect_->getOutput(getRenderer()->getRenderTarget().get());
    }

}