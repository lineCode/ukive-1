#ifndef SHELL_TEST_EMULATOR_EMULATOR_WINDOW_H_
#define SHELL_TEST_EMULATOR_EMULATOR_WINDOW_H_

#include "ukive/window/window.h"


namespace shell {

    class EmulatorWindow : public ukive::Window {
    public:
        void onCreate() override;

    private:
        bool command(std::wstring command);
        void shutdown();

        std::wstring startReading();

        HANDLE mChildStdOutRead;
        HANDLE mChildStdOutWrite;
        PROCESS_INFORMATION mProcInfo;
    };

}

#endif  // SHELL_TEST_EMULATOR_EMULATOR_WINDOW_H_