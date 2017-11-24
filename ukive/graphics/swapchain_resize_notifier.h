#ifndef UKIVE_GRAPHICS_SWAPCHAIN_RESIZE_NOTIFIER_H_
#define UKIVE_GRAPHICS_SWAPCHAIN_RESIZE_NOTIFIER_H_


namespace ukive {

    class SwapChainResizeNotifier
    {
    public:
        virtual void onSwapChainResize() = 0;
        virtual void onSwapChainResized() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_SWAPCHAIN_RESIZE_NOTIFIER_H_