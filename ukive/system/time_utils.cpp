#include "time_utils.h"

#include <algorithm>

#include <Windows.h>

#include "ukive/log.h"


namespace ukive {

    int TimeUtils::timer_res_ = 0;
    int TimeUtils::op_counter_ = 0;


    TimeUtils::TimeUtils() {
    }

    TimeUtils::~TimeUtils() {
    }

    uint64_t TimeUtils::upTimeMillis() {
        return ::GetTickCount64();
    }

    void TimeUtils::enableHighResTimer() {
        TIMECAPS tc;
        if (::timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
            LOG(Log::WARNING) << "Failed to get timer res!";
            return;
        }

        timer_res_ = std::min(std::max(tc.wPeriodMin, 1U), tc.wPeriodMax);

        auto result = ::timeBeginPeriod(timer_res_);
        if (result != TIMERR_NOERROR) {
            LOG(Log::WARNING) << "Failed to enable high res timer! Res: " << timer_res_;
            return;
        }

        ++op_counter_;
    }

    void TimeUtils::disableHighResTimer() {
        if (op_counter_ <= 0) {
            DCHECK(false) << "Wrong counter: " << op_counter_;
            return;
        }

        auto result = ::timeEndPeriod(timer_res_);
        if (result != TIMERR_NOERROR) {
            LOG(Log::WARNING) << "Failed to disable high res timer! Res: " << timer_res_;
            return;
        }

        --op_counter_;
    }
}