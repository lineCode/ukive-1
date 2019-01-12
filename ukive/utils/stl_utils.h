#ifndef UKIVE_UTILS_STL_UTILS_H_
#define UKIVE_UTILS_STL_UTILS_H_

#include <limits>

#include "ukive/log.h"


namespace ukive {

    template <class T>
    void STLDeleteElements(T* container) {
        for (auto it = container->begin(); it != container->end(); ++it) {
            delete *it;
        }
        container->clear();
    }

    template <typename R>
    auto STLCST(const R& obj, int count) -> typename R::size_type {
        DCHECK(count >= 0);
        return static_cast<typename R::size_type>(count);
    }

    template <typename R>
    int STLCInt(R count) {
        DCHECK(static_cast<R>((std::numeric_limits<int>::max)()) >= count);
        return static_cast<int>(count);
    }

}

#endif  // UKIVE_UTILS_STL_UTILS_H_
