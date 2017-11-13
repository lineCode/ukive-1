#ifndef UKIVE_APPLICATION_H_
#define UKIVE_APPLICATION_H_

#include <Windows.h>

#include <vector>

#include "utils/string_utils.h"


namespace ukive {

    class Application
    {
    public:
        Application();
        Application(wchar_t *cl);
        Application(int argc, wchar_t *argv[]);
        ~Application();

        void run();

        size_t getCommandCount();
        string16 getCommand(size_t index);

        static int getScreenWidth();
        static int getScreenHeight();

        static void setVSync(bool enable);
        static bool isVSyncEnabled();

        static HMODULE getModuleHandle();

    private:
        void initApplication();
        void cleanApplication();
        void parseCommandLine(wchar_t *cmdLine);

        static bool sVSyncEnabled;

        std::vector<std::wstring> command_list_;
    };

}

#endif  // UKIVE_APPLICATION_H_