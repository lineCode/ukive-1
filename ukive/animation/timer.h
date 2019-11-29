#ifndef UKIVE_ANIMATION_TIMER_H_
#define UKIVE_ANIMATION_TIMER_H_

#include <functional>

#include "utils/weak_ref_nest.hpp"

#include "ukive/message/cycler.h"


namespace ukive {

    class Timer {
    public:
        using Runner = std::function<void()>;

        Timer();
        ~Timer();

        void start();
        void stop();

        void setRepeat(bool repeat);
        void setRunner(const Runner& runner);
        void setDuration(uint64_t duration);

        bool isRepeat() const;
        bool isRunning() const;
        uint64_t getDuration() const;

    private:
        void onTimer();
        void postToMessageLoop();

        uint64_t duration_;
        bool is_repeat_;
        bool is_running_;
        Runner runner_;

        Cycler cycler_;
        utl::WeakRefNest<Timer> weak_ref_nest_;
    };

}

#endif  // UKIVE_ANIMATION_TIMER_H_