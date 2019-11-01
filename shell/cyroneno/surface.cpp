#include "surface.h"

#include <algorithm>

#include "equations.h"


namespace cyro {

    //////////
    Sphere::Sphere(const Point3& c, double r)
        : center_(c), radius_(r) {
    }

    bool Sphere::hit(const Ray& ray, double t0, double t1, HitRecord& rec) const {
        QuadraticEqu quad(
            ray.direction*ray.direction,
            ray.direction*(ray.origin - center_) * 2,
            (ray.origin - center_)*(ray.origin - center_) - radius_ * radius_);
        if (quad.getRootCount() > 0) {
            double t = std::min(quad.root1_, quad.root2_);
            if (t < t0 || t > t1) {
                return false;
            }

            auto p = ray.origin + ray.direction*t;
            auto n = (p - center_) / radius_;

            rec.t = t;
            rec.n = n;
            rec.p = p;
            return true;
        }

        return false;
    }

    Box Sphere::boundingBox() const {
        auto min_pos = center_ - Vector3(radius_, radius_, radius_);
        auto max_pos = center_ + Vector3(radius_, radius_, radius_);
        return Box(min_pos, max_pos);
    }


    //////////
    // TODO: 不再假定平面接近平行于 xz 平面
    Plane::Plane(const std::vector<Point3>& p, const Vector3& n)
        : n_(n), points_(p) {
    }

    bool Plane::hit(const Ray& ray, double t0, double t1, HitRecord& rec) const {
        if (points_.size() <= 2) {
            return false;
        }

        auto t = (points_.front() - ray.origin)*n_ / (ray.direction*n_);
        if (t < t0 || t > t1) {
            return false;
        }

        auto p = ray.origin + ray.direction*t;

        int hit_count = 0;
        for (auto it = points_.begin(); it != points_.end(); ++it) {
            auto v1 = *it;
            auto v2 = (it + 1 == points_.end()) ? *points_.begin() : *(it + 1);

            if ((v1.x_ < p.x_ && v2.x_ < p.x_) ||
                (v1.z_ > p.z_ && v2.z_ > p.z_) ||
                (v1.z_ < p.z_ && v2.z_ < p.z_))
            {
                continue;
            }

            if (v1.x_ >= p.x_ && v2.x_ >= p.x_) {
                ++hit_count;
                continue;
            }

            auto vx_at_pz = (p.z_ - v1.z_) / (v2.z_ - v1.z_)*(v2.x_ - v1.x_) + v1.x_;
            if (p.x_ < vx_at_pz) {
                ++hit_count;
            }
        }

        if (hit_count % 2 == 1) {
            rec.t = t;
            rec.n = n_;
            rec.p = p;
            return true;
        }

        return false;
    }

    Box Plane::boundingBox() const {
        return Box(Point3(), Point3());
    }

}