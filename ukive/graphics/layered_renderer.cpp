#include "layered_renderer.h"

#include "ukive/application.h"
#include "ukive/graphics/swapchain_resize_notifier.h"
#include "ukive/log.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/window/window.h"


namespace ukive {

    bool LayeredRenderer::init(Window* window) {
        owner_window_ = window;
        d2d_dc_ = Application::getGraphicDeviceManager()->createD2DDeviceContext();

        return createRenderResource();
    }

    bool LayeredRenderer::resize() {
        return false;
    }

    bool LayeredRenderer::render(
        Color bk_color,
        std::function<void()> callback) {

        d2d_dc_->BeginDraw();
        D2D1_COLOR_F color = {
            bk_color.r, bk_color.g, bk_color.b, bk_color.a, };
        d2d_dc_->Clear(color);

        callback();

        HRESULT hr = d2d_dc_->EndDraw();
        if (FAILED(hr)) {
            Log::e(L"Render", L"failed to draw d2d content.");
        }
        return false;
    }

    void LayeredRenderer::close() {
        releaseRenderResource();
    }

    bool LayeredRenderer::createRenderResource() {
        RB(d2d_dc_->CreateEffect(CLSID_D2D1Shadow, &shadow_effect_));
        RB(d2d_dc_->CreateEffect(CLSID_D2D12DAffineTransform, &affinetrans_effect_));

        return false;
    }

    void LayeredRenderer::releaseRenderResource() {
        shadow_effect_.reset();
        affinetrans_effect_.reset();
    }

}