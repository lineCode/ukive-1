#ifndef CYRONENO_SCENE_H_
#define CYRONENO_SCENE_H_

#include <memory>
#include <vector>

#include "ray_tracer_structs.h"


namespace cyro {

    class Surface;

    class Scene {
    public:
        Scene() = default;

        bool hit(const Ray& ray, double t0, double t1, HitRecord& rec) const;

        void addSurface(std::shared_ptr<cyro::Surface> s);
        void removeSurface(size_t index);
        std::shared_ptr<cyro::Surface> getSurface(size_t index) const;

    private:
        std::vector<std::shared_ptr<cyro::Surface>> surfaces_;
    };

}

#endif  // CYRONENO_SCENE_H_