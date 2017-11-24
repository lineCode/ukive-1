#include "system_clock.h"


namespace ukive {

    SystemClock::SystemClock() {
    }


    SystemClock::~SystemClock() {
    }


    ULONG64 SystemClock::upTimeMillis() {
        return ::GetTickCount64();
    }

}