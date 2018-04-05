#ifndef UKIVE_SYSTEM_SYSTEM_CLOCK_H_
#define UKIVE_SYSTEM_SYSTEM_CLOCK_H_

#include <Windows.h>


namespace ukive {

    class SystemClock {
    public:
        SystemClock();
        ~SystemClock();

        static ULONG64 upTimeMillis();
    };

}

#endif  // UKIVE_SYSTEM_SYSTEM_CLOCK_H_