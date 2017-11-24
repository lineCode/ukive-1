#include "message.h"


namespace ukive {

    Message *Message::pool = 0;
    uint64_t Message::pool_size = 0;
    bool Message::is_initialized = false;
    std::mutex Message::pool_sync;


    Message::Message()
        :what(-1),
        when(0),
        target(0),
        callback(nullptr),
        data(nullptr),
        next(nullptr) {
    }

    Message::~Message() {
    }


    void Message::init(uint64_t pool_capacity) {
        if (is_initialized)
            throw std::logic_error("Message-init(): can only init once.");

        Message *ptr = nullptr;

        for (uint64_t i = 0; i < pool_capacity; ++i) {
            Message *msg = new Message();
            if (pool) {
                ptr->next = msg;
                ptr = msg;
            }
            else {
                pool = msg;
                ptr = pool;
            }
        }

        is_initialized = true;
        pool_size = pool_capacity;
    }

    void Message::close() {
        while (pool) {
            Message *msg = pool;
            pool = msg->next;
            delete msg;
        }

        is_initialized = false;
        pool_size = 0;
    }


    Message *Message::obtain() {
        std::lock_guard<std::mutex> lk(pool_sync);

        if (pool) {
            Message *msg = pool;
            pool = msg->next;
            msg->next = 0;
            --pool_size;

            return msg;
        }

        return new Message();
    }

    void Message::recycle() {
        std::lock_guard<std::mutex> lk(pool_sync);

        this->next = pool;
        pool = this;
        ++pool_size;

        what = -1;
        when = 0;
        target = 0;
        callback = 0;
        data = 0;
    }

}