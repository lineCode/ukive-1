#ifndef UKIVE_SYSTEM_QPC_SERVICE_H_
#define UKIVE_SYSTEM_QPC_SERVICE_H_

#include <cstdint>

#include <Windows.h>


namespace ukive {

    class QPCService {
    public:
        QPCService();
        ~QPCService() = default;

        void Start();
        uint64_t Stop();

    private:
        LARGE_INTEGER starting_time_;
        static LARGE_INTEGER frequency_;
    };

    class MMTimerService {
    public:
        MMTimerService();
        ~MMTimerService() = default;

        void Start();
        uint64_t Stop();

    private:
        uint32_t starting_time_;
    };
}

#endif  // UKIVE_SYSTEM_QPC_SERVICE_H_