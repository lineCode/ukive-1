#include "ukive/graphics/graphic_device_manager.h"

#include "utils/log.h"


namespace ukive {

    bool GraphicDeviceManager::init() {
        if (!initPersistance()) {
            return false;
        }
        return initDXDevice();
    }

    void GraphicDeviceManager::shutdown() {
        shutdownPersistance();
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


    bool GraphicDeviceManager::initPersistance() {
        HRESULT hr = ::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &d2d_factory_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d2d factory.";
            return false;
        }

        hr = ::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&dwrite_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dwrite factory.";
            return false;
        }

        hr = ::CreateDXGIFactory(
            __uuidof(IDXGIFactory),
            reinterpret_cast<void**>(&dxgi_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi factory.";
            return false;
        }

        hr = CoCreateInstance(
            CLSID_WICImagingFactory1,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wic_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create WIC factory: " << hr;
            return false;
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
        return true;
    }

    void GraphicDeviceManager::shutdownPersistance() {
        cur_output_.reset();
        cur_adapter_.reset();

        d2d_factory_.reset();
        dwrite_factory_.reset();
        dxgi_factory_.reset();
    }

    bool GraphicDeviceManager::initDXDevice() {
        D3D_FEATURE_LEVEL featureLevel[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };

        HRESULT hr = ::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            featureLevel, ARRAYSIZE(featureLevel), D3D11_SDK_VERSION,
            &d3d_device_, nullptr, &d3d_devicecontext_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d3d device.";
            return false;
        }

        hr = d3d_device_->QueryInterface(&dxgi_device_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi device.";
            return false;
        }

        return true;
    }

    void GraphicDeviceManager::shutdownDXDevice() {
        d3d_devicecontext_.reset();
        d3d_device_.reset();

        dxgi_device_.reset();
    }


    ComPtr<IDXGIOutput> GraphicDeviceManager::getCurOutput() const {
        return cur_output_;
    }

    ComPtr<IDXGIAdapter> GraphicDeviceManager::getCurAdapter() const {
        return cur_adapter_;
    }

    ComPtr<ID2D1Factory> GraphicDeviceManager::getD2DFactory() const {
        return d2d_factory_;
    }

    ComPtr<IDWriteFactory> GraphicDeviceManager::getDWriteFactory() const {
        return dwrite_factory_;
    }

    ComPtr<IDXGIFactory> GraphicDeviceManager::getDXGIFactory() const {
        return dxgi_factory_;
    }

    ComPtr<IWICImagingFactory> GraphicDeviceManager::getWICFactory() const {
        return wic_factory_;
    }

    ComPtr<IDXGIDevice> GraphicDeviceManager::getDXGIDevice() const {
        return dxgi_device_;
    }

    ComPtr<ID3D11Device> GraphicDeviceManager::getD3DDevice() const {
        return d3d_device_;
    }

    ComPtr<ID3D11DeviceContext> GraphicDeviceManager::getD3DDeviceContext() const {
        return d3d_devicecontext_;
    }

    ComPtr<ID2D1RenderTarget> GraphicDeviceManager::createWICRenderTarget(IWICBitmap* wic_bitmap) {
        ComPtr<ID2D1RenderTarget> render_target;
        auto d2d_factory = getD2DFactory();
        if (d2d_factory) {
            const D2D1_PIXEL_FORMAT format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

            const D2D1_RENDER_TARGET_PROPERTIES properties
                = D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                    format, 96, 96,
                    D2D1_RENDER_TARGET_USAGE_NONE);

            HRESULT hr = d2d_factory->CreateWicBitmapRenderTarget(
                wic_bitmap, properties, &render_target);
            if (FAILED(hr)) {
                DCHECK(false);
                LOG(Log::WARNING) << "Failed to create WICBitmap RenderTarget: " << hr;
                return {};
            }
        }

        return render_target;
    }

    ComPtr<ID2D1DCRenderTarget> GraphicDeviceManager::createDCRenderTarget() {
        ComPtr<ID2D1DCRenderTarget> render_target;
        auto d2d_factory = getD2DFactory();

        if (d2d_factory) {
            const D2D1_PIXEL_FORMAT format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                    D2D1_ALPHA_MODE_PREMULTIPLIED);

            // DPI ¹Ì¶¨Îª 96
            const D2D1_RENDER_TARGET_PROPERTIES properties =
                D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                    format);

            HRESULT hr = d2d_factory->CreateDCRenderTarget(
                &properties, &render_target);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to create DC RenderTarget: " << hr;
                return {};
            }
        }

        return render_target;
    }

    ComPtr<ID3D11Texture2D> GraphicDeviceManager::createTexture2D(int width, int height, bool gdi_compat) {
        auto d3d_device = getD3DDevice();

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = gdi_compat ? D3D11_RESOURCE_MISC_GDI_COMPATIBLE : 0;

        ComPtr<ID3D11Texture2D> d3d_texture;
        HRESULT hr = d3d_device->CreateTexture2D(&tex_desc, nullptr, &d3d_texture);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return {};
        }

        return d3d_texture;
    }

    ComPtr<ID2D1RenderTarget> GraphicDeviceManager::createDXGIRenderTarget(IDXGISurface* surface, bool gdi_compat) {
        ComPtr<ID2D1RenderTarget> render_target;
        auto d2d_factory = getD2DFactory();
        if (d2d_factory) {
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96, gdi_compat ? D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE : D2D1_RENDER_TARGET_USAGE_NONE);

            HRESULT hr = d2d_factory->CreateDxgiSurfaceRenderTarget(surface, props, &render_target);
            if (FAILED(hr)) {
                DCHECK(false);
            }
        }

        return render_target;
    }

}