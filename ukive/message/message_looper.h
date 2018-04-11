#ifndef UKIVE_MESSAGE_MESSAGE_LOOPER_H_
#define UKIVE_MESSAGE_MESSAGE_LOOPER_H_

#include <memory>
#include <mutex>


namespace ukive {

    class Cycler;
    class Message;
    class MessageQueue;

    class MessageLooper {
    public:
        ~MessageLooper();

        void quit();
        MessageQueue* getQueue();

        static void prepare();
        static void prepareMainLooper();
        static void loop();
        static MessageQueue* myQueue();
        static MessageLooper* myLooper();
        static MessageLooper* getMainLooper();

        void receiveMessage();

    private:
        MessageLooper();

        MessageQueue* mMsgQueue;

        static std::mutex mLooperSync;
        static MessageLooper* mMainLooper;
        static thread_local std::shared_ptr<MessageLooper> looper_;
    };

}

#endif  // UKIVE_MESSAGE_MESSAGE_LOOPER_H_