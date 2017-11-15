#ifndef UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
#define UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_

#include <Windows.h>
#include <gdiplus.h>


namespace ukive {

    class GraphicDeviceManager {
    public:
        void init();
        void shutdown();

    private:
        void initGdiplus();
        void shutdownGdiplus();

        ULONG_PTR gdiplus_token_;
        Gdiplus::GdiplusStartupInput gdiplus_startup_input_;
    };

}

#endif  // UKIVE_GRAPHICS_GRAPHIC_DEVICE_MANAGER_H_
