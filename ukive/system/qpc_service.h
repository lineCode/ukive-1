#ifndef UKIVE_SYSTEM_QPC_SERVICE_H_
#define UKIVE_SYSTEM_QPC_SERVICE_H_

#include <cstdint>

namespace ukive {

    class QPCService {
    public:
        QPCService();
        ~QPCService() = default;

        void start();
        uint64_t stop();

        static uint64_t getTimeStampUs();

    private:
        static bool fetchPerformanceFrequency();

        uint64_t starting_time_;
        static thread_local uint64_t frequency_;
    };

    class MMTimerService {
    public:
        MMTimerService();
        ~MMTimerService() = default;

        void start();
        uint32_t stop();

        static void enableHighResTimer();
        static void disableHighResTimer();

        static uint32_t getTimeStamp();

    private:
        uint32_t starting_time_;

        static int timer_res_;
        static int op_counter_;
    };
}

#endif  // UKIVE_SYSTEM_QPC_SERVICE_H_