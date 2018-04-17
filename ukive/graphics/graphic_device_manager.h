#ifndef UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
#define UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#include "ukive/utils/com_ptr.h"


namespace ukive {

    class GraphicDeviceManager {
    public:
        void init();
        void shutdown();

        void EnumSystemFonts();

        ComPtr<IDXGIOutput> getCurOutput();
        ComPtr<IDXGIAdapter> getCurAdapter();
        ComPtr<ID2D1Factory> getD2DFactory();
        ComPtr<IDWriteFactory> getDWriteFactory();
        ComPtr<IDXGIFactory> getDXGIFactory();

        ComPtr<IDXGIDevice> getDXGIDevice();
        ComPtr<ID3D11Device> getD3DDevice();
        ComPtr<ID3D11DeviceContext> getD3DDeviceContext();

    private:
        void initDXPersistance();
        void shutdownDXPersistance();

        void initDXDevice();
        void shutdownDXDevice();

        ComPtr<ID2D1Factory> d2d_factory_;
        ComPtr<IDWriteFactory> dwrite_factory_;
        ComPtr<IDXGIFactory> dxgi_factory_;

        ComPtr<IDXGIOutput> cur_output_;
        ComPtr<IDXGIAdapter> cur_adapter_;

        ComPtr<ID3D11Device> d3d_device_;
        ComPtr<ID3D11DeviceContext> d3d_devicecontext_;

        ComPtr<IDXGIDevice> dxgi_device_;
    };

}

#endif  // UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
