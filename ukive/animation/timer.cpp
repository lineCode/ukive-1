#include "ukive/animation/timer.h"

#include "ukive/utils/weak_bind.h"


namespace ukive {

    Timer::Timer()
        : duration_(0),
          is_repeat_(false),
          is_running_(false),
          weak_ref_nest_(this) {}

    void Timer::start() {
        auto callback = weakref_bind(&Timer::onTimer, weak_ref_nest_.getRef());
        cycler_.postDelayed(callback, duration_);
        is_running_ = true;
    }

    void Timer::stop() {
        weak_ref_nest_.revoke();
        is_running_ = false;
    }

    void Timer::setRepeat(bool repeat) {
        is_repeat_ = repeat;
    }

    void Timer::setRunner(const Runner& runner) {
        runner_ = runner;
    }

    void Timer::setDuration(int duration) {
        duration_ = duration;
    }

    bool Timer::isRepeat() const {
        return is_repeat_;
    }

    bool Timer::isRunning() const {
        return is_running_;
    }

    int Timer::getDuration() const {
        return duration_;
    }

    void Timer::onTimer() {
        if (runner_) {
            runner_();
        }

        if (is_repeat_) {
            start();
        } else {
            is_running_ = false;
        }
    }

}