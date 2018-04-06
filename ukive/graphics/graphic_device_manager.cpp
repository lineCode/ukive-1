#include "graphic_device_manager.h"

#include "ukive/log.h"


namespace ukive {

    void GraphicDeviceManager::init() {
        initDXPersistance();
        initDXDevice();
    }

    void GraphicDeviceManager::shutdown() {
        shutdownDXPersistance();
        shutdownDXDevice();
    }


    void GraphicDeviceManager::EnumSystemFonts() {
        ComPtr<IDWriteFontCollection> collection;
        HRESULT hr = dwrite_factory_->GetSystemFontCollection(&collection);
        if (SUCCEEDED(hr)) {
            auto count = collection->GetFontFamilyCount();
            for (UINT32 i = 0; i < count; ++i) {
                ComPtr<IDWriteFontFamily> family;
                hr = collection->GetFontFamily(i, &family);
                if (SUCCEEDED(hr)) {
                    ComPtr<IDWriteLocalizedStrings> strings;
                    hr = family->GetFamilyNames(&strings);
                    if (SUCCEEDED(hr)) {
                        auto font_count = strings->GetCount();
                        for (UINT32 j = 0; j < font_count; ++j) {
                            UINT32 length;
                            hr = strings->GetStringLength(j, &length);
                            if (SUCCEEDED(hr)) {
                                WCHAR* buffer = new WCHAR[length + 1];
                                hr = strings->GetString(j, buffer, length + 1);
                                if (SUCCEEDED(hr)) {
                                    ::OutputDebugStringW(buffer);
                                    ::OutputDebugStringW(L"\n");
                                }

                                delete[] buffer;
                            }
                        }
                    }
                }
            }
        }
    }


    void GraphicDeviceManager::initDXPersistance() {
        HRESULT hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &d2d_factory_);
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create d2d factory.");
            return;
        }

        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&dwrite_factory_));
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create dwrite factory.");
            return;
        }

        hr = CreateDXGIFactory(
            __uuidof(IDXGIFactory2),
            reinterpret_cast<void**>(&dxgi_factory_));
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create dxgi factory.");
            return;
        }

        UINT adapterIndex = 0;
        HRESULT adapterResult = S_OK;
        ComPtr<IDXGIAdapter> adapter;

        while (SUCCEEDED(adapterResult)) {
            adapterResult = dxgi_factory_->EnumAdapters(adapterIndex, &adapter);
            if (adapter) {
                UINT outputIndex = 0;
                HRESULT outputResult = S_OK;
                ComPtr<IDXGIOutput> output;

                while (SUCCEEDED(outputResult)) {
                    outputResult = adapter->EnumOutputs(outputIndex, &output);
                    if (output) {
                        cur_output_ = output;
                        cur_adapter_ = adapter;

                        ++outputIndex;
                        output.reset();
                    }
                }

                ++adapterIndex;
                adapter.reset();
            }
        }
    }

    void GraphicDeviceManager::shutdownDXPersistance() {
        cur_output_.reset();
        cur_adapter_.reset();

        d2d_factory_.reset();
        dwrite_factory_.reset();
        dxgi_factory_.reset();
    }

    void GraphicDeviceManager::initDXDevice() {
        D3D_FEATURE_LEVEL featureLevel[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };

        HRESULT hr = ::D3D11CreateDevice(0,
            D3D_DRIVER_TYPE_HARDWARE,
            0, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            featureLevel, 4, D3D11_SDK_VERSION,
            &d3d_device_, 0, &d3d_devicecontext_);
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create d3d device.");
            return;
        }

        hr = d3d_device_->QueryInterface(
            __uuidof(IDXGIDevice2),
            reinterpret_cast<void**>(&dxgi_device_));
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create dxgi device.");
            return;
        }

        hr = d2d_factory_->CreateDevice(
            dxgi_device_.get(), &d2d_device_);
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create d2d device.");
            return;
        }
    }

    void GraphicDeviceManager::shutdownDXDevice() {
        d2d_device_.reset();

        d3d_devicecontext_.reset();
        d3d_device_.reset();

        dxgi_device_.reset();
    }


    ComPtr<IDXGIOutput> GraphicDeviceManager::getCurOutput() {
        return cur_output_;
    }

    ComPtr<IDXGIAdapter> GraphicDeviceManager::getCurAdapter() {
        return cur_adapter_;
    }

    ComPtr<ID2D1Factory1> GraphicDeviceManager::getD2DFactory() {
        return d2d_factory_;
    }

    ComPtr<IDWriteFactory> GraphicDeviceManager::getDWriteFactory() {
        return dwrite_factory_;
    }

    ComPtr<IDXGIFactory2> GraphicDeviceManager::getDXGIFactory() {
        return dxgi_factory_;
    }

    ComPtr<IDXGIDevice2> GraphicDeviceManager::getDXGIDevice() {
        return dxgi_device_;
    }

    ComPtr<ID2D1Device> GraphicDeviceManager::getD2DDevice() {
        return d2d_device_;
    }

    ComPtr<ID3D11Device> GraphicDeviceManager::getD3DDevice() {
        return d3d_device_;
    }

    ComPtr<ID3D11DeviceContext> GraphicDeviceManager::getD3DDeviceContext() {
        return d3d_devicecontext_;
    }

    ComPtr<ID2D1DeviceContext> GraphicDeviceManager::createD2DDeviceContext() {
        ComPtr<ID2D1DeviceContext> dc;

        HRESULT hr = d2d_device_->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc);
        if (FAILED(hr)) {
            Log::e(L"GraphicsDeviceManager", L"failed to create d2d device context.");
        }

        return dc;
    }

    ComPtr<ID2D1RenderTarget> GraphicDeviceManager::createWICRenderTarget(
        IWICBitmap* wic_bitmap, bool dpi_awareness) {

        ComPtr<ID2D1RenderTarget> render_target;

        if (d2d_factory_) {
            float dpi_x = 96.f;
            float dpi_y = 96.f;
            if (dpi_awareness) {
                d2d_factory_->GetDesktopDpi(&dpi_x, &dpi_y);
            }

            const D2D1_PIXEL_FORMAT format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

            const D2D1_RENDER_TARGET_PROPERTIES properties
                = D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                    format, dpi_x, dpi_y,
                    D2D1_RENDER_TARGET_USAGE_NONE);

            HRESULT hr = d2d_factory_->CreateWicBitmapRenderTarget(
                wic_bitmap, properties, &render_target);
            if (FAILED(hr)) {
                DCHECK(false);
                LOG(Log::WARNING) << "Failed to create WICBitmap RenderTarget: " << hr
                    << ", DpiX: " << dpi_x << ", DpiY: " << dpi_y;
                return {};
            }
        }

        return render_target;
    }

    ComPtr<ID2D1DCRenderTarget> GraphicDeviceManager::createDCRenderTarget() {
        ComPtr<ID2D1DCRenderTarget> render_target;

        if (d2d_factory_) {
            const D2D1_PIXEL_FORMAT format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                    D2D1_ALPHA_MODE_PREMULTIPLIED);

            // DPI ¹Ì¶¨Îª 96
            const D2D1_RENDER_TARGET_PROPERTIES properties =
                D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                    format);

            HRESULT hr = d2d_factory_->CreateDCRenderTarget(
                &properties, &render_target);
            if (FAILED(hr)) {
                DCHECK(false);
                LOG(Log::WARNING) << "Failed to create DC RenderTarget: " << hr;
                return {};
            }
        }

        return render_target;
    }

    ComPtr<IDXGISurface> GraphicDeviceManager::createDXGISurface(int width, int height) {
        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

        ComPtr<ID3D11Texture2D> d3d_texture;
        HRESULT hr = d3d_device_->CreateTexture2D(&tex_desc, nullptr, &d3d_texture);
        DCHECK(SUCCEEDED(hr));

        ComPtr<IDXGISurface> dxgi_surface = d3d_texture.cast<IDXGISurface>();
        DCHECK(dxgi_surface != nullptr);

        return dxgi_surface;
    }

    ComPtr<ID2D1RenderTarget> GraphicDeviceManager::createDXGIRenderTarget(
        IDXGISurface* surface, bool dpi_awareness) {

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96, 96, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

        if (dpi_awareness) {
            d2d_factory_->GetDesktopDpi(&props.dpiX, &props.dpiY);
        }

        ComPtr<ID2D1RenderTarget> render_target;
        HRESULT hr = d2d_factory_->CreateDxgiSurfaceRenderTarget(surface, props, &render_target);
        DCHECK(SUCCEEDED(hr));

        return render_target;
    }
}