#include "message_looper.h"

#include "ukive/log.h"
#include "ukive/message/cycler.h"
#include "ukive/message/message.h"
#include "ukive/message/message_queue.h"


namespace ukive {

    std::mutex MessageLooper::looper_sync_;
    MessageLooper* MessageLooper::main_looper_ = nullptr;
    thread_local std::shared_ptr<MessageLooper> MessageLooper::looper_;


    MessageLooper::MessageLooper() {
        msg_queue_ = new MessageQueue();
    }

    MessageLooper::~MessageLooper() {
        delete msg_queue_;
    }


    void MessageLooper::quit() {
        msg_queue_->quit();
    }

    MessageQueue* MessageLooper::getQueue() {
        return msg_queue_;
    }


    void MessageLooper::prepare() {
        std::lock_guard<std::mutex> lk(looper_sync_);

        if (looper_) {
            CHECK(false) << "Only one Looper may be created per thread!";
            return;
        }

        looper_.reset(new MessageLooper());
    }

    void MessageLooper::prepareMainLooper() {
        prepare();
        std::lock_guard<std::mutex> lk(looper_sync_);

        if (main_looper_) {
            CHECK(false) << "The main Looper has already been prepared!";
        }
        main_looper_ = myLooper();
    }

    bool MessageLooper::loop() {
        MessageLooper* looper = myLooper();
        if (!looper) {
            CHECK(false) << "No Looper; Looper.prepare() wasn't called on this thread!";
        }

        MessageQueue* queue = looper->getQueue();

        queue->addBarrier();

        while (true) {
            Message* msg = queue->dequeue();
            if (!msg) {
                break;
            }

            msg->target->dispatchMessage(msg);
            msg->recycle();
        }

        queue->removeBarrier();

        return queue->hasMessage();
    }

    MessageQueue* MessageLooper::myQueue() {
        return myLooper()->getQueue();
    }

    MessageLooper* MessageLooper::myLooper() {
        DCHECK(looper_);
        return looper_.get();
    }

    MessageLooper* MessageLooper::getMainLooper() {
        std::lock_guard<std::mutex> lk(looper_sync_);
        return main_looper_;
    }


    void MessageLooper::receiveMessage() {
        msg_queue_->addBarrier();

        while (true) {
            Message* msg = msg_queue_->dequeue();
            if (!msg) {
                break;
            }

            msg->target->dispatchMessage(msg);
        }

        msg_queue_->removeBarrier();
    }

}