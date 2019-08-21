#include "ukive/log.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>

#include "ukive/application.h"
#include "ukive/files/file.h"


namespace ukive {

    bool is_alloc_console_ = false;
    Log::Params log_params_;
    std::ofstream log_file_stream_;
    HANDLE console_output_handle_ = INVALID_HANDLE_VALUE;

    void InitLogging(const Log::Params& params) {
        log_params_ = params;
        if (log_params_.file_name.empty()) {
            log_params_.file_name = L"Debug.log";
        }

        if (log_params_.target & Log::OutputTarget::FILE) {
            auto exec_dir = Application::getExecFileName(true);
            File log_file(exec_dir, log_params_.file_name);
            log_file_stream_.open(log_file.getPath().c_str(), std::ios::out | std::ios::app);
        }
        if (log_params_.target & Log::OutputTarget::CONSOLE) {
            if (::AllocConsole() != 0) {
                is_alloc_console_ = true;
                console_output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
            }
        }
    }

    void UninitLogging() {
        if (log_params_.target & Log::OutputTarget::CONSOLE) {
            if (is_alloc_console_) {
                ::FreeConsole();
                is_alloc_console_ = false;
            }
        }
        if (log_params_.target & Log::OutputTarget::FILE) {
            log_file_stream_.close();
        }
    }

    Log::Log(const wchar_t* file_name, int line_number, Severity level)
        : level_(level),
          line_number_(line_number),
          file_name_(file_name) {
    }

    Log::~Log() {
        if (log_params_.short_file_name) {
            file_name_ = File(file_name_).getName();
        }

        string16 msg;
        msg.append(file_name_)
            .append(L"(")
            .append(std::to_wstring(line_number_))
            .append(L"): ")
            .append(stream_.str())
            .append(L"\n");

        if (log_params_.target & DBG_STR) {
            ::OutputDebugStringW(msg.c_str());
        }
        if (log_params_.target & FILE) {
            if (log_file_stream_.is_open()) {
                log_file_stream_ << UTF16ToUTF8(msg) << std::flush;
            }
        }
        if (log_params_.target & CONSOLE) {
            if (console_output_handle_ != INVALID_HANDLE_VALUE) {
                ::WriteConsoleW(console_output_handle_, msg.data(), msg.length(), nullptr, nullptr);
            }
        }
        if (log_params_.target & STANDARD) {
            std::wcout << msg;
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
