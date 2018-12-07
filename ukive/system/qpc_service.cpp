#include "qpc_service.h"

#include "ukive/log.h"


namespace ukive {

    // QPCService
    LARGE_INTEGER QPCService::frequency_ = { 0 };

    QPCService::QPCService()
        :starting_time_({ 0 }) {}


    void QPCService::Start() {
        starting_time_ = { 0 };

        BOOL result = TRUE;
        if (frequency_.QuadPart == 0) {
            result = ::QueryPerformanceFrequency(&frequency_);
            if (result == 0) {
                LOG(Log::WARNING) << "Failed to get QPF: " << ::GetLastError();
                return;
            }
        }

        result = ::QueryPerformanceCounter(&starting_time_);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return;
        }
    }

    uint64_t QPCService::Stop() {
        LARGE_INTEGER ending_time = { 0 };
        LARGE_INTEGER elapsed_microseconds = { 0 };

        BOOL result = ::QueryPerformanceCounter(&ending_time);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return 0;
        }

        elapsed_microseconds.QuadPart = ending_time.QuadPart - starting_time_.QuadPart;

        elapsed_microseconds.QuadPart *= 1000000;
        elapsed_microseconds.QuadPart /= frequency_.QuadPart;

        return elapsed_microseconds.QuadPart;
    }


    // MMTimerService
    MMTimerService::MMTimerService()
        :starting_time_(0) {}

    void MMTimerService::Start() {
        starting_time_ = ::timeGetTime();
    }

    uint64_t MMTimerService::Stop() {
        uint32_t ending_time = ::timeGetTime();
        return ending_time - starting_time_;
    }
}