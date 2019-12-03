#ifndef UKIVE_MESSAGE_MESSAGE_H_
#define UKIVE_MESSAGE_MESSAGE_H_

#include <mutex>
#include <functional>


namespace utl {
    class Executable;
}

namespace ukive {

    class Cycler;

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
        utl::Executable* callback;
        std::function<void()> func;
        void* data;
        std::shared_ptr<void> shared_data;

        Message* next;

    private:
        Message();
        ~Message();

        static Message* pool;
        static uint64_t pool_size;

        static bool is_initialized;
        static std::mutex pool_sync;
    };

}

#endif  // UKIVE_MESSAGE_MESSAGE_H_