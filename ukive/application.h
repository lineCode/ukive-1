#ifndef UKIVE_APPLICATION_H_
#define UKIVE_APPLICATION_H_

#include <Windows.h>

#include <memory>
#include <vector>

#include "utils/string_utils.h"
#include "ukive/graphics/wic_manager.h"
#include "ukive/text/tsf_manager.h"


namespace ukive {

    class GraphicDeviceManager;

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

        static float dpToPxX(float dp);
        static float dpToPxY(float dp);

        static float pxToDpX(int px);
        static float pxToDpY(int px);

        static HMODULE getModuleHandle();
        static GraphicDeviceManager* getGraphicDeviceManager();

        static WICManager *getWICManager();
        static TsfManager *getTsfManager();

        static int getViewUID();

    private:
        void initApplication();
        void cleanApplication();
        void parseCommandLine(wchar_t *cmdLine);

        static int view_uid_;
        static bool vsync_enabled_;
        static Application *instance_;

        std::vector<std::wstring> command_list_;

        std::unique_ptr<TsfManager> tsf_manager_;
        std::unique_ptr<WICManager> wic_manager_;
        std::unique_ptr<GraphicDeviceManager> graphic_device_manager_;
    };

}

#endif  // UKIVE_APPLICATION_H_