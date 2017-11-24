#include "message_queue.h"

#include "ukive/message/message.h"
#include "ukive/system/system_clock.h"


namespace ukive {

    MessageQueue::MessageQueue()
        :mMessage(nullptr),
        mQuitting(false),
        mHasBarrier(false) {
    }


    MessageQueue::~MessageQueue() {
    }


    void MessageQueue::quit() {
        std::lock_guard<std::mutex> lk(mQueueSync);

        mQuitting = true;
        this->removeAllLocked();
    }


    bool MessageQueue::enqueue(Message *msg) {
        if (!msg->target) {
            throw std::logic_error(
                "MessageQueue-enqueue(): Message must have a target.");
        }

        std::lock_guard<std::mutex> lk(mQueueSync);

        if (mQuitting) {
            msg->recycle();
            return false;
        }

        Message *ptr = mMessage;

        if (!ptr || !ptr->target
            || msg->when == 0 || msg->when < ptr->when) {
            msg->next = ptr;
            mMessage = msg;
        }
        else {
            Message *prev = 0;
            while (true)
            {
                prev = ptr;
                ptr = ptr->next;
                if (!ptr || !ptr->target
                    || msg->when < ptr->when) {
                    break;
                }
            }

            msg->next = ptr;
            prev->next = msg;
        }

        return true;
    }

    Message *MessageQueue::dequeue() {
        std::lock_guard<std::mutex> lk(mQueueSync);

        uint64_t now = SystemClock::upTimeMillis();
        Message *prev = 0;
        Message *ptr = mMessage;

        //find barrier.
        while (ptr && ptr->target) {
            prev = ptr;
            ptr = ptr->next;
        }

        if (ptr == 0) {
            throw std::logic_error("MessageQueue-dequeue(): cannot find barrier.");
        }

        //over the barrier.
        prev = ptr;
        ptr = ptr->next;

        while (ptr) {
            if (ptr->when <= now) {
                prev->next = ptr->next;
                Message *msg = ptr;
                return msg;
            }

            prev = ptr;
            ptr = ptr->next;
        }

        return 0;
    }


    void MessageQueue::remove(Cycler *c, void *data) {
        if (c == 0) {
            return;
        }

        std::lock_guard<std::mutex> lk(mQueueSync);

        Message *ptr = mMessage;
        Message *prev = 0;
        while (ptr) {
            if (ptr->target == c
                && (data == 0 || ptr->data == data)) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    mMessage = ptr->next;
                }

                Message *msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::remove(Cycler *c, int what, void *data) {
        if (c == 0) {
            return;
        }

        std::lock_guard<std::mutex> lk(mQueueSync);

        Message *ptr = mMessage;
        Message *prev = 0;
        while (ptr) {
            if (ptr->target == c
                && ptr->what == what
                && (data == 0 || ptr->data == data)) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    mMessage = ptr->next;
                }

                Message *msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::remove(Cycler *c, Executable *exec, void *data) {
        if (c == 0)
            return;

        std::lock_guard<std::mutex> lk(mQueueSync);

        Message *ptr = mMessage;
        Message *prev = 0;
        while (ptr) {
            if (ptr->target == c
                && ptr->callback == exec
                && (data == 0 || ptr->data == data)) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    mMessage = ptr->next;
                }

                Message *msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::removeAllLocked() {
        Message *ptr = mMessage;
        Message *msg = nullptr;
        while (ptr) {
            msg = ptr;
            ptr = ptr->next;
            msg->recycle();
        }

        mMessage = nullptr;
    }


    bool MessageQueue::contains(Cycler *c, int what, void *data) {
        if (c == nullptr) {
            return false;
        }

        std::lock_guard<std::mutex> lk(mQueueSync);

        Message *ptr = mMessage;
        while (ptr) {
            if (ptr->target == c
                && ptr->what == what
                && (data == nullptr || ptr->data == data)) {
                return true;
            }
            ptr = ptr->next;
        }

        return false;
    }

    bool MessageQueue::contains(Cycler *c, Executable *exec, void *data) {
        if (c == nullptr) {
            return false;
        }

        std::lock_guard<std::mutex> lk(mQueueSync);

        Message *ptr = mMessage;
        while (ptr) {
            if (ptr->target == c
                && ptr->callback == exec
                && (data == nullptr || ptr->data == data)) {
                return true;
            }
            ptr = ptr->next;
        }

        return false;
    }


    void MessageQueue::addBarrier() {
        std::lock_guard<std::mutex> lk(mQueueSync);

        if (mHasBarrier) {
            return;
        }

        Message *barrier = Message::obtain();
        if (mMessage) {
            barrier->next = mMessage;
        }

        mMessage = barrier;
        mHasBarrier = true;
    }

    void MessageQueue::removeBarrier() {
        std::lock_guard<std::mutex> lk(mQueueSync);

        Message *ptr = mMessage;
        Message *prev = 0;
        while (ptr) {
            if (ptr->target == 0) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    mMessage = ptr->next;
                }

                Message *msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                break;
            }

            prev = ptr;
            ptr = ptr->next;
        }

        mHasBarrier = false;
    }

}