#include "application.h"

#include <dwmapi.h>
#include <ShellScalingAPI.h>
#include <VersionHelpers.h>

#include "utils/files/file.h"
#include "utils/log.h"
#include "utils/stl_utils.h"

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/message/message.h"
#include "ukive/message/message_looper.h"
#include "ukive/text/word_breaker.h"
#include "ukive/system/dynamic_windows_api.h"
#include "ukive/resources/layout_instantiator.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "winmm.lib")


namespace ukive {

    int Application::view_uid_ = 10;
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
            LOG(Log::ERR) << "Failed to initialize COM";
        }

        Message::init(50);
        MessageLooper::prepareMainLooper();
        WordBreaker::initGlobal();

        LayoutInstantiator::init();

        graphic_device_manager_.reset(new GraphicDeviceManager());
        graphic_device_manager_->init();

        wic_manager_.reset(new WICManager());

        tsf_manager_.reset(new TsfManager());
        hr = tsf_manager_->init();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to initialize TSF";
        }
    }

    void Application::cleanApplication() {
        Message::close();
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

        command_list_.clear();

        string16 cur_cmd;
        string16 cmd_string = cmd_line;
        bool in_quote = false;

        for (auto ch : cmd_string) {
            if (ch == L' ') {
                if (in_quote) {
                    cur_cmd.push_back(ch);
                } else {
                    if (!cur_cmd.empty()) {
                        command_list_.push_back(cur_cmd);
                        cur_cmd.clear();
                    }
                }
            } else if (ch == L'"') {
                if (in_quote) {
                    in_quote = false;
                    if (!cur_cmd.empty()) {
                        command_list_.push_back(cur_cmd);
                        cur_cmd.clear();
                    }
                } else {
                    in_quote = true;
                }
            } else {
                cur_cmd.push_back(ch);
            }
        }

        if (!cur_cmd.empty()) {
            command_list_.push_back(cur_cmd);
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
                    //
                }
            }

            DWORD status = ::GetQueueStatus(QS_INPUT);
            if (HIWORD(status) & QS_INPUT) {
                //
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

    string16 Application::getCommand(int index) {
        return instance_->command_list_.at(
            utl::STLCST(instance_->command_list_, index));
    }

    int Application::getCommandCount() {
        return utl::STLCInt(instance_->command_list_.size());
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

    void Application::getPrimaryDpi(int* dpi_x, int* dpi_y) {
        static bool is_win8_1_or_above = ::IsWindows8Point1OrGreater();
        if (is_win8_1_or_above) {
            POINT pt = { 0, 0 };
            HMONITOR monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

            UINT _dpi_x = 96, _dpi_y = 96;
            HRESULT hr = win::UDGetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &_dpi_x, &_dpi_y);
            if (SUCCEEDED(hr)) {
                *dpi_x = _dpi_x;
                *dpi_y = _dpi_y;
                return;
            }
        }

        HDC screen = ::GetDC(nullptr);
        *dpi_x = ::GetDeviceCaps(screen, LOGPIXELSX);
        *dpi_y = ::GetDeviceCaps(screen, LOGPIXELSY);
        ::ReleaseDC(nullptr, screen);
    }

    float Application::dpToPxX(float dp) {
        int dpi_x, dpi_y;
        getPrimaryDpi(&dpi_x, &dpi_y);
        return dpi_x / 96.f * dp;
    }

    float Application::dpToPxY(float dp) {
        int dpi_x, dpi_y;
        getPrimaryDpi(&dpi_x, &dpi_y);
        return dpi_y / 96.f * dp;
    }

    float Application::pxToDpX(int px) {
        int dpi_x, dpi_y;
        getPrimaryDpi(&dpi_x, &dpi_y);
        return px / (dpi_x / 96.f);
    }

    float Application::pxToDpY(int px) {
        int dpi_x, dpi_y;
        getPrimaryDpi(&dpi_x, &dpi_y);
        return px / (dpi_y / 96.f);
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