#include "renderer.h"

#include <fstream>

#include "ukive/application.h"
#include "ukive/graphics/swapchain_resize_notifier.h"
#include "ukive/log.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/window/window.h"


namespace ukive {

    Renderer::Renderer()
        :is_layered_(false),
        is_hardware_acc_(true) {}

    Renderer::~Renderer() {
    }


    HRESULT Renderer::init(Window* window) {
        owner_window_ = window;
        d2d_dc_ = Application::getGraphicDeviceManager()->createD2DDeviceContext();

        return createRenderResource();
    }

    HRESULT Renderer::createRenderResource() {
        RH(d2d_dc_->CreateEffect(CLSID_D2D1Shadow, &shadow_effect_));
        RH(d2d_dc_->CreateEffect(CLSID_D2D12DAffineTransform, &affinetrans_effect_));

        HRESULT hr = S_OK;
        if (is_layered_) {
            if (is_hardware_acc_) {
                hr = createHardwareBRT();
            } else {
                hr = createSoftwareBRT();
            }
        } else {
            hr = createSwapchainBRT();
        }

        return hr;
    }

    void Renderer::releaseRenderResource() {
        bitmap_render_target_.reset();
        swapchain_.reset();
        shadow_effect_.reset();
        affinetrans_effect_.reset();
    }

    HRESULT Renderer::resize() {
        HRESULT hr = S_OK;
        if (is_layered_) {
            if (is_hardware_acc_) {
                hr = resizeHardwareBRT();
            } else {
                hr = resizeSoftwareBRT();
            }
        } else {
            hr = resizeSwapchainBRT();
        }
        return hr;
    }

    bool Renderer::render(Color bg_color, std::function<void()> callback) {
        d2d_dc_->BeginDraw();
        D2D1_COLOR_F color = {
            bg_color.r, bg_color.g, bg_color.b, bg_color.a };
        d2d_dc_->Clear(color);

        callback();

        HRESULT hr = S_OK;
        if (is_layered_) {
            hr = drawLayered();
            DCHECK(SUCCEEDED(hr));
        }

        hr = d2d_dc_->EndDraw();
        if (FAILED(hr)) {
            Log::e(L"Render", L"failed to draw d2d content.");
        }

        if (!is_layered_) {
            hr = drawSwapchain();
        }

        return !FAILED(hr);
    }

    void Renderer::close() {
        releaseRenderResource();
    }


    HRESULT Renderer::createHardwareBRT() {
        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = owner_window_->getWidth();
        tex_desc.Height = owner_window_->getHeight();
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

        ComPtr<ID3D11Texture2D> d3d_texture;
        HRESULT hr = Application::getGraphicDeviceManager()->getD3DDevice()->
            CreateTexture2D(&tex_desc, nullptr, &d3d_texture);
        DCHECK(SUCCEEDED(hr));

        auto dxgi_surface  = d3d_texture.cast<IDXGISurface>();
        DCHECK(dxgi_surface != nullptr);

        hr = createBitmapRenderTarget(dxgi_surface.get(), &bitmap_render_target_, true);
        DCHECK(SUCCEEDED(hr));

        d2d_dc_->SetTarget(bitmap_render_target_.get());

        return S_OK;
    }

    HRESULT Renderer::createSoftwareBRT() {
        auto wic_bmp = Application::getWICManager()->createBitmap(
            owner_window_->getClientWidth(),
            owner_window_->getClientHeight());

        HRESULT hr = createBitmapRenderTarget(wic_bmp.get(), &bitmap_render_target_, true);
        DCHECK(SUCCEEDED(hr));

        return S_OK;
    }

    HRESULT Renderer::createSwapchainBRT() {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        auto gdm = Application::getGraphicDeviceManager();

        RH(gdm->getDXGIFactory()->CreateSwapChainForHwnd(
            gdm->getD3DDevice().get(),
            owner_window_->getHandle(),
            &swapChainDesc, nullptr, nullptr, &swapchain_));

        ComPtr<IDXGISurface> back_buffer;
        RH(swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer)));
        RH(createBitmapRenderTarget(back_buffer.get(), &bitmap_render_target_));

        d2d_dc_->SetTarget(bitmap_render_target_.get());

        return S_OK;
    }

    HRESULT Renderer::resizeHardwareBRT() {
        d2d_dc_->SetTarget(nullptr);
        bitmap_render_target_.reset();

        for (auto notifier : sc_resize_notifier_list_) {
            notifier->onPreSwapChainResize();
        }

        RH(createHardwareBRT());

        for (auto notifier : sc_resize_notifier_list_) {
            notifier->onPostSwapChainResize();
        }

        return S_OK;
    }

    HRESULT Renderer::resizeSoftwareBRT() {
        d2d_dc_->SetTarget(nullptr);
        bitmap_render_target_.reset();
        return createSoftwareBRT();
    }

    HRESULT Renderer::resizeSwapchainBRT() {
        d2d_dc_->SetTarget(nullptr);
        bitmap_render_target_.reset();

        for (auto notifier : sc_resize_notifier_list_) {
            notifier->onPreSwapChainResize();
        }

        RH(swapchain_->ResizeBuffers(
            0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

        ComPtr<IDXGISurface> back_buffer;
        RH(swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer)));
        RH(createBitmapRenderTarget(back_buffer.get(), &bitmap_render_target_));

        d2d_dc_->SetTarget(bitmap_render_target_.get());

        for (auto notifier : sc_resize_notifier_list_) {
            notifier->onPostSwapChainResize();
        }

        return S_OK;
    }

    HRESULT Renderer::drawLayered() {
        auto gdi_rt = d2d_dc_.cast<ID2D1GdiInteropRenderTarget>();
        DCHECK(gdi_rt != nullptr);

        HDC hdc = NULL;
        HRESULT hr = gdi_rt->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hdc);
        DCHECK(SUCCEEDED(hr));

        RECT wr;
        ::GetWindowRect(owner_window_->getHandle(), &wr);
        POINT zero = { 0, 0 };
        SIZE size = { wr.right - wr.left, wr.bottom - wr.top };
        POINT position = { wr.left, wr.top };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        BOOL ret = ::UpdateLayeredWindow(owner_window_->getHandle(), NULL, &position, &size, hdc, &zero,
            RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (ret == 0) {
            int errorno = ::GetLastError();
            DCHECK(false);
        }

        RECT rect = {};
        hr = gdi_rt->ReleaseDC(&rect);
        DCHECK(SUCCEEDED(hr));

        return hr;
    }

    HRESULT Renderer::drawSwapchain() {
        HRESULT hr = swapchain_->Present(Application::isVSyncEnabled() ? 1 : 0, 0);
        if (FAILED(hr)) {
            Log::e(L"Render", L"failed to present.");
        }

        return hr;
    }


    HRESULT Renderer::drawShadow(float elevation, float alpha, ID2D1Bitmap* bitmap) {
        //在 Alpha 动画时，令阴影更快消退。
        float shadow_alpha;
        if (alpha == 0.f) {
            shadow_alpha = 0.f;
        } else if (alpha == 1.f) {
            shadow_alpha = .38f;
        } else {
            shadow_alpha = static_cast<float>(.38f * std::pow(2, 8 * (alpha - 1)) / 1.f);
        }

        shadow_effect_->SetInput(0, bitmap);
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, D2D1_SHADOW_OPTIMIZATION_BALANCED));
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, elevation));
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0, 0, 0, shadow_alpha)));

        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(0, 0);// elevation / 1.5f);
        affinetrans_effect_->SetInputEffect(0, shadow_effect_.get());
        RH(affinetrans_effect_->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix));

        d2d_dc_->DrawImage(affinetrans_effect_.get());

        return S_OK;
    }


    void Renderer::addSwapChainResizeNotifier(SwapChainResizeNotifier* notifier) {
        sc_resize_notifier_list_.push_back(notifier);
    }

    void Renderer::removeSwapChainResizeNotifier(SwapChainResizeNotifier* notifier) {
        for (auto it = sc_resize_notifier_list_.begin();
            it != sc_resize_notifier_list_.end();) {

            if ((*it) == notifier) {
                it = sc_resize_notifier_list_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void Renderer::removeAllSwapChainResizeNotifier() {
        sc_resize_notifier_list_.clear();
    }

    HRESULT Renderer::createBitmapRenderTarget(
        IWICBitmap* wic_bitmap, ID2D1Bitmap1** bitmap, bool gdi_compat) {

        D2D1_BITMAP_OPTIONS bmp_options
            = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        if (gdi_compat) {
            bmp_options |= D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE;
        }

        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                bmp_options,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        return d2d_dc_->CreateBitmapFromWicBitmap(wic_bitmap, bitmapProperties, bitmap);
    }

    HRESULT Renderer::createBitmapRenderTarget(
        IDXGISurface* dxgiSurface, ID2D1Bitmap1** bitmap, bool gdi_compat) {

        D2D1_BITMAP_OPTIONS bmp_options
            = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        if (gdi_compat) {
            bmp_options |= D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE;
        }

        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                bmp_options,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        return d2d_dc_->CreateBitmapFromDxgiSurface(dxgiSurface, bitmapProperties, bitmap);
    }

    HRESULT Renderer::createCompatBitmapRenderTarget(
        float width, float height, ID2D1BitmapRenderTarget** bRT) {

        ComPtr<ID2D1BitmapRenderTarget> bmpRenderTarget;
        RH(d2d_dc_->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height), bRT));

        return S_OK;
    }

    HRESULT Renderer::createDXGISurfaceRenderTarget(
        IDXGISurface* dxgiSurface, ID2D1RenderTarget** renderTarget) {

        HRESULT hr = S_OK;
        auto gdm = Application::getGraphicDeviceManager();

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96, 96, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

        return gdm->getD2DFactory()->CreateDxgiSurfaceRenderTarget(dxgiSurface, props, renderTarget);
    }

    HRESULT Renderer::createWindowRenderTarget(
        HWND handle, unsigned int width, unsigned int height, ID2D1HwndRenderTarget** renderTarget) {

        HRESULT hr = S_OK;
        auto gdm = Application::getGraphicDeviceManager();

        D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
        // Set the DPI to be the default system DPI to allow direct mapping
        // between image pixels and desktop pixels in different system DPI settings
        renderTargetProperties.dpiX = 96;
        renderTargetProperties.dpiY = 96;

        return gdm->getD2DFactory()->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(handle, D2D1::SizeU(width, height)),
            renderTarget);
    }

    ComPtr<ID2D1Effect> Renderer::getShadowEffect() {
        return shadow_effect_;
    }

    ComPtr<ID2D1Effect> Renderer::getAffineTransEffect() {
        return affinetrans_effect_;
    }

    ComPtr<IDXGISwapChain1> Renderer::getSwapChain() {
        return swapchain_;
    }

    ComPtr<ID2D1DeviceContext> Renderer::getD2DDeviceContext() {
        return d2d_dc_;
    }

    HRESULT Renderer::createTextFormat(
        const string16 fontFamilyName,
        float fontSize, string16 localeName,
        IDWriteTextFormat** textFormat)
    {
        return Application::getGraphicDeviceManager()->getDWriteFactory()->CreateTextFormat(
            fontFamilyName.c_str(), nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            localeName.c_str(),
            textFormat);
    }

    HRESULT Renderer::createTextLayout(
        const string16 text,
        IDWriteTextFormat* textFormat,
        float maxWidth, float maxHeight,
        IDWriteTextLayout** textLayout)
    {
        return Application::getGraphicDeviceManager()->getDWriteFactory()->CreateTextLayout(
            text.c_str(), text.length(), textFormat, maxWidth, maxHeight, textLayout);
    }

}