#include "message_queue.h"

#include "ukive/log.h"
#include "ukive/message/message.h"
#include "ukive/system/system_clock.h"


namespace ukive {

    MessageQueue::MessageQueue()
        :message_(nullptr),
        is_quitting_(false),
        has_barrier_(false) {
    }


    MessageQueue::~MessageQueue() {
    }


    void MessageQueue::quit() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        is_quitting_ = true;
        removeAllLocked();
    }


    bool MessageQueue::enqueue(Message* msg) {
        if (!msg->target) {
            LOG(Log::WARNING) << "Message must have a target!";
            msg->recycle();
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        if (is_quitting_) {
            msg->recycle();
            return false;
        }

        Message* ptr = message_;

        if (!ptr || !ptr->target
            || msg->when == 0 || msg->when < ptr->when) {
            msg->next = ptr;
            message_ = msg;
        } else {
            Message* prev = nullptr;
            while (true) {
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

    Message* MessageQueue::dequeue() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        uint64_t now = SystemClock::upTimeMillis();
        Message* prev = nullptr;
        Message* ptr = message_;

        //find barrier.
        while (ptr && ptr->target) {
            prev = ptr;
            ptr = ptr->next;
        }

        if (!ptr) {
            throw std::logic_error("MessageQueue-dequeue(): cannot find barrier.");
        }

        //over the barrier.
        prev = ptr;
        ptr = ptr->next;

        while (ptr) {
            if (ptr->when <= now) {
                prev->next = ptr->next;
                Message* msg = ptr;
                return msg;
            }

            prev = ptr;
            ptr = ptr->next;
        }

        return nullptr;
    }


    void MessageQueue::remove(Cycler* c, void* data) {
        if (c == nullptr) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c
                && (data == nullptr || ptr->data == data)) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    message_ = ptr->next;
                }

                Message* msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::remove(Cycler* c, int what, void* data) {
        if (c == nullptr) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c
                && ptr->what == what
                && (data == nullptr || ptr->data == data)) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    message_ = ptr->next;
                }

                Message* msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::remove(Cycler* c, Executable* exec, void* data) {
        if (c == nullptr) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c
                && ptr->callback == exec
                && (data == nullptr || ptr->data == data)) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    message_ = ptr->next;
                }

                Message* msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::removeAllLocked() {
        Message* ptr = message_;
        Message* msg = nullptr;
        while (ptr) {
            msg = ptr;
            ptr = ptr->next;
            msg->recycle();
        }

        message_ = nullptr;
    }


    bool MessageQueue::contains(Cycler* c, int what, void* data) {
        if (c == nullptr) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
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

    bool MessageQueue::contains(Cycler* c, Executable* exec, void* data) {
        if (c == nullptr) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
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
        std::lock_guard<std::mutex> lk(queue_sync_);

        if (has_barrier_) {
            return;
        }

        Message* barrier = Message::obtain();
        if (message_) {
            barrier->next = message_;
        }

        message_ = barrier;
        has_barrier_ = true;
    }

    void MessageQueue::removeBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == nullptr) {
                if (prev) {
                    prev->next = ptr->next;
                }
                else {
                    message_ = ptr->next;
                }

                Message* msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                break;
            }

            prev = ptr;
            ptr = ptr->next;
        }

        has_barrier_ = false;
    }

}