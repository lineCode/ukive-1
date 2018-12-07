#ifndef UKIVE_SYSTEM_TIME_UTILS_H_
#define UKIVE_SYSTEM_TIME_UTILS_H_

#include <cstdint>


namespace ukive {

    class TimeUtils {
    public:
        TimeUtils();
        ~TimeUtils();

        static uint64_t upTimeMillis();

        static void enableHighResTimer();
        static void disableHighResTimer();

    private:
        static int timer_res_;
        static int op_counter_;
    };

}

#endif  // UKIVE_SYSTEM_TIME_UTILS_H_