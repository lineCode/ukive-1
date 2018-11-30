#include "point.h"

#include "vector.h"


namespace cyro {

    //////////
    Point2::Point2()
        : x_(0), y_(0) {
    }

    Point2::Point2(double x, double y)
        : x_(x), y_(y) {
    }

    Point2::Point2(const Point2& rhs)
        : x_(rhs.x_), y_(rhs.y_) {
    }

    Point2& Point2::operator=(const Point2& rhs) {
        x_ = rhs.x_;
        y_ = rhs.y_;
        return *this;
    }

    Point2 Point2::operator+(const Vector2& rhs) const {
        Point2 p(*this);
        p.add(rhs);
        return p;
    }

    Vector2 Point2::operator-(const Point2& rhs) const {
        Vector2 v(x_ - rhs.x_, y_ - rhs.y_);
        return v;
    }

    Point2 Point2::operator-(const Vector2& rhs) const {
        Point2 p(*this);
        p.sub(rhs);
        return p;
    }

    void Point2::add(const Vector2& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
    }

    void Point2::sub(const Vector2& rhs) {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
    }


    //////////
    Point3::Point3()
        : x_(0), y_(0), z_(0) {}

    Point3::Point3(double x, double y, double z)
        : x_(x), y_(y), z_(z) {}

    Point3::Point3(const Point3& rhs)
        : x_(rhs.x_), y_(rhs.y_), z_(rhs.z_) {}

    Point3& Point3::operator=(const Point3& rhs) {
        x_ = rhs.x_;
        y_ = rhs.y_;
        z_ = rhs.z_;
        return *this;
    }

    Point3 Point3::operator+(const Vector3& rhs) const {
        Point3 p(*this);
        p.add(rhs);
        return p;
    }

    Vector3 Point3::operator-(const Point3& rhs) const {
        Vector3 v(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
        return v;
    }

    Point3 Point3::operator-(const Vector3& rhs) const {
        Point3 p(*this);
        p.sub(rhs);
        return p;
    }

    void Point3::add(const Vector3& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
    }

    void Point3::sub(const Vector3& rhs) {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        z_ -= rhs.z_;
    }
}