#ifndef UKIVE_MESSAGE_CYCLER_H_
#define UKIVE_MESSAGE_CYCLER_H_

#include <cstdint>
#include <functional>

#include "message_queue.h"


namespace ukive {

    class Executable;
    class Message;
    class MessageLooper;


    class CyclerListener {
    public:
        virtual ~CyclerListener() = default;

        virtual void onHandleMessage(Message* msg) = 0;
    };


    class Cycler {
    public:
        enum TimeUnit {
            Millis,
            MillisPrecise,
            Micros,
        };

        explicit Cycler(TimeUnit unit = Millis);
        explicit Cycler(MessageLooper* looper, TimeUnit unit = Millis);
        virtual ~Cycler();

        void setListener(CyclerListener* l);

        void post(Executable* exec);
        void postDelayed(Executable* exec, uint64_t delay);
        void postAtTime(Executable* exec, uint64_t at_time);

        void post(const std::function<void()>& func, int what = -1);
        void postDelayed(const std::function<void()>& func, uint64_t delay, int what = -1);
        void postAtTime(const std::function<void()>& func, uint64_t at_time, int what = -1);

        bool hasCallbacks(Executable* exec);
        void removeCallbacks(Executable* exec);

        void sendMessage(Message* msg);
        void sendMessageDelayed(Message* msg, uint64_t delay);
        void sendMessageAtTime(Message* msg, uint64_t at_time);
        void sendEmptyMessage(int what);
        void sendEmptyMessageDelayed(int what, uint64_t delay);
        void sendEmptyMessageAtTime(int what, uint64_t at_time);

        void enqueueMessage(Message* msg);

        bool hasMessages(int what);
        bool hasMessages(int what, void* data);
        void removeMessages(int what);
        void removeMessages(int what, void* data);

        void dispatchMessage(Message* msg);

        uint64_t now() const;

    private:
        TimeUnit time_unit_;
        MessageLooper* looper_;
        CyclerListener* listener_;
    };

}

#endif  // UKIVE_MESSAGE_CYCLER_H_