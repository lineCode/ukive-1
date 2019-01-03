#ifndef CYRONENO_VECTOR_H_
#define CYRONENO_VECTOR_H_


namespace cyro {

    class Vector2 {
    public:
        Vector2();
        Vector2(double x, double y);
        Vector2(const Vector2& rhs);

        Vector2& operator=(const Vector2& rhs);

        Vector2 operator+(const Vector2& rhs) const;
        Vector2 operator-() const;
        Vector2 operator-(const Vector2& rhs) const;
        Vector2 operator*(double factor) const;
        Vector2 operator/(double factor) const;
        double operator*(const Vector2& rhs) const;
        Vector2 normalize() const;

        void add(const Vector2& rhs);
        void sub(const Vector2& rhs);
        void mul(double factor);
        void div(double factor);
        void inv();
        void nor();

        double length() const;

        double x_, y_;
    };


    class Vector3 {
    public:
        Vector3();
        Vector3(double x, double y, double z);
        Vector3(const Vector3& rhs);

        Vector3& operator=(const Vector3& rhs);

        Vector3 operator+(const Vector3& rhs) const;
        Vector3 operator-() const;
        Vector3 operator-(const Vector3& rhs) const;
        Vector3 operator*(double factor) const;
        Vector3 operator/(double factor) const;
        double operator*(const Vector3& rhs) const;
        Vector3 operator^(const Vector3& rhs) const;
        Vector3 normalize() const;

        void add(const Vector3& rhs);
        void sub(const Vector3& rhs);
        void mul(double factor);
        void div(double factor);
        void cross(const Vector3& rhs);
        void inv();
        void nor();

        double length() const;

        double x_, y_, z_;
    };


    class Vector4 {
    public:
        Vector4();
        Vector4(double x, double y, double z, double w);
        Vector4(const Vector4& rhs);

        Vector4& operator=(const Vector4& rhs);

        Vector4 operator+(const Vector4& rhs) const;
        Vector4 operator-() const;
        Vector4 operator-(const Vector4& rhs) const;
        Vector4 operator*(double factor) const;
        Vector4 operator/(double factor) const;
        double operator*(const Vector4& rhs) const;
        Vector4 normalize() const;

        void add(const Vector4& rhs);
        void sub(const Vector4& rhs);
        void mul(double factor);
        void div(double factor);
        void inv();
        void nor();

        double length() const;

        double x_, y_, z_, w_;
    };

}

#endif  // CYRONENO_VECTOR_H_