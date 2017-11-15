#include "graphic_device_manager.h"

#include "ukive/log.h"

#pragma comment(lib,"gdiplus.lib")


namespace ukive {

    void GraphicDeviceManager::init() {
        initGdiplus();
    }

    void GraphicDeviceManager::shutdown() {
        shutdownGdiplus();
    }

    void GraphicDeviceManager::initGdiplus() {
        auto status = Gdiplus::GdiplusStartup(&gdiplus_token_, &gdiplus_startup_input_, NULL);
        if (status != Gdiplus::Status::Ok) {
            Log::e(L"failed to init gdiplus.");
        }
    }

    void GraphicDeviceManager::shutdownGdiplus() {
        Gdiplus::GdiplusShutdown(gdiplus_token_);
    }

}