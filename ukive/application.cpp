#include "application.h"

#include <algorithm>

#include <dwmapi.h>
#include <ShellScalingAPI.h>
#include <VersionHelpers.h>

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
//#pragma comment(lib, "Shcore.lib")
#pragma comment(lib, "winmm.lib")


namespace ukive {

    int Application::view_uid_ = 10000;
    bool Application::vsync_enabled_ = true;
    Application* Application::instance_ = nullptr;


    Application::Application() {
        initApplication();

        instance_ = this;
    }

    Application::Application(wchar_t* cl) {
        parseCommandLine(cl);
        initApplication();

        instance_ = this;
    }

    Application::Application(int argc, wchar_t* argv[]) {
        for (int i = 0; i < argc; ++i) {
            command_list_.push_back(argv[i]);
        }
        initApplication();

        instance_ = this;
    }


    Application::~Application() {
        cleanApplication();

        instance_ = nullptr;
    }

    void Application::initApplication() {
        HRESULT hr = ::CoInitialize(nullptr);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "failed to init COM.";
        }

        Message::init(50);
        MessageLooper::prepareMainLooper();
        WordBreaker::initGlobal();

        hr = AnimationManager::initGlobal();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Init anim library failed.";
        }

        graphic_device_manager_.reset(new GraphicDeviceManager());
        graphic_device_manager_->init();

        wic_manager_.reset(new WICManager());

        tsf_manager_.reset(new TsfManager());
        hr = tsf_manager_->init();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Init Tsf failed.";
        }
    }

    void Application::cleanApplication() {
        Message::close();
        AnimationManager::closeGlobal();
        WordBreaker::closeGlobal();

        tsf_manager_->close();
        tsf_manager_.reset();

        wic_manager_.reset();

        graphic_device_manager_->shutdown();

        ::CoUninitialize();
    }

    void Application::parseCommandLine(wchar_t* cmd_line) {
        if (!cmd_line) {
            return;
        }

        std::wstring cmd_string = cmd_line;
        if (cmd_string.empty()) {
            return;
        }

        size_t i = cmd_string.find(L" ");
        if (i == std::wstring::npos) {
            command_list_.push_back(cmd_string);
            return;
        }

        size_t new_start = 0;

        while (i != std::wstring::npos) {
            std::wstring tmp = cmd_string.substr(new_start, i - new_start);
            if (!tmp.empty()) {
                command_list_.push_back(tmp);
            }

            new_start = i + 1;
            i = cmd_string.find(L" ", new_start);
            if (i == std::wstring::npos) {
                tmp = cmd_string.substr(new_start, cmd_string.length() - new_start);
                if (!tmp.empty()) {
                    command_list_.push_back(tmp);
                }
            }
        }
    }


    void Application::run() {
        MSG msg;
        bool done = false;
        ZeroMemory(&msg, sizeof(MSG));

        while (!done) {
            if (vsync_enabled_) {
                HRESULT hr = graphic_device_manager_->getCurOutput()->WaitForVBlank();
                if (FAILED(hr)) {
                    LOG(Log::WARNING) << "Failed to wait vblank: " << hr;
                    ::Sleep(16);
                }
            }

            bool has_message = MessageLooper::loop();
            if (!has_message) {
                DWORD result = ::MsgWaitForMultipleObjectsEx(
                    0, nullptr, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (result == WAIT_OBJECT_0) {
                    int i = 0;
                }
            }

            DWORD status = ::GetQueueStatus(QS_INPUT);
            if (HIWORD(status) & QS_INPUT) {
                int i = 0;
            }

            while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    done = true;
                    MessageLooper::myLooper()->quit();
                    break;
                } else {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
            }
        }
    }


    string16 Application::getCommand(size_t index) {
        return instance_->command_list_.at(index);
    }


    size_t Application::getCommandCount() {
        return instance_->command_list_.size();
    }

    void Application::setVSync(bool enable){
        vsync_enabled_ = enable;
    }

    bool Application::isVSyncEnabled(){
        return vsync_enabled_;
    }

    HMODULE Application::getModuleHandle() {
        return ::GetModuleHandle(nullptr);
    }

    string16 Application::getExecFileName(bool dir) {
        WCHAR buffer[MAX_PATH];
        DWORD result = ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (result == 0) {
            return {};
        }

        DWORD size = MAX_PATH;
        std::unique_ptr<WCHAR[]> heap_buff;
        while (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            size *= 2;
            heap_buff.reset(new WCHAR[size]);
            result = ::GetModuleFileNameW(nullptr, heap_buff.get(), size);
            if (result == 0) {
                return {};
            }
        }

        string16 file_name;
        if (heap_buff) {
            file_name = heap_buff.get();
        } else {
            file_name = buffer;
        }

        if (dir) {
            auto i = file_name.find_last_of(L"\\");
            if (i != string16::npos) {
                file_name = file_name.substr(0, i);
            }
        }

        return file_name;
    }

    WICManager* Application::getWICManager() {
        return instance_->wic_manager_.get();
    }

    TsfManager* Application::getTsfManager() {
        return instance_->tsf_manager_.get();
    }

    int Application::getViewID() {
        ++view_uid_;
        return view_uid_;
    }

    int Application::getPrimaryDpi() {
        if (::IsWindows8Point1OrGreater()) {
            POINT pt = { 0, 0 };
            HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

            UINT dpi_x = 96, dpi_y = 96;
            using GetDpiForMonitorPtr = HRESULT(STDAPICALLTYPE*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
            auto func = reinterpret_cast<GetDpiForMonitorPtr>(
                ::GetProcAddress(::LoadLibraryW(L"Shcore.dll"), "GetDpiForMonitor"));
            if (func) {
                HRESULT hr = func(monitor, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
                if (SUCCEEDED(hr)) {
                    return dpi_x;
                }
            }
        }

        HDC screen = ::GetDC(nullptr);
        int dpiX = ::GetDeviceCaps(screen, LOGPIXELSX);
        //int dpiY = ::GetDeviceCaps(screen, LOGPIXELSY);
        ::ReleaseDC(nullptr, screen);

        return dpiX;
    }

    float Application::dpToPx(float dp) {
        return getPrimaryDpi() / 96.f * dp;
    }

    float Application::pxToDp(int px) {
        return px / (getPrimaryDpi() / 96.f);
    }

    bool Application::isAeroEnabled() {
        BOOL dwm_enabled = FALSE;
        HRESULT hr = ::DwmIsCompositionEnabled(&dwm_enabled);
        return (SUCCEEDED(hr) && dwm_enabled == TRUE);
    }

    void Application::quitSystemQueue() {
        ::PostQuitMessage(0);
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