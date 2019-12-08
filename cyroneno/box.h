#ifndef CYRONENO_BOX_H_
#define CYRONENO_BOX_H_

#include "point.hpp"


namespace cyro {

    class Box {
    public:
        Box(const Point3& min, const Point3& max);

        Point3 min_, max_;
    };

}

#endif  // CYRONENO_BOX_H_