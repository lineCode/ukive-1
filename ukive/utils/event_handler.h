#ifndef UKIVE_UTILS_EVENT_HANDLER_H_
#define UKIVE_UTILS_EVENT_HANDLER_H_

#include <functional>
#include <vector>


/* Example:
 *
class Executor {
public:
    using TestHandler = EventHandler<void(int)>;
    using TestWrapper = EventWrapper<void(int)>;

    void run(int param) {
        events_(param);
    }

    TestHandler& events() {
        return events_;
    }

private:
    TestHandler events_;
};

class Person {
public:
    explicit Person(Executor *exec)
        :exec_(exec) {

        using namespace std::placeholders;

        handler1_ = std::bind(&Person::onRun, this, _1);

        exec_->events() += handler1_;
        exec_->events() += [](int p) {
            std::cout << "Lambda: " << p << std::endl;
        };
    }

    ~Person() {
        exec_->events() -= handler1_;
        //exec_->events() -= handler2_;
    }

    void onRun(int param) {
        std::cout << "onRun: " << param << std::endl;
    }

private:
    Executor* exec_;
    Executor::TestWrapper handler1_;
    Executor::TestWrapper handler2_;
};

int main(int argc, char *argv[]) {
    Executor* exec = new Executor();
    {
        Person p1(exec), p2(exec);
        exec->run(233);
    }
    delete exec;
    return 0;
}
*/


namespace ukive {

    template <typename Func>
    class EventWrapper {
    public:
        EventWrapper() = default;
        explicit EventWrapper(const std::function<Func>& func)
            : id_(0),
            func_(func) {}

        EventWrapper& operator=(const std::function<Func>& func) {
            func_ = func;
            return *this;
        }

        template <typename... Args>
        void operator()(Args&&... args) {
            func_(std::forward<Args>(args)...);
        }

        void setId(intptr_t id) {
            id_ = id;
        }

        intptr_t getId() const {
            return id_;
        }

    private:
        intptr_t id_;
        std::function<Func> func_;
    };

    template <typename Func>
    class EventHandler {
    public:
        template <typename... Args>
        void operator()(Args&&... args) {
            for (auto& ev : evs_) {
                ev(std::forward<Args>(args)...);
            }
        }

        void operator+=(EventWrapper<Func>& ev) {
            ev.setId(reinterpret_cast<intptr_t>(&ev));
            evs_.push_back(ev);
        }

        void operator+=(std::function<Func>&& f) {
            auto wrapper = EventWrapper<Func>(f);
            evs_.push_back(std::move(wrapper));
        }

        void operator-=(EventWrapper<Func>& ev) {
            if (ev.getId() == 0) {
                return;
            }

            bool hit = false;
            for (auto it = evs_.begin(); it != evs_.end();) {
                if (it->getId() != 0 && it->getId() == ev.getId()) {
                    hit = true;
                    it = evs_.erase(it);
                } else {
                    ++it;
                }
            }

            if (hit) {
                ev.setId(0);
            }
        }

    private:
        std::vector<EventWrapper<Func>> evs_;
    };

}


#endif  // UKIVE_UTILS_EVENT_HANDLER_H_