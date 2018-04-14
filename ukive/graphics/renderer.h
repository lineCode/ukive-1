#ifndef UKIVE_GRAPHICS_RENDERER_H_
#define UKIVE_GRAPHICS_RENDERER_H_

#include <functional>
#include <vector>

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Canvas;
    class Window;
    class SwapChainResizeNotifier;

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        HRESULT init(Window* window);
        HRESULT resize();
        bool render(const Color& bg_color, std::function<void()> callback);
        void close();

        HRESULT drawShadow(float elevation, float alpha, ID2D1Bitmap* bitmap);

        void addSwapChainResizeNotifier(SwapChainResizeNotifier* notifier);
        void removeSwapChainResizeNotifier(SwapChainResizeNotifier* notifier);
        void removeAllSwapChainResizeNotifier();

        ComPtr<ID2D1Bitmap1> createBitmapRenderTarget(
            IWICBitmap* wic_bitmap, bool gdi_compat = false);
        ComPtr<ID2D1Bitmap1> createBitmapRenderTarget(
            IDXGISurface* dxgiSurface, bool gdi_compat = false);

        static ComPtr<ID2D1RenderTarget> createWICRenderTarget(
            IWICBitmap* wic_bitmap, bool dpi_awareness);
        static ComPtr<ID2D1DCRenderTarget> createDCRenderTarget();

        static ComPtr<ID3D11Texture2D> createTexture2D(int width, int height);
        static ComPtr<ID2D1RenderTarget> createDXGIRenderTarget(
            IDXGISurface* surface, bool dpi_awareness);

        ComPtr<ID2D1Effect> getShadowEffect();
        ComPtr<ID2D1Effect> getAffineTransEffect();
        ComPtr<IDXGISwapChain1> getSwapChain();
        ComPtr<ID2D1DeviceContext> getD2DDeviceContext();

        static ComPtr<IDWriteTextFormat> createTextFormat(
            const string16& font_family_name,
            float font_size, const string16& locale_name);
        static ComPtr<IDWriteTextLayout> createTextLayout(
            const string16& text, IDWriteTextFormat* format,
            float max_width, float max_height);

    private:
        HRESULT createRenderResource();
        void releaseRenderResource();

        void createHardwareBRT();
        void createSoftwareBRT();
        HRESULT createSwapchainBRT();

        HRESULT resizeHardwareBRT();
        HRESULT resizeSoftwareBRT();
        HRESULT resizeSwapchainBRT();

        HRESULT drawLayered();
        HRESULT drawSwapchain();

        bool is_layered_;
        bool is_hardware_acc_;

        Window* owner_window_;
        std::vector<SwapChainResizeNotifier*> sc_resize_notifiers_;

        ComPtr<IDXGISwapChain1> swapchain_;
        ComPtr<ID2D1Bitmap1> bitmap_render_target_;
        ComPtr<ID2D1DeviceContext> d2d_dc_;

        ComPtr<ID2D1Effect> shadow_effect_;
        ComPtr<ID2D1Effect> affinetrans_effect_;
    };

}

#endif  // UKIVE_GRAPHICS_RENDERER_H_