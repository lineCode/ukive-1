#ifndef UKIVE_LOG_H_
#define UKIVE_LOG_H_

#include "utils/string_utils.h"

namespace ukive {

    class Log {

        static void i(const string16 &msg);
        static void d(const string16 &msg);
        static void w(const string16 &msg);
        static void e(const string16 &msg);
        static void v(const string16 &msg);

        static void debugBreak();

    };

}

#endif  // UKIVE_LOG_H_