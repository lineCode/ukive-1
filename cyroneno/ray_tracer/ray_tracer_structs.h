#ifndef CYRONENO_RAY_TRACER_RAY_TRACER_STRUCTS_H_
#define CYRONENO_RAY_TRACER_RAY_TRACER_STRUCTS_H_

#include "../color.h"
#include "../point.hpp"
#include "../vector.hpp"


namespace cyro {

    enum ProjectionType {
        ORTHO,
        PERSP,
    };

    struct Ray {
        Point3 origin;
        Vector3 direction;
    };

    struct HitRecord {
        int index;

        double t;
        Point3 p;
        Vector3 n;
    };

    struct Light {
        Point3 pos;
        ColorRGB intensity;
    };

}

#endif  // CYRONENO_RAY_TRACER_RAY_TRACER_STRUCTS_H_