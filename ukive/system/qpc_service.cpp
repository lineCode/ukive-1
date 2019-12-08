#include "qpc_service.h"

#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>

#include "utils/log.h"


namespace ukive {

    // QPCService
    thread_local uint64_t QPCService::frequency_ = 0;

    QPCService::QPCService()
        : starting_time_(0) {}

    void QPCService::start() {
        if (!fetchPerformanceFrequency()) {
            return;
        }

        LARGE_INTEGER stime = { 0 };
        BOOL result = ::QueryPerformanceCounter(&stime);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            starting_time_ = 0;
            return;
        }

        starting_time_ = stime.QuadPart;
    }

    uint64_t QPCService::stop() {
        LARGE_INTEGER ending_time = { 0 };

        BOOL result = ::QueryPerformanceCounter(&ending_time);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return 0;
        }

        uint64_t ucount = ending_time.QuadPart - starting_time_;
        uint64_t freq = frequency_;

        uint64_t prev = ucount / freq;
        uint64_t prev_re = ucount % freq;
        uint64_t next = (prev_re * 1000000) / freq;

        return prev * 1000000 + next;
    }

    // static
    uint64_t QPCService::getTimeStampUs() {
        if (!fetchPerformanceFrequency()) {
            return 0;
        }

        LARGE_INTEGER count = { 0 };
        BOOL result = ::QueryPerformanceCounter(&count);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return 0;
        }

        uint64_t ucount = count.QuadPart;
        uint64_t freq = frequency_;

        uint64_t prev = ucount / freq;
        uint64_t prev_re = ucount % freq;
        uint64_t next = (prev_re * 1000000) / freq;

        return prev * 1000000 + next;
    }

    // static
    bool QPCService::fetchPerformanceFrequency() {
        if (frequency_ == 0) {
            LARGE_INTEGER freq = { 0 };
            BOOL result = ::QueryPerformanceFrequency(&freq);
            if (result == 0) {
                LOG(Log::WARNING) << "Failed to get QPF: " << ::GetLastError();
                return false;
            }
            frequency_ = freq.QuadPart;
        }
        return true;
    }


    // MMTimerService
    int MMTimerService::timer_res_ = 0;
    int MMTimerService::op_counter_ = 0;

    MMTimerService::MMTimerService()
        : starting_time_(0) {}

    void MMTimerService::start() {
        starting_time_ = ::timeGetTime();
    }

    uint32_t MMTimerService::stop() {
        uint32_t ending_time = ::timeGetTime();
        return ending_time - starting_time_;
    }

    // static
    void MMTimerService::enableHighResTimer() {
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

    // static
    void MMTimerService::disableHighResTimer() {
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

    // static
    uint32_t MMTimerService::getTimeStamp() {
        return ::timeGetTime();
    }
}
