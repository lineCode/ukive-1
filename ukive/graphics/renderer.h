#ifndef UKIVE_GRAPHICS_RENDERER_H_
#define UKIVE_GRAPHICS_RENDERER_H_

#include <functional>
#include <vector>

#include "ukive/graphics/drawing_object_manager.h"
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

        HRESULT createBitmapRenderTarget(
            IWICBitmap* wic_bitmap, ID2D1Bitmap1** bitmap, bool gdi_compat = false);
        HRESULT createBitmapRenderTarget(
            IDXGISurface* dxgiSurface, ID2D1Bitmap1** bitmap, bool gdi_compat = false);

        ComPtr<ID2D1Effect> getShadowEffect();
        ComPtr<ID2D1Effect> getAffineTransEffect();
        ComPtr<IDXGISwapChain1> getSwapChain();
        ComPtr<ID2D1DeviceContext> getD2DDeviceContext();

        static HRESULT createTextFormat(
            const string16 fontFamilyName,
            float fontSize, string16 localeName,
            IDWriteTextFormat** textFormat);
        static HRESULT createTextLayout(
            const string16 text, IDWriteTextFormat* textFormat,
            float maxWidth, float maxHeight,
            IDWriteTextLayout** textLayout);

    private:
        HRESULT createRenderResource();
        void releaseRenderResource();

        HRESULT createHardwareBRT();
        HRESULT createSoftwareBRT();
        HRESULT createSwapchainBRT();

        HRESULT resizeHardwareBRT();
        HRESULT resizeSoftwareBRT();
        HRESULT resizeSwapchainBRT();

        HRESULT drawLayered();
        HRESULT drawSwapchain();

        bool is_layered_;
        bool is_hardware_acc_;

        Window* owner_window_;
        std::vector<SwapChainResizeNotifier*> sc_resize_notifier_list_;

        ComPtr<IDXGISwapChain1> swapchain_;
        ComPtr<ID2D1Bitmap1> bitmap_render_target_;
        ComPtr<ID2D1DeviceContext> d2d_dc_;

        ComPtr<ID2D1Effect> shadow_effect_;
        ComPtr<ID2D1Effect> affinetrans_effect_;
    };

}

#endif  // UKIVE_GRAPHICS_RENDERER_H_