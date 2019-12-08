#include "equations.h"

#include <cmath>


namespace cyro {

    // Line2DEqu
    Line2DEqu::Line2DEqu(const Point2& p0, const Point2& p1)
        : a_(p0.y - p1.y),
          b_(p1.x - p0.x),
          c_(p0.x * p1.y - p1.x * p0.y),
          m_((p1.y - p0.y) / (p1.x - p0.x)) {
    }

    double Line2DEqu::cal(double x, double y) const {
        return a_ * x + b_ * y + c_;
    }


    // QuadraticEqu
    QuadraticEqu::QuadraticEqu(double a, double b, double c)
        : a_(a),
          b_(b),
          c_(c),
          drt_(0),
          root1_(0),
          root2_(0)
    {
        drt_ = b*b - 4 * a*c;
        if (drt_ == 0) {
            root1_ = root2_ = -b_ / (2 * a);
        } else if (drt_ > 0) {
            root1_ = (-b_ + std::sqrt(drt_)) / (2 * a);
            root2_ = (-b_ - std::sqrt(drt_)) / (2 * a);
        }
    }

    QuadraticEqu::QuadraticEqu(const QuadraticEqu& rhs)
        : a_(rhs.a_),
          b_(rhs.b_),
          c_(rhs.c_),
          drt_(rhs.drt_),
          root1_(rhs.root1_),
          root2_(rhs.root2_) {
    }

    int QuadraticEqu::getRootCount() const {
        if (drt_ > 0) {
            return 2;
        }

        if (drt_ == 0) {
            return 1;
        }

        return 0;
    }


    // PlaneEqu
    PlaneEqu::PlaneEqu(const Vector3& n, const Point3& q)
        : d_(-(n * q.toVector())), n_(n) {}

    double PlaneEqu::cal(const Point3& p) const {
        return n_ * p.toVector() + d_;
    }

    // PlaneEqu4D
    PlaneEqu4D::PlaneEqu4D(const Vector4& n, const Point4& q)
        : d_(-(n * q.toVector())), n_(n) {}

    double PlaneEqu4D::cal(const Point4& p) const {
        return n_ * p.toVector() + d_;
    }

}
