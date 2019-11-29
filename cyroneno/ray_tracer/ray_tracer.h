#ifndef CYRONENO_RAY_TRACER_RAY_TRACER_H_
#define CYRONENO_RAY_TRACER_RAY_TRACER_H_

#include "../color.h"
#include "../image.h"
#include "ray_tracer_structs.h"
#include "scene.h"


namespace cyro {

    class RayTracer {
    public:
        RayTracer();

        void rayTracer(
            ProjectionType type, int img_width, int img_height, ImagePng* image);

    private:
        void initLights();
        void initSurfaces();
        ColorBGRInt rayColor(const cyro::Ray& ray, double t0, double t1);

        Scene scene_;
        std::vector<Light> lights_;
    };

}

#endif  // CYRONENO_RAY_TRACER_RAY_TRACER_H_