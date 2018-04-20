#include "time_utils.h"


namespace ukive {

    TimeUtils::TimeUtils() {
    }

    TimeUtils::~TimeUtils() {
    }


    ULONG64 TimeUtils::upTimeMillis() {
        return ::GetTickCount64();
    }

}