#ifndef UKIVE_GRAPHICS_SWAPCHAIN_RESIZE_NOTIFIER_H_
#define UKIVE_GRAPHICS_SWAPCHAIN_RESIZE_NOTIFIER_H_


namespace ukive {

    class SwapChainResizeNotifier {
    public:
        virtual ~SwapChainResizeNotifier() = default;

        virtual void onPreSwapChainResize() = 0;
        virtual void onPostSwapChainResize() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_SWAPCHAIN_RESIZE_NOTIFIER_H_