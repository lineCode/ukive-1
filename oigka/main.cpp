#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/log.h"
#include "utils/files/file.h"

#include "oigka/layout_processor.h"
#include "oigka/resource_header_processor.h"


int processXMLFiles(const string16& res_path, const string16& build_path) {
    utl::File layout_xml_file(res_path, L"layout");
    utl::File header_file(res_path, L"oigka_resources_id.h");
    utl::File build_res_file(build_path, L"oigka");

    string16 layout_xml_path = layout_xml_file.getPath();
    string16 header_file_path = header_file.getPath();
    string16 build_res_path = build_res_file.getPath();

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

    utl::Log::Params log_params;
    log_params.file_name = L"Oigka.log";
    log_params.short_file_name = true;
    log_params.target = utl::Log::OutputTarget::CONSOLE;
    utl::InitLogging(log_params);

    auto cmds = utl::splitString(lpCmdLine, L" ", true);
    if (cmds.size() != 2) {
        LOG(Log::ERR) << "Invalid params: The number of params must equal 2!";
        result = 1;
    }

    if (result == 0) {
        string16 resource_path = cmds[0];
        string16 build_path = cmds[1];

        if (resource_path.size() >= 2 &&
            resource_path.front() == '"' &&
            resource_path.back() == '"')
        {
            resource_path = resource_path.substr(1, resource_path.length() - 2);
        }

        if (build_path.size() >= 2 &&
            build_path.front() == '"' &&
            build_path.back() == '"')
        {
            build_path = build_path.substr(1, build_path.length() - 2);
        }

        result = processXMLFiles(resource_path, build_path);
    }

    if (result != 0) {
        system("pause");
    }

    utl::UninitLogging();
    return result;
}