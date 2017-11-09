#ifndef UKIVE_APPLICATION_H_
#define UKIVE_APPLICATION_H_

#include <string>
#include <vector>


namespace ukive {

    class Application
    {
    public:
        Application();
        Application(wchar_t *cl);
        Application(int argc, wchar_t *argv[]);
        ~Application();

        enum QuitStrategy {
            QUIT_WHEN_LAST_WINDOW_CLOSED,
            QUIT_WHEN_STARTUP_WINDOW_CLOSED,
        };

        void run();

        size_t getCommandCount();
        std::wstring getCommand(size_t index);

        void setQuitStrategy(QuitStrategy qs);
        QuitStrategy getQuitStrategy();

        static int getScreenWidth();
        static int getScreenHeight();

        static void setVSync(bool enable);
        static bool isVSyncEnabled();

    private:
        void initApplication();
        void cleanApplication();
        void parseCommandLine(wchar_t *cmdLine);

        static bool sVSyncEnabled;

        QuitStrategy quit_strategy_;
        std::vector<std::wstring> command_list_;
    };

}

#endif  // UKIVE_APPLICATION_H_