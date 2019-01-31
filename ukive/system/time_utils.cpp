#include "ukive/system/time_utils.h"

#include <Windows.h>

#include "ukive/system/qpc_service.h"


namespace ukive {

    TimeUtils::TimeUtils() {
    }

    TimeUtils::~TimeUtils() {
    }

    uint64_t TimeUtils::upTimeMillis() {
        return ::GetTickCount64();
    }

    uint64_t TimeUtils::upTimeMillisPrecise() {
        return QPCService::getTimeStampUs() / 1000;
    }

    uint64_t TimeUtils::upTimeMicros() {
        return QPCService::getTimeStampUs();
    }

}
