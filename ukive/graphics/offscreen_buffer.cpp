#include "ukive/graphics/offscreen_buffer.h"

#include "utils/log.h"

#include "ukive/application.h"


namespace ukive {

    OffscreenBuffer::OffscreenBuffer(int width, int height)
        : width_(width), height_(height) {
    }

    bool OffscreenBuffer::onCreate(bool hw_acc) {
        return createHardwareBRT(width_, height_);
    }

    bool OffscreenBuffer::onResize(int width, int height) {
        width_ = width;
        height_ = height;
        return createHardwareBRT(width, height);
    }

    void OffscreenBuffer::onDestroy() {
        rt_.reset();
        d3d_tex2d_.reset();
    }

    void OffscreenBuffer::onBeginDraw() {
        rt_->BeginDraw();
    }

    bool OffscreenBuffer::onEndDraw() {
        HRESULT hr = rt_->EndDraw();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to draw d2d content.";
            return false;
        }
        return true;
    }

    std::shared_ptr<Bitmap> OffscreenBuffer::onExtractBitmap() {
        ComPtr<ID2D1Bitmap> bitmap;
        D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        HRESULT hr = rt_->CreateSharedBitmap(
            __uuidof(IDXGISurface), d3d_tex2d_.cast<IDXGISurface>().get(), &bmp_prop, &bitmap);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return {};
        }

        return std::make_shared<Bitmap>(bitmap);
    }

    ComPtr<ID3D11Texture2D> OffscreenBuffer::getTexture() {
        return d3d_tex2d_;
    }

    bool OffscreenBuffer::createHardwareBRT(int width, int height) {
        auto d3d_device = Application::getGraphicDeviceManager()->getD3DDevice();

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = 0;

        d3d_tex2d_.reset();
        HRESULT hr = d3d_device->CreateTexture2D(&tex_desc, nullptr, &d3d_tex2d_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return false;
        }

        auto dxgi_surface = d3d_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to query DXGI surface.";
            return false;
        }

        rt_ = Application::getGraphicDeviceManager()->createDXGIRenderTarget(dxgi_surface.get(), false);
        return rt_ != nullptr;
    }

}