#ifndef UKIVE_ANIMATION_TIMER_H_
#define UKIVE_ANIMATION_TIMER_H_

#include <functional>

#include "ukive/message/cycler.h"
#include "ukive/utils/weak_ref_nest.h"


namespace ukive {

    class Timer {
    public:
        using Runner = std::function<void()>;

        Timer();

        void start();
        void stop();

        void setRepeat(bool repeat);
        void setRunner(const Runner& runner);
        void setDuration(int duration);

        bool isRepeat() const;
        bool isRunning() const;
        int getDuration() const;

    private:
        void onTimer();

        int duration_;
        bool is_repeat_;
        bool is_running_;
        Runner runner_;

        Cycler cycler_;
        WeakRefNest<Timer> weak_ref_nest_;
    };

}

#endif  // UKIVE_ANIMATION_TIMER_H_