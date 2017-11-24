#include "application.h"

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/log.h"
#include "ukive/message/message.h"
#include "ukive/message/message_looper.h"
#include "ukive/animation/animation_manager.h"
#include "ukive/text/word_breaker.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Elscore.lib")


namespace ukive {

    int Application::view_uid_ = 10000;
    bool Application::vsync_enabled_ = true;
    Application *Application::instance_ = nullptr;


    Application::Application() {
        initApplication();

        instance_ = this;
    }

    Application::Application(wchar_t *cl) {
        parseCommandLine(cl);
        initApplication();

        instance_ = this;
    }

    Application::Application(int argc, wchar_t *argv[]) {
        for (int i = 0; i < argc; ++i)
            command_list_.push_back(argv[i]);
        initApplication();

        instance_ = this;
    }


    Application::~Application() {
        cleanApplication();

        instance_ = nullptr;
    }

    void Application::initApplication() {
        ::CoInitialize(NULL);

        Message::init(50);
        MessageLooper::init();
        MessageLooper::prepareMainLooper();
        WordBreaker::initGlobal();

        HRESULT hr = AnimationManager::initGlobal();
        if (FAILED(hr))
            throw std::runtime_error(
                "UAnimationManager-initGlobal(): Init anim library failed.");

        wic_manager_.reset(new WICManager());
        hr = wic_manager_->init();
        if (FAILED(hr))
            throw std::runtime_error("UApplication-initApplication(): Init WIC failed.");

        tsf_manager_.reset(new TsfManager());
        hr = tsf_manager_->init();
        if (FAILED(hr))
            throw std::runtime_error("UApplication-initApplication(): Init Tsf failed.");

        graphic_device_manager_.reset(new GraphicDeviceManager());
        graphic_device_manager_->init();
    }

    void Application::cleanApplication() {
        MessageLooper::close();
        Message::close();
        AnimationManager::closeGlobal();
        WordBreaker::closeGlobal();

        tsf_manager_->close();
        tsf_manager_.reset();

        wic_manager_->close();
        wic_manager_.reset();

        graphic_device_manager_->shutdown();

        ::CoUninitialize();
    }

    void Application::parseCommandLine(wchar_t *cmdLine) {
        if (cmdLine == nullptr)
            return;

        std::wstring cmdString = cmdLine;
        if (cmdString.empty())
            return;

        size_t i = cmdString.find(L" ");
        if (i == std::wstring::npos)
        {
            command_list_.push_back(cmdString);
            return;
        }

        size_t newStart = 0;

        while (i != std::wstring::npos) {
            std::wstring tmp = cmdString.substr(newStart, i - newStart);
            command_list_.push_back(tmp);

            newStart = i + 1;
            i = cmdString.find(L" ", newStart);
            if (i == std::wstring::npos) {
                tmp = cmdString.substr(newStart, cmdString.length() - newStart);
                command_list_.push_back(tmp);
            }
        }
    }


    void Application::run() {
        MSG msg;
        bool done = false;
        ZeroMemory(&msg, sizeof(MSG));

        while (!done)
        {
            if (vsync_enabled_)
                HRESULT hr = graphic_device_manager_->getCurOutput()->WaitForVBlank();

            MessageLooper::loop();
            while (::PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    done = true;
                    MessageLooper::myLooper()->quit();
                    break;
                }
                else
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessageW(&msg);
                }
            }
        }
    }


    string16 Application::getCommand(size_t index) {
        return command_list_.at(index);
    }


    size_t Application::getCommandCount() {
        return command_list_.size();
    }

    void Application::setVSync(bool enable){
        vsync_enabled_ = enable;
    }

    bool Application::isVSyncEnabled(){
        return vsync_enabled_;
    }

    float Application::dpToPxX(float dp) {
        FLOAT dpiX, dpiY;
        instance_->graphic_device_manager_->getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
        return dpiX / 96 * dp;
    }

    float Application::dpToPxY(float dp) {
        FLOAT dpiX, dpiY;
        instance_->graphic_device_manager_->getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
        return dpiY / 96 * dp;
    }

    HMODULE Application::getModuleHandle() {
        return ::GetModuleHandle(NULL);
    }

    WICManager *Application::getWICManager() {
        return instance_->wic_manager_.get();
    }

    TsfManager *Application::getTsfManager() {
        return instance_->tsf_manager_.get();
    }

    int Application::getViewUID() {
        ++view_uid_;
        return view_uid_;
    }

    GraphicDeviceManager* Application::getGraphicDeviceManager() {
        return instance_->graphic_device_manager_.get();
    }


    int Application::getScreenWidth(){
        return ::GetSystemMetrics(SM_CXSCREEN);
    }

    int Application::getScreenHeight(){
        return ::GetSystemMetrics(SM_CYSCREEN);
    }

}