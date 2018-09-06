#include "log.h"

#include <intrin.h>
#include <Windows.h>


namespace ukive {

    void Log::i(const string16 &tag, const string16 &msg) {
        ::OutputDebugString(msg.c_str());
    }

    void Log::d(const string16 &tag, const string16 &msg) {
        if (tag == L"TsfEditor") {
            return;
        }
        ::OutputDebugString(msg.c_str());
    }

    void Log::w(const string16 &tag, const string16 &msg) {
        ::OutputDebugString(msg.c_str());
    }

    void Log::e(const string16 &tag, const string16 &msg) {
        ::OutputDebugString(msg.c_str());
        debugBreak();
    }

    void Log::v(const string16 &tag, const string16 &msg) {
        ::OutputDebugString(msg.c_str());
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