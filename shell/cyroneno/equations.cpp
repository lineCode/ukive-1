#include "equations.h"

#include <cmath>


namespace cyro {

    Quadratic::Quadratic(double a, double b, double c)
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

    Quadratic::Quadratic(const Quadratic& rhs)
        : a_(rhs.a_),
          b_(rhs.b_),
          c_(rhs.c_),
          drt_(rhs.drt_),
          root1_(rhs.root1_),
          root2_(rhs.root2_) {
    }

    int Quadratic::getRootCount() const {
        if (drt_ > 0) {
            return 2;
        }

        if (drt_ == 0) {
            return 1;
        }

        return 0;
    }

}