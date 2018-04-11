#ifndef UKIVE_MESSAGE_MESSAGE_QUEUE_H_
#define UKIVE_MESSAGE_MESSAGE_QUEUE_H_

#include <mutex>


namespace ukive {

    class Cycler;
    class Message;
    class Executable;

    class MessageQueue {
    public:
        MessageQueue();
        ~MessageQueue();

        void quit();

        bool enqueue(Message* msg);
        Message* dequeue();
        void remove(Cycler* c, void* data);
        void remove(Cycler* c, int what, void* data);
        void remove(Cycler* c, Executable* exec, void* data);
        bool contains(Cycler* c, int what, void* data);
        bool contains(Cycler* c, Executable* exec, void* data);

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