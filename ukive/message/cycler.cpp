#include "cycler.h"

#include "utils/log.h"
#include "utils/executable.h"

#include "ukive/message/message.h"
#include "ukive/message/message_looper.h"
#include "ukive/system/time_utils.h"


namespace ukive {

    Cycler::Cycler(TimeUnit unit)
        : time_unit_(unit),
          looper_(MessageLooper::myLooper()),
          listener_(nullptr) {
    }

    Cycler::Cycler(MessageLooper* looper, TimeUnit unit)
        : time_unit_(unit),
          looper_(looper),
          listener_(nullptr) {
    }

    Cycler::~Cycler() {
        looper_->getQueue()->remove(this);
    }

    void Cycler::setListener(CyclerListener* l) {
        listener_ = l;
    }

    void Cycler::post(utl::Executable* exec) {
        postDelayed(exec, 0);
    }

    void Cycler::postDelayed(utl::Executable* exec, uint64_t delay) {
        postAtTime(exec, delay + now());
    }

    void Cycler::postAtTime(utl::Executable* exec, uint64_t at_time) {
        Message* msg = Message::obtain();
        msg->callback = exec;

        sendMessageAtTime(msg, at_time);
    }

    void Cycler::post(const std::function<void()>& func, int what) {
        postDelayed(func, 0, what);
    }

    void Cycler::postDelayed(const std::function<void()>& func, uint64_t delay, int what) {
        postAtTime(func, delay + now(), what);
    }

    void Cycler::postAtTime(const std::function<void()>& func, uint64_t at_time, int what) {
        Message* msg = Message::obtain();
        msg->func = func;
        msg->what = what;

        sendMessageAtTime(msg, at_time);
    }

    bool Cycler::hasCallbacks(utl::Executable* exec) {
        return looper_->getQueue()->contains(this, exec);
    }

    void Cycler::removeCallbacks(utl::Executable* exec) {
        looper_->getQueue()->remove(this, exec);
    }

    void Cycler::sendEmptyMessage(int what) {
        sendEmptyMessageDelayed(what, 0);
    }

    void Cycler::sendEmptyMessageDelayed(int what, uint64_t delay) {
        sendEmptyMessageAtTime(what, delay + now());
    }

    void Cycler::sendEmptyMessageAtTime(int what, uint64_t at_time) {
        Message* msg = Message::obtain();
        msg->what = what;

        sendMessageAtTime(msg, at_time);
    }

    void Cycler::sendMessage(Message* msg) {
        sendMessageDelayed(msg, 0);
    }

    void Cycler::sendMessageDelayed(Message* msg, uint64_t delay) {
        sendMessageAtTime(msg, delay + now());
    }

    void Cycler::sendMessageAtTime(Message* msg, uint64_t at_time) {
        msg->when = at_time;
        enqueueMessage(msg);
    }

    void Cycler::enqueueMessage(Message* msg) {
        msg->target = this;
        looper_->getQueue()->enqueue(msg);
        looper_->wakeup();
    }

    bool Cycler::hasMessages(int what) {
        return looper_->getQueue()->contains(this, what);
    }

    void Cycler::removeMessages(int what) {
        looper_->getQueue()->remove(this, what);
    }

    void Cycler::dispatchMessage(Message* msg) {
        if (msg->callback) {
            msg->callback->run();
        } else if (msg->func) {
            msg->func();
        } else {
            if (listener_) {
                listener_->onHandleMessage(msg);
            }
        }
    }

    uint64_t Cycler::now() const {
        switch (time_unit_) {
        case Millis:
            return TimeUtils::upTimeMillis();
        case MillisPrecise:
            return TimeUtils::upTimeMillisPrecise();
        case Micros:
            return TimeUtils::upTimeMicros();
        }

        DCHECK(false);
        return 0;
    }

}