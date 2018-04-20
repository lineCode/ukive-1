#ifndef UKIVE_SYSTEM_TIME_UTILS_H_
#define UKIVE_SYSTEM_TIME_UTILS_H_

#include <Windows.h>


namespace ukive {

    class TimeUtils {
    public:
        TimeUtils();
        ~TimeUtils();

        static ULONG64 upTimeMillis();
    };

}

#endif  // UKIVE_SYSTEM_TIME_UTILS_H_