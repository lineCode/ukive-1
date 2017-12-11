#include "cycler.h"

#include "ukive/message/message.h"
#include "ukive/message/message_looper.h"
#include "ukive/system/system_clock.h"
#include "ukive/utils/executable.h"


namespace ukive {

    Cycler::Cycler() {
        looper_ = MessageLooper::myLooper();
    }

    Cycler::Cycler(MessageLooper *looper) {
        looper_ = looper;
    }

    Cycler::~Cycler() {
        looper_->getQueue()->remove(this, nullptr);
    }


    void Cycler::post(Executable *exec) {
        postDelayed(exec, 0);
    }

    void Cycler::postDelayed(Executable *exec, uint64_t millis) {
        postAtTime(exec, millis + SystemClock::upTimeMillis());
    }

    void Cycler::postAtTime(Executable *exec, uint64_t atTimeMillis) {
        Message *msg = Message::obtain();
        msg->callback = exec;

        sendMessageAtTime(msg, atTimeMillis);
    }

    bool Cycler::hasCallbacks(Executable *exec) {
        return looper_->getQueue()->contains(this, exec, nullptr);
    }

    void Cycler::removeCallbacks(Executable *exec) {
        looper_->getQueue()->remove(this, exec, nullptr);
    }


    void Cycler::sendEmptyMessage(int what) {
        sendEmptyMessageDelayed(what, 0);
    }

    void Cycler::sendEmptyMessageDelayed(int what, uint64_t millis) {
        sendEmptyMessageAtTime(what, millis + SystemClock::upTimeMillis());
    }

    void Cycler::sendEmptyMessageAtTime(int what, uint64_t atTimeMillis) {
        Message *msg = Message::obtain();
        msg->what = what;

        sendMessageAtTime(msg, atTimeMillis);
    }

    void Cycler::sendMessage(Message *msg) {
        sendMessageDelayed(msg, 0);
    }

    void Cycler::sendMessageDelayed(Message *msg, uint64_t millis) {
        sendMessageAtTime(msg, millis + SystemClock::upTimeMillis());
    }

    void Cycler::sendMessageAtTime(Message *msg, uint64_t atTimeMillis) {
        msg->when = atTimeMillis;
        enqueueMessage(msg);
    }


    void Cycler::enqueueMessage(Message *msg) {
        msg->target = this;
        looper_->getQueue()->enqueue(msg);
    }


    bool Cycler::hasMessages(int what) {
        return looper_->getQueue()->contains(this, what, nullptr);
    }

    bool Cycler::hasMessages(int what, void *data) {
        return looper_->getQueue()->contains(this, what, data);
    }

    void Cycler::removeMessages(int what) {
        looper_->getQueue()->remove(this, what, nullptr);
    }

    void Cycler::removeMessages(int what, void *data) {
        looper_->getQueue()->remove(this, what, data);
    }


    void Cycler::dispatchMessage(Message *msg) {
        if (msg->callback) {
            msg->callback->run();
        }
        else {
            handleMessage(msg);
        }
    }

    void Cycler::handleMessage(Message *msg) {
    }

}