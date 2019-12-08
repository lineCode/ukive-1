#ifndef UTILS_WEAK_REF_NEST_HPP_
#define UTILS_WEAK_REF_NEST_HPP_

#include <memory>


namespace utl {

    // WeakRef
    template <typename T>
    class WeakRef {
    public:
        struct Flag {
            bool available;

            Flag()
                : available(true) {}
        };

        WeakRef(T* ptr, const std::shared_ptr<Flag>& flag)
            : ptr_(ptr),
              flag_(flag) {
        }

        bool isAvailable() const {
            return flag_ && flag_->available;
        }

        T* getPtr() const {
            return ptr_;
        }

    private:
        T* ptr_;
        std::shared_ptr<Flag> flag_;
    };


    // WeakRefNest
    template <typename T>
    class WeakRefNest {
    public:
        explicit WeakRefNest(T* ptr)
            : ptr_(ptr) {}

        ~WeakRefNest() {
            revoke();
        }

        WeakRef<T> getRef() {
            if (!flag_) {
                flag_ = std::make_shared<typename WeakRef<T>::Flag>();
            }
            return WeakRef<T>(ptr_, flag_);
        }

        void revoke() {
            if (flag_) {
                flag_->available = false;
                flag_.reset();
            }
        }

    private:
        T* ptr_;
        std::shared_ptr<typename WeakRef<T>::Flag> flag_;
    };

}

#endif  // UTILS_WEAK_REF_NEST_HPP_