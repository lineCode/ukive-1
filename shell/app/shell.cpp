#include <Windows.h>

#include <memory>

#include "ukive/application.h"

#include "shell/test/test_window.h"
#include "shell/lod/lod_window.h"
#include "shell/test/bitmap_research/bitmap_research_window.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    std::shared_ptr<ukive::Application> app
        = std::make_shared<ukive::Application>(lpCmdLine);

    /*ukive::Window *bmp_window = new shell::BitmapResearchWindow();
    bmp_window->setTitle(L"Ukive Bitmap Dumper");
    bmp_window->setWidth(ukive::Application::dpToPx(400));
    bmp_window->setHeight(ukive::Application::dpToPx(400));
    bmp_window->center();
    bmp_window->show();*/

    std::shared_ptr<ukive::Window> test_window = std::make_shared<shell::TestWindow>();
    test_window->setTitle(L"Test");
    test_window->setWidth(ukive::Application::dpToPx(400));
    test_window->setHeight(ukive::Application::dpToPx(400));
    test_window->center();
    test_window->show();

    std::shared_ptr<ukive::Window> lod_window = std::make_shared<shell::LodWindow>();
    lod_window->setTitle(L"Test");
    lod_window->setWidth(ukive::Application::dpToPx(400));
    lod_window->setHeight(ukive::Application::dpToPx(400));
    lod_window->center();
    lod_window->show();

    app->run();

    return 0;
}