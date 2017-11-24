#include "message_looper.h"

#include "ukive/message/cycler.h"
#include "ukive/message/message.h"
#include "ukive/message/message_queue.h"


namespace ukive {

    std::mutex MessageLooper::mLooperSync;
    MessageLooper *MessageLooper::mMainLooper = nullptr;
    ThreadLocal<std::shared_ptr<MessageLooper>> *MessageLooper::mThreadLocal = nullptr;


    MessageLooper::MessageLooper() {
        mMsgQueue = new MessageQueue();
    }

    MessageLooper::~MessageLooper() {
        delete mMsgQueue;
    }


    void MessageLooper::init() {
        if (mThreadLocal)
            throw std::logic_error("MessageLooper-init(): can only init once.");

        mThreadLocal = new ThreadLocal<std::shared_ptr<MessageLooper>>();
    }

    void MessageLooper::close() {
        if (mThreadLocal) {
            delete mThreadLocal;
            mThreadLocal = nullptr;
        }
    }


    void MessageLooper::quit() {
        mMsgQueue->quit();
    }

    MessageQueue *MessageLooper::getQueue() {
        return mMsgQueue;
    }


    void MessageLooper::prepare() {
        std::lock_guard<std::mutex> lk(mLooperSync);

        std::shared_ptr<MessageLooper> looperPtr;
        if (mThreadLocal->get(looperPtr))
            throw std::logic_error("MessageLooper-prepare(): Only one Looper may be created per thread");

        std::shared_ptr<MessageLooper> newLooper(new MessageLooper());
        mThreadLocal->set(newLooper);
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
        std::shared_ptr<MessageLooper> looperPtr;
        if (mThreadLocal->get(looperPtr))
            return looperPtr.get();

        return nullptr;
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