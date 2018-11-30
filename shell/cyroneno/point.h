#ifndef CYRONENO_POINT_H_
#define CYRONENO_POINT_H_


namespace cyro {

    class Vector2;
    class Vector3;

    class Point2 {
    public:
        Point2();
        Point2(double x, double y);
        Point2(const Point2& rhs);

        Point2& operator=(const Point2& rhs);
        Point2 operator+(const Vector2& rhs) const;
        Vector2 operator-(const Point2& rhs) const;
        Point2 operator-(const Vector2& rhs) const;

        void add(const Vector2& rhs);
        void sub(const Vector2& rhs);

        double x_, y_;
    };

    class Point3 {
    public:
        Point3();
        Point3(double x, double y, double z);
        Point3(const Point3& rhs);

        Point3& operator=(const Point3& rhs);
        Point3 operator+(const Vector3& rhs) const;
        Vector3 operator-(const Point3& rhs) const;
        Point3 operator-(const Vector3& rhs) const;

        void add(const Vector3& rhs);
        void sub(const Vector3& rhs);

        double x_, y_, z_;
    };

}

#endif  // CYRONENO_POINT_H_