#ifndef CYRONENO_EQUATIONS_H_
#define CYRONENO_EQUATIONS_H_

#include "point.hpp"
#include "vector.hpp"


namespace cyro {

    class Line2DEqu {
    public:
        Line2DEqu(const Point2& p0, const Point2& p1);

        double cal(double x, double y) const;

        double a_, b_, c_;
        double m_;
    };

    class QuadraticEqu {
    public:
        QuadraticEqu(double a, double b, double c);
        QuadraticEqu(const QuadraticEqu& rhs);

        int getRootCount() const;

        double a_, b_, c_;
        double drt_;
        double root1_, root2_;
    };

    class PlaneEqu {
    public:
        PlaneEqu(const Vector3& n, const Point3& q);

        double cal(const Point3& p) const;

        double d_;
        Vector3 n_;
    };

    class PlaneEqu4D {
    public:
        PlaneEqu4D(const Vector4& n, const Point4& q);

        double cal(const Point4& p) const;

        double d_;
        Vector4 n_;
    };

}

#endif  // CYRONENO_EQUATIONS_H_