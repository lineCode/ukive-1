#include <Windows.h>

#include <memory>

#include "ukive/application.h"
#include "ukive/log.h"

#include "shell/test/test_window.h"
#include "shell/lod/lod_window.h"
#include "shell/test/3d/motion_window.h"
#include "shell/test/text/text_window.h"
#include "shell/test/bitmap_research/bitmap_research_window.h"
#include "shell/test/shadow/shadow_window.h"
#include "shell/disassembler/disassembler_window.h"


int APIENTRY wWinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{
    ukive::Log::Params log_params;
    log_params.file_name = L"Debug.log";
    log_params.short_file_name = false;
    log_params.target = ukive::Log::OutputTarget::DBG_STR | ukive::Log::OutputTarget::FILE;
    ukive::InitLogging(log_params);

    LOG(Log::INFO) << "Application start.";

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
    test_window->setFrameType(ukive::Window::FRAME_CUSTOM);
    test_window->setTranslucent(false);
    test_window->center();
    test_window->show();

    //auto thr_dimen_window = std::make_shared<shell::MotionWindow>();
    //thr_dimen_window->setTitle(L"3D Motion");
    //thr_dimen_window->setWidth(ukive::Application::dpToPx(600));
    //thr_dimen_window->setHeight(ukive::Application::dpToPx(600));
    ////thr_dimen_window->setTranslucent(true);
    //thr_dimen_window->center();
    //thr_dimen_window->show();

    //auto dar_window = std::make_shared<shell::DisassemblerWindow>();
    //dar_window->setTitle(L"Disassembler");
    //dar_window->setWidth(ukive::Application::dpToPx(600));
    //dar_window->setHeight(ukive::Application::dpToPx(600));
    //dar_window->setFrameType(ukive::Window::FRAME_CUSTOM);
    ////dar_window->setTranslucent(true);
    //dar_window->center();
    //dar_window->show();

    app->run();

    LOG(Log::INFO) << "Application exit.\n";

    ukive::UninitLogging();

    return 0;
}
