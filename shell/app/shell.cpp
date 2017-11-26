#include <Windows.h>

#include "ukive/application.h"

#include "shell/test/test_window.h"
#include "shell/lod/lod_window.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    ukive::Application *app = new ukive::Application(lpCmdLine);
    ukive::Window *test_window = new shell::TestWindow();
    test_window->center();
    test_window->show();

    /*ukive::Window *lod_window = new shell::LodWindow();
    lod_window->show();*/

    app->run();

    delete app;

    return 0;
}