#ifndef UKIVE_UTILS_COM_PTR_H_
#define UKIVE_UTILS_COM_PTR_H_

namespace ukive {

    template <class T>
    class ComPtr {
    public:
        ComPtr()
            :ptr_(nullptr) {}

        explicit ComPtr(T* real)
            :ptr_(real) {}

        ComPtr(const ComPtr& rhs) {
            if (rhs.ptr_) {
                rhs.ptr_->AddRef();
            }
            ptr_ = rhs.ptr_;
        }

        ~ComPtr() {
            reset();
        }


        ComPtr& operator =(const ComPtr& rhs) {
            //防止自身给自身赋值。
            if (this == &rhs) {
                return *this;
            }

            if (rhs.ptr_) {
                rhs.ptr_->AddRef();
            }

            if (ptr_) {
                ptr_->Release();
            }

            ptr_ = rhs.ptr_;
            return *this;
        }

        ComPtr& operator =(T* real) {
            if (real == ptr_) {
                return *this;
            }

            if (ptr_) {
                ptr_->Release();
            }

            ptr_ = real;
            return *this;
        }

        T* operator ->() const {
            return ptr_;
        }

        T** operator &() {
            return &ptr_;
        }

        bool operator ==(nullptr_t) const {
            return ptr_ == nullptr;
        }

        bool operator ==(const ComPtr<T>& rhs) const {
            return ptr_ == rhs.ptr_;
        }

        bool operator !=(nullptr_t) const {
            return ptr_ != nullptr;
        }

        bool operator !=(const ComPtr<T>& rhs) const {
            return ptr_ != rhs.ptr_;
        }

        explicit operator bool() const {
            return ptr_ != nullptr;
        }

        T* get() const {
            return ptr_;
        }

        template<class Ct>
        ComPtr<Ct> cast() {
            Ct* casted = nullptr;
            ptr_->template QueryInterface<Ct>(&casted);
            return ComPtr<Ct>(casted);
        }

        void reset() {
            if (ptr_) {
                ptr_->Release();
                ptr_ = nullptr;
            }
        }

    private:
        T* ptr_;
    };

}

#endif  // UKIVE_UTILS_COM_PTR_H_