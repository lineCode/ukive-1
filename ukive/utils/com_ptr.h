#ifndef UKIVE_UTILS_COM_PTR_H_
#define UKIVE_UTILS_COM_PTR_H_

namespace ukive {

    template <class T>
    class ComPtr
    {
    private:
        T *mPtr;

    public:
        ComPtr()
        {
            mPtr = nullptr;
        }

        ComPtr(T *real)
        {
            mPtr = real;
        }

        ComPtr(const ComPtr& rhs)
        {
            if (rhs.mPtr)
                rhs.mPtr->AddRef();
            mPtr = rhs.mPtr;
        }

        ~ComPtr()
        {
            reset();
        }


        ComPtr& operator =(const ComPtr& rhs)
        {
            //防止自身给自身赋值。
            if (this == (ComPtr*)(&(int&)rhs))
                return *this;

            if (rhs.mPtr)
                rhs.mPtr->AddRef();

            if (mPtr)
                mPtr->Release();
            mPtr = rhs.mPtr;
            return *this;
        }

        ComPtr& operator =(T *real)
        {
            if (real == mPtr)
                return *this;

            if (mPtr)
                mPtr->Release();
            mPtr = real;
            return *this;
        }

        T* operator ->()
        {
            return mPtr;
        }

        T** operator &()
        {
            return (&mPtr);
        }

        bool operator ==(std::nullptr_t) const
        {
            return (mPtr == nullptr);
        }

        bool operator !=(std::nullptr_t) const
        {
            return (mPtr != nullptr);
        }

        explicit operator bool() const
        {
            return (mPtr != nullptr);
        }

        T* get() const
        {
            return mPtr;
        }

        template<class Ct>
        ComPtr<Ct> cast()
        {
            Ct *casted = nullptr;
            mPtr->QueryInterface<Ct>(&casted);
            return ComPtr<Ct>(casted);
        }

        void reset()
        {
            if (mPtr)
            {
                mPtr->Release();
                mPtr = nullptr;
            }
        }
    };

}

#endif  // UKIVE_UTILS_COM_PTR_H_