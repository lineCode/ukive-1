#ifndef UKIVE_APPLICATION_H_
#define UKIVE_APPLICATION_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <vector>

#include "utils/string_utils.h"
#include "ukive/graphics/wic_manager.h"
#include "ukive/text/tsf_manager.h"


namespace ukive {

    class GraphicDeviceManager;

    class Application {
    public:
        Application();
        explicit Application(wchar_t* cl);
        Application(int argc, wchar_t* argv[]);
        ~Application();

        void run();

        static int getCommandCount();
        static string16 getCommand(int index);

        static int getScreenWidth();
        static int getScreenHeight();

        static void setVSync(bool enable);
        static bool isVSyncEnabled();

        static HMODULE getModuleHandle();
        static GraphicDeviceManager* getGraphicDeviceManager();

        static WICManager* getWICManager();
        static TsfManager* getTsfManager();

        static int getViewID();

        static void getPrimaryDpi(int* dpi_x, int* dpi_y);

        static float dpToPxX(float dp);
        static float dpToPxY(float dp);
        static float pxToDpX(int px);
        static float pxToDpY(int px);

        static bool isAeroEnabled();

        static void quitSystemQueue();

    private:
        void initApplication();
        void cleanApplication();
        void parseCommandLine(wchar_t* cmd_line);

        static int view_uid_;
        static bool vsync_enabled_;
        static Application* instance_;

        std::vector<string16> command_list_;

        std::unique_ptr<TsfManager> tsf_manager_;
        std::unique_ptr<WICManager> wic_manager_;
        std::unique_ptr<GraphicDeviceManager> graphic_device_manager_;
    };

}

#endif  // UKIVE_APPLICATION_H_