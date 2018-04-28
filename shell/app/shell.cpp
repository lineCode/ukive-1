#include <Windows.h>

#include <memory>

#include "ukive/application.h"

#include "shell/test/test_window.h"
#include "shell/lod/lod_window.h"
#include "shell/test/text/text_window.h"
#include "shell/test/bitmap_research/bitmap_research_window.h"
#include "shell/test/shadow/shadow_window.h"


int APIENTRY wWinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow) {

    auto app = std::make_shared<ukive::Application>(lpCmdLine);

    /*auto bmp_window = std::make_shared<shell::BitmapResearchWindow>();
    bmp_window->setTitle(L"Ukive Bitmap Dumper");
    bmp_window->setWidth(ukive::Application::dpToPx(400));
    bmp_window->setHeight(ukive::Application::dpToPx(400));
    bmp_window->center();
    bmp_window->show();*/

    /*auto text_window = std::make_shared<shell::TextWindow>();
    text_window->setTitle(L"Test");
    text_window->setWidth(ukive::Application::dpToPx(400));
    text_window->setHeight(ukive::Application::dpToPx(400));
    text_window->center();
    text_window->show();*/

    //auto lod_window = std::make_shared<shell::LodWindow>();
    //lod_window->setTitle(L"Test");
    //lod_window->setWidth(ukive::Application::dpToPx(600));
    //lod_window->setHeight(ukive::Application::dpToPx(500));
    ////lod_window->setTranslucent(true);
    //lod_window->center();
    //lod_window->show();

    /*auto shadow_window = std::make_shared<shell::ShadowWindow>();
    shadow_window->setTitle(L"Test");
    shadow_window->setWidth(ukive::Application::dpToPx(600));
    shadow_window->setHeight(ukive::Application::dpToPx(400));
    shadow_window->center();
    shadow_window->show();*/

    auto test_window = std::make_shared<shell::TestWindow>();
    test_window->setTitle(L"Test");
    test_window->setWidth(ukive::Application::dpToPx(600));
    test_window->setHeight(ukive::Application::dpToPx(600));
    test_window->center();
    test_window->show();

    app->run();

    return 0;
}