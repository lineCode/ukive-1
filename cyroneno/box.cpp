#include "box.h"


namespace cyro {

    Box::Box(const Point3& min, const Point3& max)
        : min_(min), max_(max) {
    }

}