#include <Windows.h>

#include "ukive/application.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    ukive::Application *app = new ukive::Application(lpCmdLine);
    app->run();

    delete app;

    return 0;
}