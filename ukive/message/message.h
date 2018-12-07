#ifndef UKIVE_MESSAGE_MESSAGE_H_
#define UKIVE_MESSAGE_MESSAGE_H_

#include <mutex>
#include <functional>


namespace ukive {

    class Cycler;
    class Executable;

    class Message {
    public:
        static Message* obtain();
        void recycle();

        static void init(uint64_t pool_capacity);
        static void close();

    public:
        int what;
        uint64_t when;
        Cycler* target;
        Executable* callback;
        std::function<void()> func;
        void* data;

        Message* next;

        static Message* pool;
        static uint64_t pool_size;

    private:
        Message();
        ~Message();

        static bool is_initialized;
        static std::mutex pool_sync;
    };

}

#endif  // UKIVE_MESSAGE_MESSAGE_H_