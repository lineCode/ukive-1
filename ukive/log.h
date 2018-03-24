#ifndef UKIVE_LOG_H_
#define UKIVE_LOG_H_

#include <string>
#include <sstream>

#include "utils/string_utils.h"


#define LOG(level) ukive::Log(__FILE__, __LINE__, level).stream()

#define CHECK(exp) { \
    bool result = (exp); \
    if (!result) { ukive::Log::debugBreak(); } \
}

#ifndef NDEBUG

#define DLOG(level) LOG(level)
#define DCHECK(exp) CHECK(exp)

#else

#define DLOG(level)
#define DCHECK(exp)

#endif


namespace ukive {

    class Log {
    public:
        enum Severity {
            INFO,
            WARNING,
            FATAL
        };

        static void i(const string16 &tag, const string16 &msg);
        static void d(const string16 &tag, const string16 &msg);
        static void w(const string16 &tag, const string16 &msg);
        static void e(const string16 &tag, const string16 &msg);
        static void v(const string16 &tag, const string16 &msg);

        static void debugBreak();

        Log(const char *file_name, int line_number, Severity level);
        ~Log();

        std::ostringstream& stream();

    private:
        Severity level_;
        int line_number_;
        std::string file_name_;
        std::ostringstream stream_;
    };

}

#endif  // UKIVE_LOG_H_