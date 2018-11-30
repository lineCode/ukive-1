#include "ukive/graphics/point.h"


namespace ukive {

    Point::Point()
        :x(0), y(0) {}

    Point::Point(int x, int y)
        : x(x), y(y) {}

    Point::Point(const Point& rhs)
        : x(rhs.x), y(rhs.y) {}

    Point& Point::operator=(const Point& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    bool Point::operator==(const Point& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }

    bool Point::operator!=(const Point& rhs) const {
        return (x != rhs.x || y != rhs.y);
    }

    PointF Point::operator+(const Vector2& vec) const {
        return PointF(x + vec.x, y + vec.y);
    }

    Vector2 Point::operator-(const Point& rhs) const {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    void Point::set(int x, int y) {
        this->x = x;
        this->y = y;
    }

    PointF Point::toPointF() const {
        return PointF(x, y);
    }


    PointF::PointF()
        :x(0), y(0) {}

    PointF::PointF(float x, float y)
        : x(x), y(y) {}

    PointF::PointF(const PointF& rhs)
        : x(rhs.x), y(rhs.y) {}

    PointF& PointF::operator=(const PointF& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    bool PointF::operator==(const PointF& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }

    bool PointF::operator!=(const PointF& rhs) const {
        return (x != rhs.x || y != rhs.y);
    }

    PointF PointF::operator+(const Vector2& vec) const {
        return PointF(x + vec.x, y + vec.y);
    }

    Vector2 PointF::operator-(const PointF& rhs) const {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    void PointF::set(float x, float y) {
        this->x = x;
        this->y = y;
    }

}