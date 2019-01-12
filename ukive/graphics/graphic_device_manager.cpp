#include "ukive/graphics/graphic_device_manager.h"

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
        HRESULT hr = ::D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &d2d_factory_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d2d factory.";
            return;
        }

        hr = ::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&dwrite_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dwrite factory.";
            return;
        }

        hr = ::CreateDXGIFactory(
            __uuidof(IDXGIFactory),
            reinterpret_cast<void**>(&dxgi_factory_));
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi factory.";
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

        HRESULT hr = ::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            featureLevel, ARRAYSIZE(featureLevel), D3D11_SDK_VERSION,
            &d3d_device_, nullptr, &d3d_devicecontext_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create d3d device.";
            return;
        }

        hr = d3d_device_->QueryInterface(&dxgi_device_);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to create dxgi device.";
            return;
        }
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

    ComPtr<IDXGIDevice> GraphicDeviceManager::getDXGIDevice() const {
        return dxgi_device_;
    }

    ComPtr<ID3D11Device> GraphicDeviceManager::getD3DDevice() const {
        return d3d_device_;
    }

    ComPtr<ID3D11DeviceContext> GraphicDeviceManager::getD3DDeviceContext() const {
        return d3d_devicecontext_;
    }

}