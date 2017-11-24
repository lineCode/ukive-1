#ifndef UKIVE_UTILS_THREAD_LOCAL_H_
#define UKIVE_UTILS_THREAD_LOCAL_H_

#include <Windows.h>

#include <map>


namespace ukive {

    template<class T>
    class ThreadLocal {
    private:
        static std::map<DWORD, T> sValueList;

    public:
        ThreadLocal() {
        }

        ~ThreadLocal() {
        }

        void set(T value) {
            sValueList.insert(
                std::pair<DWORD, T>(
                    ::GetCurrentThreadId(), value));
        }

        bool get(T &value) {
            auto it = sValueList.find(::GetCurrentThreadId());
            if (it == sValueList.end()) {
                return false;
            }
            else {
                value = it->second;
            }

            return true;
        }

        void remove() {
            sValueList.erase(::GetCurrentThreadId());
        }
    };

    template<class T>
    std::map<DWORD, T> ThreadLocal<T>::sValueList;

}

#endif  // UKIVE_UTILS_THREAD_LOCAL_H_