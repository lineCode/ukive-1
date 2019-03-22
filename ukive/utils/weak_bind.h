#ifndef UKIVE_UTILS_WEAK_BIND_H_
#define UKIVE_UTILS_WEAK_BIND_H_

#include <functional>
#include <memory>

#include "ukive/utils/weak_ref_nest.h"


namespace ukive {

    using Solid = std::function<void()>;

    /**
     * WeakPtr
     */
    template <typename T>
    class SharedHelper : public std::enable_shared_from_this<T> {
    public:
        std::shared_ptr<T> s_this() { return shared_from_this(); }
        std::shared_ptr<const T> s_this() const { return shared_from_this(); }
        std::weak_ptr<T> w_this() { return weak_from_this(); }
        std::weak_ptr<const T> w_this() const { return weak_from_this(); }
    };

    template <typename Ret>
    struct WeakPtrWrapper {
        template <typename Obj>
        Ret operator()(const std::weak_ptr<Obj>& obj, const Solid& bound_func, Ret& def_ret) {
            if (auto ptr = obj.lock()) {
                return bound_func();
            }

            return def_ret;
        }
    };

    // 无返回值的特例
    template <>
    struct WeakPtrWrapper<void> {
        template <typename Obj>
        void operator()(const std::weak_ptr<Obj>& obj, Solid& bound_func) {
            if (auto ptr = obj.lock()) {
                bound_func();
            }
        }
    };

    template <typename Func, typename Obj, typename... Types>
    std::function<void()> weakptr_bind(Func&& func, const std::shared_ptr<Obj>& obj, Types&&... args) {
        return std::bind(
            WeakPtrWrapper<void>(),
            std::weak_ptr<Obj>(obj),
            std::function<void()>(std::bind(func, obj.get(), std::forward<Types>(args)...)));
    }

    template <typename Ret, typename Func, typename Obj, typename... Types>
    std::function<Ret()> weakptr_ret_bind(
        Ret&& def_ret, Func&& func, const std::shared_ptr<Obj>& obj, Types&&... args) {
        return std::bind(
            WeakPtrWrapper<Ret>(),
            std::weak_ptr<Obj>(obj),
            std::function<Ret()>(std::bind(func, obj.get(), std::forward<Types>(args)...)), def_ret);
    }


    /**
     * WeakRef
     */
    template <typename Ret>
    struct WeakRefWrapper {
        template <typename Obj>
        Ret operator()(const WeakRef<Obj>& ref, const Solid& bound_func, Ret& def_ret) {
            if (ref.isAvailable()) {
                return bound_func();
            }

            return def_ret;
        }
    };

    // 无返回值的特例
    template <>
    struct WeakRefWrapper<void> {
        template <typename Obj>
        void operator()(const WeakRef<Obj>& ref, Solid& bound_func) {
            if (ref.isAvailable()) {
                bound_func();
            }
        }
    };

    template <typename Func, typename Obj, typename... Types>
    std::function<void()> weakref_bind(Func&& func, const WeakRef<Obj>& obj, Types&&... args) {
        return std::bind(
            WeakRefWrapper<void>(),
            obj,
            std::function<void()>(std::bind(func, obj.getPtr(), std::forward<Types>(args)...)));
    }

    template <typename Ret, typename Func, typename Obj, typename... Types>
    std::function<Ret()> weakref_ret_bind(
        Ret&& def_ret, Func&& func, const WeakRef<Obj>& obj, Types&&... args) {
        return std::bind(
            WeakRefWrapper<Ret>(),
            obj,
            std::function<Ret()>(std::bind(func, obj.getPtr(), std::forward<Types>(args)...)), def_ret);
    }
}

#endif  // UKIVE_UTILS_WEAK_BIND_H_