#include "message_queue.h"

#include "utils/log.h"

#include "ukive/message/cycler.h"
#include "ukive/message/message.h"


namespace ukive {

    MessageQueue::MessageQueue()
        : is_quitting_(false),
          has_barrier_(false),
          message_(nullptr) {}

    MessageQueue::~MessageQueue() {
    }

    void MessageQueue::quit() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        is_quitting_ = true;
        removeAllLocked();
    }

    bool MessageQueue::hasMessage() {
        std::lock_guard<std::mutex> lk(queue_sync_);
        return message_ != nullptr;
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

        if (!ptr ||
            !ptr->target ||
            msg->when == 0 ||
            msg->when < ptr->when)
        {
            msg->next = ptr;
            message_ = msg;
        } else {
            Message* prev;
            while (true) {
                prev = ptr;
                ptr = ptr->next;
                if (!ptr || !ptr->target || msg->when < ptr->when) {
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

        Message* ptr = message_;

        // find barrier
        while (ptr && ptr->target) {
            ptr = ptr->next;
        }

        CHECK(ptr) << "Cannot find barrier!";

        // over the barrier
        Message* prev = ptr;
        ptr = ptr->next;

        while (ptr) {
            if (ptr->when <= ptr->target->now()) {
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
        if (!c) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c &&
                (!data || ptr->data == data))
            {
                if (prev) {
                    prev->next = ptr->next;
                } else {
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
        if (!c) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c &&
                ptr->what == what &&
                (!data || ptr->data == data))
            {
                if (prev) {
                    prev->next = ptr->next;
                } else {
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

    void MessageQueue::remove(Cycler* c, utl::Executable* exec, void* data) {
        if (!c) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c &&
                ptr->callback == exec &&
                (!data || ptr->data == data))
            {
                if (prev) {
                    prev->next = ptr->next;
                } else {
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
        auto ptr = message_;
        while (ptr) {
            auto msg = ptr;
            ptr = ptr->next;
            msg->recycle();
        }

        message_ = nullptr;
    }

    bool MessageQueue::contains(Cycler* c, int what, void* data) {
        if (!c) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        while (ptr) {
            if (ptr->target == c &&
                ptr->what == what &&
                (!data || ptr->data == data))
            {
                return true;
            }
            ptr = ptr->next;
        }

        return false;
    }

    bool MessageQueue::contains(Cycler* c, utl::Executable* exec, void* data) {
        if (!c) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        while (ptr) {
            if (ptr->target == c &&
                ptr->callback == exec &&
                (data == nullptr || ptr->data == data))
            {
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

        auto barrier = Message::obtain();
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
            if (!ptr->target) {
                if (prev) {
                    prev->next = ptr->next;
                } else {
                    message_ = ptr->next;
                }
                ptr->recycle();
                break;
            }

            prev = ptr;
            ptr = ptr->next;
        }

        has_barrier_ = false;
    }

}