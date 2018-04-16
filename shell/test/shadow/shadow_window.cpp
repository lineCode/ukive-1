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

#include "shadow_effect.h"
#include "ukive/graphics/direct3d/effects/shadow_effect.h"

#define RADIUS 128
#define BACKGROUND_SIZE 100
#define TEXTURE_SIZE (BACKGROUND_SIZE + RADIUS * 2)


namespace shell {

    void ShadowWindow::onCreate() {
        Window::onCreate();

        setBackgroundColor(ukive::Color::White);

        auto d2d_factory = ukive::Application::getGraphicDeviceManager()->getD2DFactory();
        HRESULT hr = ShadowEffect::Register(d2d_factory.get());
        DCHECK(SUCCEEDED(hr));

        hr = getRenderer()->getD2DDeviceContext()->CreateEffect(CLSID_ShadowEffect, &effect_);
        DCHECK(SUCCEEDED(hr));

        d3d_tex2d_ = ukive::Renderer::createTexture2D(TEXTURE_SIZE, TEXTURE_SIZE);

        auto dxgi_surface = d3d_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            DCHECK(false);
            LOG(ukive::Log::WARNING) << "Failed to query DXGI surface.";
            return;
        }

        auto d2d_rt = ukive::Renderer::createDXGIRenderTarget(dxgi_surface.get(), false);

        d2d_rt->BeginDraw();
        d2d_rt->Clear();
        ukive::Canvas canvas(d2d_rt);
        canvas.translate(RADIUS, RADIUS);
        canvas.fillRect(ukive::RectF(0, 0, BACKGROUND_SIZE, BACKGROUND_SIZE), ukive::Color::Red200);
        //canvas.fillCircle(BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 2.f, BACKGROUND_SIZE / 5.f, ukive::Color::Green200);
        hr = d2d_rt->EndDraw();
        DCHECK(SUCCEEDED(hr));

        d3d_effect_ = new ukive::ShadowEffect(RADIUS);
        d3d_effect_->setSize(TEXTURE_SIZE, TEXTURE_SIZE);
        d3d_effect_->setContent(d3d_tex2d_.get());

        d3d_effect_->draw();

        D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        auto d2d_dc = getRenderer()->getD2DDeviceContext();
        hr = d2d_dc->CreateSharedBitmap(
            __uuidof(IDXGISurface), d3d_effect_->getOutput().cast<IDXGISurface>().get(), &bmp_prop, &d3d_content_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(ukive::Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return;
        }


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
        de_button_->setElevation(0);

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
        //animator_->startTransition(0, ukive::Transition::linearTransition(2, 256));
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
            SHADOW_EFFECT_PROP_CORNER_RADIUS, dpToPx(2.f));

       getRenderer()->getD2DDeviceContext()->DrawImage(effect_.get());*/

        ukive::Bitmap bmp(d3d_content_);
        canvas->drawBitmap(100, 1, &bmp);
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