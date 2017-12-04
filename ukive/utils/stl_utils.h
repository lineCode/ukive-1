#ifndef UKIVE_UTILS_STL_UTILS_H_
#define UKIVE_UTILS_STL_UTILS_H_


namespace ukive {

    template <class T>
    void STLDeleteElements(T *container) {
        for (auto it = container->begin();
            it != container->end(); ++it) {
            delete *it;
        }
        container->clear();
    }

}

#endif  // UKIVE_UTILS_STL_UTILS_H_
