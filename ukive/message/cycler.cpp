#include "cycler.h"

#include "ukive/message/message.h"
#include "ukive/message/message_looper.h"
#include "ukive/system/system_clock.h"
#include "ukive/utils/executable.h"


namespace ukive {

    Cycler::Cycler() {
        mLooper = MessageLooper::myLooper();
    }

    Cycler::Cycler(MessageLooper *looper) {
        mLooper = looper;
    }

    Cycler::~Cycler() {
        mLooper->getQueue()->remove(this, 0);
    }


    void Cycler::post(Executable *exec) {
        this->postDelayed(exec, 0);
    }

    void Cycler::postDelayed(Executable *exec, uint64_t millis) {
        this->postAtTime(exec, millis + SystemClock::upTimeMillis());
    }

    void Cycler::postAtTime(Executable *exec, uint64_t atTimeMillis) {
        Message *msg = Message::obtain();
        msg->callback = exec;

        this->sendMessageAtTime(msg, atTimeMillis);
    }

    bool Cycler::hasCallbacks(Executable *exec)
    {
        return mLooper->getQueue()->contains(this, exec, 0);
    }

    void Cycler::removeCallbacks(Executable *exec)
    {
        mLooper->getQueue()->remove(this, exec, 0);
    }


    void Cycler::sendEmptyMessage(int what)
    {
        this->sendEmptyMessageDelayed(what, 0);
    }

    void Cycler::sendEmptyMessageDelayed(int what, uint64_t millis)
    {
        this->sendEmptyMessageAtTime(what, millis + SystemClock::upTimeMillis());
    }

    void Cycler::sendEmptyMessageAtTime(int what, uint64_t atTimeMillis)
    {
        Message *msg = Message::obtain();
        msg->what = what;

        this->sendMessageAtTime(msg, atTimeMillis);
    }

    void Cycler::sendMessage(Message *msg)
    {
        this->sendMessageDelayed(msg, 0);
    }

    void Cycler::sendMessageDelayed(Message *msg, uint64_t millis)
    {
        this->sendMessageAtTime(msg, millis + SystemClock::upTimeMillis());
    }

    void Cycler::sendMessageAtTime(Message *msg, uint64_t atTimeMillis)
    {
        msg->when = atTimeMillis;
        this->enqueueMessage(msg);
    }


    void Cycler::enqueueMessage(Message *msg)
    {
        msg->target = this;
        mLooper->getQueue()->enqueue(msg);
    }


    bool Cycler::hasMessages(int what)
    {
        return mLooper->getQueue()->contains(this, what, 0);
    }

    bool Cycler::hasMessages(int what, void *data)
    {
        return mLooper->getQueue()->contains(this, what, data);
    }

    void Cycler::removeMessages(int what)
    {
        mLooper->getQueue()->remove(this, what, 0);
    }

    void Cycler::removeMessages(int what, void *data)
    {
        mLooper->getQueue()->remove(this, what, data);
    }


    void Cycler::dispatchMessage(Message *msg)
    {
        if (msg->callback)
            msg->callback->run();
        else
            this->handleMessage(msg);
    }

    void Cycler::handleMessage(Message *msg)
    {
    }

}