#ifndef UKIVE_GRAPHICS_POINT_H_
#define UKIVE_GRAPHICS_POINT_H_

#include "ukive/graphics/vector2.h"


namespace ukive {

    class PointF;

    class Point {
    public:
        Point();
        Point(int x, int y);
        Point(const Point& rhs);

        Point& operator=(const Point& rhs);
        bool operator==(const Point& rhs) const;
        bool operator!=(const Point& rhs) const;

        PointF operator+(const Vector2& vec) const;
        Vector2 operator-(const Point& rhs) const;

        void set(int x, int y);
        PointF toPointF() const;

        int x;
        int y;
    };

    class PointF {
    public:
        PointF();
        PointF(float x, float y);
        PointF(const PointF& rhs);

        PointF& operator=(const PointF& rhs);
        bool operator==(const PointF& rhs) const;
        bool operator!=(const PointF& rhs) const;

        PointF operator+(const Vector2& vec) const;
        Vector2 operator-(const PointF& rhs) const;

        void set(float x, float y);

        float x;
        float y;
    };

}

#endif  // UKIVE_GRAPHICS_POINT_H_