#ifndef UTILS_STL_UTILS_H_
#define UTILS_STL_UTILS_H_

#include <limits>

#include "utils/log.h"


namespace utl {

    template <class T>
    void STLDeleteElements(T* container) {
        for (auto it = container->begin(); it != container->end(); ++it) {
            delete *it;
        }
        container->clear();
    }

    template <typename R>
    auto STLCST(const R&, int count) -> typename R::size_type {
        DCHECK(count >= 0);
        return static_cast<typename R::size_type>(count);
    }

    template <typename R>
    int STLCInt(R count) {
        DCHECK(static_cast<R>((std::numeric_limits<int>::max)()) >= count);
        return static_cast<int>(count);
    }

    template <typename R>
    uint32_t STLCU32(R count) {
        DCHECK(static_cast<R>((std::numeric_limits<uint32_t>::max)()) >= count);
        return static_cast<uint32_t>(count);
    }

}

#endif  // UTILS_STL_UTILS_H_
