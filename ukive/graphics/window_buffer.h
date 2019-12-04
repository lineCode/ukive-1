#ifndef UKIVE_GRAPHICS_WINDOW_BUFFER_H_
#define UKIVE_GRAPHICS_WINDOW_BUFFER_H_

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class Window;

    class WindowBuffer : public CyroBuffer {
    public:
        explicit WindowBuffer(Window* w);

        bool onCreate(bool hw_acc) override;
        bool onResize(int width, int height) override;
        void onDestroy() override;

        void onBeginDraw() override;
        bool onEndDraw() override;

        std::shared_ptr<Bitmap> onExtractBitmap() override;

        ComPtr<ID2D1RenderTarget> getRT() const override { return rt_; }

    private:
        bool createHardwareBRT();
        bool createSoftwareBRT();
        bool createSwapchainBRT();

        bool resizeHardwareBRT();
        bool resizeSoftwareBRT();
        bool resizeSwapchainBRT();

        bool drawLayered();
        bool drawSwapchain();

        bool is_layered_ = false;
        bool is_hardware_acc_ = false;
        Window* window_ = nullptr;
        ComPtr<IDXGISwapChain> swapchain_;
        ComPtr<ID2D1RenderTarget> rt_;
    };

}

#endif  // UKIVE_GRAPHICS_WINDOW_BUFFER_H_