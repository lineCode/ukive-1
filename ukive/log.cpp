#include "log.h"

#include <Windows.h>

#include <fstream>

#include "ukive/application.h"
#include "ukive/files/file.h"


namespace ukive {

    std::ofstream log_file_stream_;

    void InitLogging() {
        auto exec_dir = Application::getExecFileName(true);
        File log_file(exec_dir, L"Debug.log");
        log_file_stream_.open(log_file.getPath().c_str(), std::ios::out | std::ios::app);
        log_file_stream_.imbue(std::locale("en_US.UTF-8"));
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
            log_file_stream_ << UTF16ToUTF8(msg) << std::flush;
        }

        switch (level_) {
        case Severity::INFO:
            break;
        case Severity::WARNING:
            debugBreakIfInDebugger();
            break;
        case Severity::ERR:
            debugBreakIfInDebugger();
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

    void Log::debugBreakIfInDebugger() {
        if (::IsDebuggerPresent() != 0) {
            __debugbreak();
        }
    }

}
