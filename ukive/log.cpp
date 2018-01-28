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

    Log::Log(const char *file_name, int line_number, int level)
        :level_(level),
        file_name_(file_name),
        line_number_(line_number) {
    }

    Log::~Log() {
        std::string msg;
        msg.append(file_name_)
            .append("(")
            .append(std::to_string(line_number_))
            .append("): ")
            .append(stream_.str())
            .append("\n");
        ::OutputDebugStringA(msg.c_str());
    }

    std::ostringstream& Log::stream() {
        return stream_;
    }

    void Log::debugBreak() {
        __debugbreak();
    }

}