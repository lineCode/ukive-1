#include "log.h"

#include <intrin.h>


namespace ukive {

    void Log::i(const string16 &msg) {

    }

    void Log::d(const string16 &msg) {

    }

    void Log::w(const string16 &msg) {

    }

    void Log::e(const string16 &msg) {
        debugBreak();
    }

    void Log::v(const string16 &msg) {

    }


    void Log::debugBreak() {
        __debugbreak();
    }

}