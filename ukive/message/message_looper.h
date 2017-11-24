#ifndef UKIVE_MESSAGE_MESSAGE_LOOPER_H_
#define UKIVE_MESSAGE_MESSAGE_LOOPER_H_

#include <memory>
#include <mutex>

#include "ukive/utils/thread_local.h"


namespace ukive {

    class Cycler;
    class Message;
    class MessageQueue;

    class MessageLooper {
    public:
        ~MessageLooper();

        static void init();
        static void close();

        void quit();
        MessageQueue *getQueue();

        static void prepare();
        static void prepareMainLooper();
        static void loop();
        static MessageQueue *myQueue();
        static MessageLooper *myLooper();
        static MessageLooper *getMainLooper();

        void receiveMessage();

    private:
        MessageLooper();

        MessageQueue *mMsgQueue;

        static std::mutex mLooperSync;
        static MessageLooper *mMainLooper;
        static ThreadLocal<std::shared_ptr<MessageLooper>> *mThreadLocal;
    };

}

#endif  // UKIVE_MESSAGE_MESSAGE_LOOPER_H_