#ifndef UKIVE_UTILS_WEAK_BIND_H_
#define UKIVE_UTILS_WEAK_BIND_H_

#include <functional>
#include <memory>


namespace ukive {

    typedef std::function<void()> Solid;

    template <typename T>
    class SharedHelper : public std::enable_shared_from_this<T> {
    public:
        std::shared_ptr<T> s_this() { return shared_from_this(); }
        std::shared_ptr<const T> s_this() const { return shared_from_this(); }
        std::weak_ptr<T> w_this() { return weak_from_this(); }
        std::weak_ptr<const T> w_this() const { return weak_from_this(); }
    };


    template <typename Ret>
    struct Wrapper {
        template <typename Obj>
        Ret operator()(const std::weak_ptr<Obj> &obj, const Solid &bound_func, Ret &def_ret)
        {
            if (auto ptr = obj.lock()) {
                return bound_func();
            }

            return def_ret;
        }
    };

    template <typename Ret, typename Func, typename Obj, typename... Types>
    std::function<Ret()> weak_bind(Ret &&def_ret, Func &&func, const std::shared_ptr<Obj> &obj, Types&&... args) {
        return std::bind(
            Wrapper<Ret>(),
            std::weak_ptr<Obj>(obj),
            std::function<Ret()>(std::bind(func, obj.get(), std::forward<Types>(args)...)), def_ret);
    }

    // 无返回值的特例
    template <>
    struct Wrapper<void> {
        template <typename Obj>
        void operator()(const std::weak_ptr<Obj> &obj, Solid &bound_func)
        {
            if (auto ptr = obj.lock()) {
                bound_func();
            }
        }
    };

    template <typename Func, typename Obj, typename... Types>
    std::function<void()> weak_bind(Func &&func, const std::shared_ptr<Obj> &obj, Types&&... args) {
        return std::bind(
            Wrapper<void>(),
            std::weak_ptr<Obj>(obj),
            std::function<void()>(std::bind(func, obj.get(), std::forward<Types>(args)...)));
    }
}

#endif  // UKIVE_UTILS_WEAK_BIND_H_