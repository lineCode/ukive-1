#ifndef UKIVE_GRAPHICS_RENDERER_H_
#define UKIVE_GRAPHICS_RENDERER_H_

#include <functional>
#include <list>

#include "ukive/graphics/drawing_object_manager.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Canvas;
    class WindowImpl;
    class GraphicDeviceManager;
    class Direct3DRenderListener;
    class SwapChainResizeNotifier;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        HRESULT init(WindowImpl *window);
        HRESULT resize();
        bool render(
            Color bkColor,
            std::function<void()> renderCallback);
        void close();

        HRESULT drawWithShadow(
            float elevation, float width, float height,
            std::function<void(ComPtr<ID2D1RenderTarget> rt)> drawer);
        HRESULT drawShadow(float elevation, float alpha, ID2D1Bitmap *bitmap);
        HRESULT drawOnBitmap(
            float width, float height, ID2D1Bitmap **bitmap,
            std::function<void(ComPtr<ID2D1RenderTarget> rt)> drawer);

        static void drawObjects(DrawingObjectManager::DrawingObject *object);
        static void draw(ID3D11Buffer* vertices, ID3D11Buffer* indices, int structSize, int indexCount);

        void addSwapChainResizeNotifier(SwapChainResizeNotifier *notifier);
        void removeSwapChainResizeNotifier(SwapChainResizeNotifier *notifier);
        void removeAllSwapChainResizeNotifier();

        void setDirect3DRenderListener(Direct3DRenderListener *listener);

        static void setVertexShader(ID3D11VertexShader *shader);
        static void setPixelShader(ID3D11PixelShader *shader);
        static void setInputLayout(ID3D11InputLayout *inputLayout);
        static void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
        static void setConstantBuffers(UINT startSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);

        UINT getScWidth();
        UINT getScHeight();
        ComPtr<ID2D1Effect> getShadowEffect();
        ComPtr<ID2D1Effect> getAffineTransEffect();
        ComPtr<IDXGISwapChain1> getSwapChain();
        ComPtr<ID2D1DeviceContext> getD2DDeviceContext();

        HRESULT createBitmapRenderTarget(
            IDXGISurface *dxgiSurface, ID2D1Bitmap1 **bitmap);
        HRESULT createCompatBitmapRenderTarget(
            float width, float height, ID2D1BitmapRenderTarget **bRT);
        HRESULT createDXGISurfaceRenderTarget(
            IDXGISurface *dxgiSurface, ID2D1RenderTarget **renderTarget);
        HRESULT createWindowRenderTarget(
            HWND handle, unsigned int width, unsigned int height,
            ID2D1HwndRenderTarget **renderTarget);

        static HRESULT createTextFormat(
            string16 fontFamilyName,
            float fontSize, string16 localeName,
            IDWriteTextFormat **textFormat);
        static HRESULT createTextLayout(
            string16 text, IDWriteTextFormat *textFormat,
            float maxWidth, float maxHeight,
            IDWriteTextLayout **textLayout);

        static HRESULT createVertexShader(
            string16 fileName,
            D3D11_INPUT_ELEMENT_DESC *polygonLayout,
            UINT numElements,
            ID3D11VertexShader **vertexShader,
            ID3D11InputLayout **inputLayout);
        static HRESULT createPixelShader(
            string16 fileName,
            ID3D11PixelShader **pixelShader);

        static HRESULT createVertexBuffer(
            void *vertices, UINT structSize, UINT vertexCount, ID3D11Buffer *&vertexBuffer);
        static HRESULT createIndexBuffer(int *indices, UINT indexCount, ID3D11Buffer *&indexBuffer);
        static HRESULT createConstantBuffer(UINT size, ID3D11Buffer **buffer);

        static D3D11_MAPPED_SUBRESOURCE lockResource(ID3D11Resource *resource);
        static void unlockResource(ID3D11Resource *resource);

    private:
        HRESULT createRenderResource();
        void releaseRenderResource();

        UINT width_, height_;
        WindowImpl *owner_window_;
        Direct3DRenderListener *d3d_render_listener_;
        std::list<SwapChainResizeNotifier*> sc_resize_notifier_list_;

        ComPtr<IDXGISwapChain1> swapchain_;
        ComPtr<ID2D1Bitmap1> bitmap_render_target_;
        ComPtr<ID2D1DeviceContext> d2d_dc_;

        ComPtr<ID2D1Effect> shadow_effect_;
        ComPtr<ID2D1Effect> affinetrans_effect_;
    };

}

#endif  // UKIVE_GRAPHICS_RENDERER_H_