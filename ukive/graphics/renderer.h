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

        void addSwapChainResizeNotifier(SwapChainResizeNotifier* notifier);
        void removeSwapChainResizeNotifier(SwapChainResizeNotifier* notifier);
        void removeAllSwapChainResizeNotifier();

        static ComPtr<ID2D1RenderTarget> createWICRenderTarget(IWICBitmap* wic_bitmap);
        static ComPtr<ID2D1DCRenderTarget> createDCRenderTarget();

        static ComPtr<ID3D11Texture2D> createTexture2D(int width, int height);
        static ComPtr<ID2D1RenderTarget> createDXGIRenderTarget(
            IDXGISurface* surface, bool gdi_compat);

        static ComPtr<IDWriteTextFormat> createTextFormat(
            const string16& font_family_name,
            float font_size, const string16& locale_name);
        static ComPtr<IDWriteTextLayout> createTextLayout(
            const string16& text, IDWriteTextFormat* format,
            float max_width, float max_height);

        ComPtr<IDXGISwapChain> getSwapChain() const;
        ComPtr<ID2D1RenderTarget> getRenderTarget() const;

    private:
        HRESULT createRenderResource();
        void releaseRenderResource();

        void createHardwareBRT();
        void createSoftwareBRT();
        HRESULT createSwapchainBRT();

        HRESULT resizeHardwareBRT();
        HRESULT resizeSoftwareBRT();
        HRESULT resizeSwapchainBRT();

        void applyTextRenderingParams();

        HRESULT drawLayered();
        HRESULT drawSwapchain();

        bool is_layered_;
        bool is_hardware_acc_;

        Window* owner_window_;
        std::vector<SwapChainResizeNotifier*> sc_resize_notifiers_;

        ComPtr<IDXGISwapChain> swapchain_;
        ComPtr<ID2D1RenderTarget> d2d_rt_;
    };

}

#endif  // UKIVE_GRAPHICS_RENDERER_H_