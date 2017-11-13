#include "application.h"


namespace ukive {

    bool Application::sVSyncEnabled = true;


    Application::Application() {
        initApplication();
    }

    Application::Application(wchar_t *cl) {
        parseCommandLine(cl);
        initApplication();
    }

    Application::Application(int argc, wchar_t *argv[]) {
        for (int i = 0; i < argc; ++i)
            command_list_.push_back(argv[i]);
        initApplication();
    }


    Application::~Application() {
        cleanApplication();
    }

    void Application::cleanApplication() {
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
        BOOL ret;
        ZeroMemory(&msg, sizeof(MSG));

        while ((ret = ::GetMessageW(&msg, NULL, 0, 0)) != 0)
        {
            if (ret == -1) {
                // Error.
            }
            else
            {
                ::TranslateMessage(&msg);
                ::DispatchMessageW(&msg);
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
        sVSyncEnabled = enable;
    }

    bool Application::isVSyncEnabled(){
        return sVSyncEnabled;
    }

    HMODULE Application::getModuleHandle() {
        return ::GetModuleHandle(NULL);
    }


    int Application::getScreenWidth(){
        return ::GetSystemMetrics(SM_CXSCREEN);
    }

    int Application::getScreenHeight(){
        return ::GetSystemMetrics(SM_CYSCREEN);
    }

}