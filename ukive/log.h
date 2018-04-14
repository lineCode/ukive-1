#ifndef UKIVE_LOG_H_
#define UKIVE_LOG_H_

#include <string>
#include <sstream>

#include "utils/string_utils.h"


#define LOG_STREAM(level) \
    ::ukive::Log(__FILE__, __LINE__, level).stream()

#define VOIDABLE_STREAM(stream, condition) \
    !(condition) ? (void) 0 : ::ukive::LogVoidify() & (stream)

#define IS_LOG_ON   true
#define IS_CHECK_ON true

#ifndef NDEBUG

#define IS_DLOG_ON   true
#define IS_DCHECK_ON true

#else

#define IS_DLOG_ON   false
#define IS_DCHECK_ON false

#endif

#define LOG(level) \
    VOIDABLE_STREAM(LOG_STREAM(level), IS_LOG_ON)

#define CHECK(condition) \
    VOIDABLE_STREAM(LOG_STREAM(::ukive::Log::FATAL), IS_CHECK_ON && !(condition))

#define DLOG(level) \
    VOIDABLE_STREAM(LOG_STREAM(level), IS_DLOG_ON)

#define DCHECK(condition) \
    VOIDABLE_STREAM(LOG_STREAM(::ukive::Log::FATAL), IS_DCHECK_ON && !(condition))


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

    class LogVoidify {
    public:
        LogVoidify() = default;
        void operator&(std::ostream& s) {}
    };

}

#endif  // UKIVE_LOG_H_