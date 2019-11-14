#ifndef SHELL_TEST_EMULATOR_EMULATOR_WINDOW_H_
#define SHELL_TEST_EMULATOR_EMULATOR_WINDOW_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ukive/window/window.h"


namespace shell {

    class EmulatorWindow : public ukive::Window {
    public:
        EmulatorWindow() = default;

        void onCreate() override;

    private:
        bool command(std::wstring command);
        void shutdown();

        std::wstring startReading();

        HANDLE mChildStdOutRead = nullptr;
        HANDLE mChildStdOutWrite = nullptr;
        PROCESS_INFORMATION mProcInfo;
    };

}

#endif  // SHELL_TEST_EMULATOR_EMULATOR_WINDOW_H_