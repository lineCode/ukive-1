#include "qpc_service.h"

#include "ukive/log.h"


namespace ukive {

    LARGE_INTEGER QPCService::frequency_ = { 0 };

    QPCService::QPCService()
        :starting_time_({ 0 }),
        ending_time_({ 0 }),
        elapsed_microseconds_({ 0 }) {}


    void QPCService::Start() {
        starting_time_ = { 0 };
        ending_time_ = { 0 };
        elapsed_microseconds_ = { 0 };

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
        BOOL result = ::QueryPerformanceCounter(&ending_time_);
        if (result == 0) {
            LOG(Log::WARNING) << "Failed to get QPC: " << ::GetLastError();
            return 0;
        }

        elapsed_microseconds_.QuadPart = ending_time_.QuadPart - starting_time_.QuadPart;

        elapsed_microseconds_.QuadPart *= 1000000;
        elapsed_microseconds_.QuadPart /= frequency_.QuadPart;

        return elapsed_microseconds_.QuadPart;
    }

}