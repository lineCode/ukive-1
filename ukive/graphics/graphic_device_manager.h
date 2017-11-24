#ifndef UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
#define UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_

#include <Windows.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <d3d11_2.h>
#include <d3dcompiler.h>
#include <dxgi1_3.h>

#include "ukive/utils/com_ptr.h"


namespace ukive {

    class GraphicDeviceManager {
    public:
        void init();
        void shutdown();

        ComPtr<IDXGIOutput> getCurOutput();
        ComPtr<IDXGIAdapter> getCurAdapter();
        ComPtr<ID2D1Factory1> getD2DFactory();
        ComPtr<IDWriteFactory> getDWriteFactory();
        ComPtr<IDXGIFactory2> getDXGIFactory();

        ComPtr<IDXGIDevice2> getDXGIDevice();
        ComPtr<ID2D1Device> getD2DDevice();
        ComPtr<ID3D11Device> getD3DDevice();
        ComPtr<ID3D11DeviceContext> getD3DDeviceContext();

        ComPtr<ID2D1DeviceContext> createD2DDeviceContext();

    private:
        void initDXPersistance();
        void shutdownDXPersistance();

        void initDXDevice();
        void shutdownDXDevice();

        ComPtr<ID2D1Factory1> d2d_factory_;
        ComPtr<IDWriteFactory> dwrite_factory_;
        ComPtr<IDXGIFactory2> dxgi_factory_;

        ComPtr<IDXGIOutput> cur_output_;
        ComPtr<IDXGIAdapter> cur_adapter_;

        ComPtr<ID3D11Device> d3d_device_;                //Win7.
        ComPtr<ID3D11DeviceContext> d3d_devicecontext_;  //Win7.

        ComPtr<IDXGIDevice2> dxgi_device_;               //Win8 and 7-PU.
        ComPtr<ID2D1Device> d2d_device_;                 //Win8 and 7-PU.
    };

}

#endif  // UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
