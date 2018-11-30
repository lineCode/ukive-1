#include "scene.h"

#include "surface.h"


namespace cyro {

    bool Scene::hit(const Ray& ray, double t0, double t1, HitRecord& rec) const {
        int index = 0;
        HitRecord hit{};
        std::shared_ptr<Surface> hit_surface;

        for (int i = 0; i < static_cast<int>(surfaces_.size()); ++i) {
            HitRecord cur_hit;
            if (surfaces_[i]->hit(ray, t0, t1, cur_hit)) {
                if (hit_surface && cur_hit.t < hit.t) {
                    hit = cur_hit;
                    hit_surface = surfaces_[i];
                    index = i;
                } else if (!hit_surface) {
                    hit = cur_hit;
                    hit_surface = surfaces_[i];
                    index = i;
                }
            }
        }

        if (hit_surface) {
            rec.index = index;
            rec.t = hit.t;
            rec.p = hit.p;
            rec.n = hit.n;

            return true;
        }

        return false;
    }

    void Scene::addSurface(std::shared_ptr<cyro::Surface> s) {
        surfaces_.push_back(s);
    }

    void Scene::removeSurface(size_t index) {
        surfaces_.erase(surfaces_.begin() + index);
    }

    std::shared_ptr<cyro::Surface> Scene::getSurface(size_t index) const {
        return surfaces_[index];
    }

}