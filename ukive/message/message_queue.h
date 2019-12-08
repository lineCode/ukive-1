#ifndef UKIVE_MESSAGE_MESSAGE_QUEUE_H_
#define UKIVE_MESSAGE_MESSAGE_QUEUE_H_

#include <mutex>


namespace utl {
    class Executable;
}

namespace ukive {

    class Cycler;
    class Message;

    class MessageQueue {
    public:
        MessageQueue();
        ~MessageQueue();

        void quit();
        bool hasMessage();

        bool enqueue(Message* msg);
        Message* dequeue();
        void remove(Cycler* c);
        void remove(Cycler* c, int what);
        void remove(Cycler* c, utl::Executable* exec);
        bool contains(Cycler* c, int what);
        bool contains(Cycler* c, utl::Executable* exec);

        void addBarrier();
        void removeBarrier();

    private:
        void removeAllLocked();

        bool is_quitting_;
        bool has_barrier_;
        Message* message_;
        std::mutex queue_sync_;
    };

}

#endif  // UKIVE_MESSAGE_MESSAGE_QUEUE_H_