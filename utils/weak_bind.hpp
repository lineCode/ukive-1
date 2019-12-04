#ifndef UTILS_WEAK_BIND_HPP_
#define UTILS_WEAK_BIND_HPP_

#include <functional>
#include <memory>

#include "utils/weak_ref_nest.hpp"


namespace utl {

    using Solid = std::function<void()>;

    /**
     * WeakPtr
     * 对象指针必须是 std::shared_ptr
     */
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
     * 需要与 WeakRefNest 合用
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

#endif  // UTILS_WEAK_BIND_HPP_