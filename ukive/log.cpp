#include "log.h"

#include <intrin.h>
#include <Windows.h>

#include <fstream>

#include "ukive/application.h"
#include "ukive/files/file.h"


namespace ukive {

    std::wofstream log_file_stream_;

    void InitLogging() {
        auto exec_dir = ukive::Application::getExecFileName(true);
        File log_file(exec_dir, L"Debug.log");
        log_file_stream_.open(log_file.getPath().c_str(), std::ios::out | std::ios::app);
    }

    void UninitLogging() {
        log_file_stream_.close();
    }


    Log::Log(const wchar_t* file_name, int line_number, Severity level)
        :level_(level),
        line_number_(line_number),
        file_name_(file_name) {
    }

    Log::~Log() {
        string16 msg;
        msg.append(file_name_)
            .append(L"(")
            .append(std::to_wstring(line_number_))
            .append(L"): ")
            .append(stream_.str())
            .append(L"\n");
        ::OutputDebugStringW(msg.c_str());

        if (log_file_stream_.is_open()) {
            log_file_stream_ << msg << std::flush;
        }

        switch (level_) {
        case Severity::INFO:
            break;
        case Severity::WARNING:
            debugBreak();
            break;
        case Severity::ERR:
            debugBreak();
            break;
        case Severity::FATAL:
            debugBreak();
            break;
        }
    }

    std::wostringstream& Log::stream() {
        return stream_;
    }

    void Log::debugBreak() {
        __debugbreak();
    }

}
