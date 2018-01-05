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


    void Log::debugBreak() {
        __debugbreak();
    }

}