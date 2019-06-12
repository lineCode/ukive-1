#include <Windows.h>

#include "ukive/log.h"

#include "oigka/layout_processor.h"
#include "oigka/resource_header_processor.h"


int processXMLFiles(const string16& res_path, const string16& build_path) {
    string16 layout_xml_path = res_path + L"\\layout";
    string16 header_file_path = res_path + L"\\oigka_resources_id.h";

    string16 build_res_path = build_path + L"\\oigka";

    LOG(Log::INFO) << "Start processing...";

    oigka::LayoutProcessor layout_processor;
    if (layout_processor.process(layout_xml_path, build_res_path)) {
        LOG(Log::INFO) << "Generating resources header...";

        oigka::ResourceHeaderProcessor header_processor;
        if (!header_processor.write(
            header_file_path,
            layout_processor.getViewIdMap(), layout_processor.getLayoutIdMap()))
        {
            LOG(Log::ERR) << "Failed to generate header: " << header_file_path;
            return 1;
        }
    } else {
        LOG(Log::ERR) << "Failed to process layout";
        return 1;
    }

    LOG(Log::INFO) << "Succeeded!";

    return 0;
}

int APIENTRY wWinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{
    int result = 0;

    ukive::Log::Params log_params;
    log_params.file_name = L"Oigka.log";
    log_params.short_file_name = true;
    log_params.target = ukive::Log::OutputTarget::CONSOLE;
    ukive::InitLogging(log_params);

    auto cmds = ukive::splitString(lpCmdLine, L" ", true);
    if (cmds.size() != 2) {
        LOG(Log::ERR) << "Invalid params: The number of params must equal 2!";
        result = 1;
    }

    if (result == 0) {
        string16 resource_path = cmds[0];
        string16 build_path = cmds[1];

        result = processXMLFiles(resource_path, build_path);
    }

    system("pause");

    ukive::UninitLogging();
    return result;
}