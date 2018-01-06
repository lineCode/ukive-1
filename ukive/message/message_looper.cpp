#include "message_looper.h"

#include "ukive/message/cycler.h"
#include "ukive/message/message.h"
#include "ukive/message/message_queue.h"


namespace ukive {

    std::mutex MessageLooper::mLooperSync;
    MessageLooper *MessageLooper::mMainLooper = nullptr;
    thread_local std::shared_ptr<MessageLooper> MessageLooper::looper_;


    MessageLooper::MessageLooper() {
        mMsgQueue = new MessageQueue();
    }

    MessageLooper::~MessageLooper() {
        delete mMsgQueue;
    }


    void MessageLooper::quit() {
        mMsgQueue->quit();
    }

    MessageQueue *MessageLooper::getQueue() {
        return mMsgQueue;
    }


    void MessageLooper::prepare() {
        std::lock_guard<std::mutex> lk(mLooperSync);

        if (looper_) {
            throw std::logic_error("MessageLooper-prepare(): Only one Looper may be created per thread");
        }

        looper_.reset(new MessageLooper());
    }

    void MessageLooper::prepareMainLooper() {
        prepare();
        std::lock_guard<std::mutex> lk(mLooperSync);

        if (mMainLooper)
            throw std::logic_error("The main Looper has already been prepared.");
        mMainLooper = myLooper();
    }

    void MessageLooper::loop() {
        MessageLooper *looper = myLooper();
        if (!looper)
            throw std::logic_error(
                "MessageLooper-loop(): No Looper; Looper.prepare() wasn't called on this thread.");

        MessageQueue *queue = looper->getQueue();

        queue->addBarrier();

        while (true) {
            Message *msg = queue->dequeue();
            if (!msg)
                break;

            msg->target->dispatchMessage(msg);
            msg->recycle();
        }

        queue->removeBarrier();
    }

    MessageQueue *MessageLooper::myQueue() {
        return myLooper()->getQueue();
    }

    MessageLooper *MessageLooper::myLooper() {
        return looper_.get();
    }

    MessageLooper *MessageLooper::getMainLooper() {
        std::lock_guard<std::mutex> lk(mLooperSync);
        return mMainLooper;
    }


    void MessageLooper::receiveMessage() {
        mMsgQueue->addBarrier();

        while (true) {
            Message *msg = mMsgQueue->dequeue();
            if (msg == 0)
                break;

            msg->target->dispatchMessage(msg);
        }

        mMsgQueue->removeBarrier();
    }

}