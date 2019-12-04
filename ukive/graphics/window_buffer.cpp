#include "ukive/graphics/window_buffer.h"

#include "utils/log.h"

#include "ukive/application.h"
#include "ukive/system/win10_version.h"
#include "ukive/window/window.h"
#include "ukive/window/window_impl.h"
#include "ukive/graphics/bitmap_factory.h"


namespace ukive {

    WindowBuffer::WindowBuffer(Window* w)
        : window_(w) {}

    bool WindowBuffer::onCreate(bool hw_acc) {
        is_hardware_acc_ = hw_acc;
        is_layered_ = window_->isTranslucent();

        bool ret;
        if (is_layered_) {
            if (is_hardware_acc_) {
                ret = createHardwareBRT();
            } else {
                ret = createSoftwareBRT();
            }
        } else {
            ret = createSwapchainBRT();
        }
        return ret;
    }

    bool WindowBuffer::onResize(int width, int height) {
        bool ret;
        if (is_layered_) {
            if (is_hardware_acc_) {
                ret = resizeHardwareBRT();
            } else {
                ret = resizeSoftwareBRT();
            }
        } else {
            ret = resizeSwapchainBRT();
        }
        return ret;
    }

    void WindowBuffer::onDestroy() {
        rt_.reset();
        swapchain_.reset();
    }

    void WindowBuffer::onBeginDraw() {
        rt_->BeginDraw();
    }

    bool WindowBuffer::onEndDraw() {
        bool ret = false;
        if (is_layered_) {
            ret = drawLayered();
        }

        HRESULT hr = rt_->EndDraw();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to draw d2d content.";
            return false;
        }

        if (!is_layered_) {
            ret = drawSwapchain();
        }
        return ret;
    }

    std::shared_ptr<Bitmap> WindowBuffer::onExtractBitmap() {
        return {};
    }

    bool WindowBuffer::createHardwareBRT() {
        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = window_->getWidth();
        tex_desc.Height = window_->getHeight();
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

        ComPtr<ID3D11Texture2D> d3d_texture;
        HRESULT hr = Application::getGraphicDeviceManager()->getD3DDevice()->
            CreateTexture2D(&tex_desc, nullptr, &d3d_texture);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create tex2d.";
            return {};
        }

        auto dxgi_surface = d3d_texture.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to cast tex2d to dxgi surface.";
            return {};
        }

        rt_ = Application::getGraphicDeviceManager()->createDXGIRenderTarget(dxgi_surface.get(), true);
        return rt_ != nullptr;
    }

    bool WindowBuffer::createSoftwareBRT() {
        auto wic_bmp = BitmapFactory::create(
            window_->getWidth(),
            window_->getHeight());
        if (!wic_bmp.getNative()) {
            LOG(Log::WARNING) << "Failed to create wic bitmap.";
            return {};
        }

        rt_ = Application::getGraphicDeviceManager()->createWICRenderTarget(wic_bmp.getNative().get());
        return rt_ != nullptr;
    }

    bool WindowBuffer::createSwapchainBRT() {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.OutputWindow = window_->getImpl()->getHandle();
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        auto gdm = Application::getGraphicDeviceManager();

        HRESULT hr = gdm->getDXGIFactory()->CreateSwapChain(
            gdm->getD3DDevice().get(),
            &swapChainDesc, &swapchain_);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create swap chain: " << hr;
            return {};
        }

        ComPtr<IDXGISurface> back_buffer;
        hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to query DXGI surface: " << hr;
            return {};
        }

        rt_ = Application::getGraphicDeviceManager()->createDXGIRenderTarget(back_buffer.get(), false);
        return rt_ != nullptr;
    }

    bool WindowBuffer::resizeHardwareBRT() {
        if (window_->getWidth() <= 0 ||
            window_->getHeight() <= 0)
        {
            return true;
        }

        onDestroy();
        return createHardwareBRT();
    }

    bool WindowBuffer::resizeSoftwareBRT() {
        if (window_->getWidth() <= 0 ||
            window_->getHeight() <= 0)
        {
            return true;
        }

        onDestroy();
        return createSoftwareBRT();
    }

    bool WindowBuffer::resizeSwapchainBRT() {
        rt_.reset();

        // 在某些系统上，需要传入完整大小
        static bool need_total = win::isWin10Ver1703OrGreater();

        int width = window_->getClientWidth(need_total);
        int height = window_->getClientHeight(need_total);
        if (width <= 0 || height <= 0) {
            return true;
        }

        HRESULT hr = swapchain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to resize swap chain: " << hr;
            return false;
        }

        ComPtr<IDXGISurface> back_buffer;
        hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to query DXGI surface: " << hr;
            return false;
        }

        rt_ = Application::getGraphicDeviceManager()->createDXGIRenderTarget(back_buffer.get(), false);
        return rt_ != nullptr;
    }

    bool WindowBuffer::drawLayered() {
        auto gdi_rt = rt_.cast<ID2D1GdiInteropRenderTarget>();
        if (!gdi_rt) {
            LOG(Log::FATAL) << "Failed to cast ID2D1RenderTarget to GDI RT.";
            return false;
        }

        HDC hdc = nullptr;
        HRESULT hr = gdi_rt->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hdc);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to get DC: " << hr;
            return false;
        }

        RECT wr;
        ::GetWindowRect(window_->getImpl()->getHandle(), &wr);
        POINT zero = { 0, 0 };
        SIZE size = { wr.right - wr.left, wr.bottom - wr.top };
        POINT position = { wr.left, wr.top };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        BOOL ret = ::UpdateLayeredWindow(
            window_->getImpl()->getHandle(),
            nullptr, &position, &size, hdc, &zero,
            RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (ret == 0) {
            LOG(Log::ERR) << "Failed to update layered window: " << ::GetLastError();
        }

        RECT rect = {};
        hr = gdi_rt->ReleaseDC(&rect);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to release DC: " << hr;
            return false;
        }

        return true;
    }

    bool WindowBuffer::drawSwapchain() {
        HRESULT hr = swapchain_->Present(Application::isVSyncEnabled() ? 1 : 0, 0);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to present.";
            return false;
        }

        return true;
    }

}