#ifndef CYRONENO_MATRIX_H_
#define CYRONENO_MATRIX_H_


namespace cyro {

    class Point3;
    class Point4;
    class Vector3;
    class Vector4;

    class Matrix3x3 {
    public:
        Matrix3x3();
        Matrix3x3(
            double m11, double m12, double m13,
            double m21, double m22, double m23,
            double m31, double m32, double m33);
        Matrix3x3(const Matrix3x3& rhs);

        Matrix3x3& operator=(const Matrix3x3& rhs);
        Matrix3x3 operator+(const Matrix3x3& rhs);
        Matrix3x3 operator-(const Matrix3x3& rhs);
        Matrix3x3 operator*(const Matrix3x3& rhs);
        Point3 operator*(const Point3& p);
        Vector3 operator*(const Vector3& v);
        Matrix3x3 operator*(double factor);
        Matrix3x3 operator/(double factor);

        void identity();
        void add(const Matrix3x3& rhs);
        void sub(const Matrix3x3& rhs);
        void mul(const Matrix3x3& rhs);
        void mul(double factor);
        void div(double factor);

        static Matrix3x3 scale(double sx, double sy);
        static Matrix3x3 shearX(double s);
        static Matrix3x3 shearY(double s);
        static Matrix3x3 rotate(double angle);
        static Matrix3x3 reflect(bool rx, bool ry);
        static Matrix3x3 translate(double tx, double ty);

        double m11_, m12_, m13_,
               m21_, m22_, m23_,
               m31_, m32_, m33_;
    };


    class Matrix4x4 {
    public:
        Matrix4x4();
        Matrix4x4(
            double m11, double m12, double m13, double m14,
            double m21, double m22, double m23, double m24,
            double m31, double m32, double m33, double m34,
            double m41, double m42, double m43, double m44);
        Matrix4x4(const Matrix4x4& rhs);

        Matrix4x4& operator=(const Matrix4x4& rhs);
        Matrix4x4 operator+(const Matrix4x4& rhs);
        Matrix4x4 operator-(const Matrix4x4& rhs);
        Matrix4x4 operator*(const Matrix4x4& rhs);
        Point4 operator*(const Point4& p);
        Vector4 operator*(const Vector4& v);
        Matrix4x4 operator*(double factor);
        Matrix4x4 operator/(double factor);

        void identity();
        void add(const Matrix4x4& rhs);
        void sub(const Matrix4x4& rhs);
        void mul(const Matrix4x4& rhs);
        void mul(double factor);
        void div(double factor);

        static Matrix4x4 scale(double sx, double sy, double sz);
        static Matrix4x4 shearX(double sy, double sz);
        static Matrix4x4 shearY(double sx, double sz);
        static Matrix4x4 shearZ(double sx, double sy);
        static Matrix4x4 rotateX(double angle);
        static Matrix4x4 rotateY(double angle);
        static Matrix4x4 rotateZ(double angle);
        static Matrix4x4 translate(double tx, double ty, double tz);

        static Matrix4x4 viewport(int pixel_width, int pixel_height);
        static Matrix4x4 orthoProj(double l, double r, double b, double t, double n, double f);
        static Matrix4x4 persp(double n, double f);
        static Matrix4x4 camera(const Vector3& eye, const Vector3& look, const Vector3& up);
        static Matrix4x4 cameraInverse(const Vector3& eye, const Vector3& look, const Vector3& up);

        double m11_, m12_, m13_, m14_,
               m21_, m22_, m23_, m24_,
               m31_, m32_, m33_, m34_,
               m41_, m42_, m43_, m44_;
    };

}

#endif  // CYRONENO_MATRIX_H_