#ifndef CYRONENO_POINT_H_
#define CYRONENO_POINT_H_


namespace cyro {

    class Vector2;
    class Vector3;
    class Vector4;

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
        Point3(const Point2& p, double z);
        Point3(const Point3& rhs);

        Point3& operator=(const Point3& rhs);
        Point3 operator+(const Vector3& rhs) const;
        Vector3 operator-(const Point3& rhs) const;
        Point3 operator-(const Vector3& rhs) const;
        Vector3 toVector() const;

        void add(const Vector3& rhs);
        void sub(const Vector3& rhs);

        double x_, y_, z_;
    };

    class Point4 {
    public:
        Point4();
        Point4(double x, double y, double z, double w);
        Point4(const Point3& p, double w);
        Point4(const Point4& rhs);

        Point4& operator=(const Point4& rhs);
        Point4 operator+(const Vector4& rhs) const;
        Vector4 operator-(const Point4& rhs) const;
        Point4 operator-(const Vector4& rhs) const;
        Vector4 toVector() const;

        void add(const Vector4& rhs);
        void sub(const Vector4& rhs);

        Point3 toPoint3() const;

        double x_, y_, z_, w_;
    };

}

#endif  // CYRONENO_POINT_H_