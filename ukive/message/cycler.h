#ifndef UKIVE_MESSAGE_CYCLER_H_
#define UKIVE_MESSAGE_CYCLER_H_

#include <cstdint>

#include "message_queue.h"


namespace ukive {

    class Executable;
    class Message;
    class MessageLooper;

    class Cycler
    {
    public:
        Cycler();
        Cycler(MessageLooper *looper);
        virtual ~Cycler();

        void post(Executable *exec);
        void postDelayed(Executable *exec, uint64_t millis);
        void postAtTime(Executable *exec, uint64_t atTimeMillis);

        bool hasCallbacks(Executable *exec);
        void removeCallbacks(Executable *exec);

        void sendMessage(Message *msg);
        void sendMessageDelayed(Message *msg, uint64_t millis);
        void sendMessageAtTime(Message *msg, uint64_t atTimeMillis);
        void sendEmptyMessage(int what);
        void sendEmptyMessageDelayed(int what, uint64_t millis);
        void sendEmptyMessageAtTime(int what, uint64_t atTimeMillis);

        void enqueueMessage(Message *msg);

        bool hasMessages(int what);
        bool hasMessages(int what, void *data);
        void removeMessages(int what);
        void removeMessages(int what, void *data);

        void dispatchMessage(Message *msg);
        virtual void handleMessage(Message *msg);

    private:
        MessageLooper *looper_;
    };

}

#endif  // UKIVE_MESSAGE_CYCLER_H_