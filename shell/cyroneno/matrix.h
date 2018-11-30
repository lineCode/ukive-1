#ifndef CYRONENO_MATRIX_H_
#define CYRONENO_MATRIX_H_


namespace cyro {

    class Matrix3x3 {
    public:
        Matrix3x3();
        Matrix3x3(const Matrix3x3& rhs);

        Matrix3x3& operator=(const Matrix3x3& rhs);
        Matrix3x3 operator+(const Matrix3x3& rhs);
        Matrix3x3 operator-(const Matrix3x3& rhs);
        Matrix3x3 operator*(const Matrix3x3& rhs);
        Matrix3x3 operator*(double factor);
        Matrix3x3 operator/(double factor);

        void identity();
        void add(const Matrix3x3& rhs);
        void sub(const Matrix3x3& rhs);
        void mul(const Matrix3x3& rhs);
        void mul(double factor);
        void div(double factor);

        double m11_, m12_, m13_,
               m21_, m22_, m23_,
               m31_, m32_, m33_;
    };


    class Matrix4x4 {
    public:
        Matrix4x4();
        Matrix4x4(const Matrix4x4& rhs);

        Matrix4x4& operator=(const Matrix4x4& rhs);
        Matrix4x4 operator+(const Matrix4x4& rhs);
        Matrix4x4 operator-(const Matrix4x4& rhs);
        Matrix4x4 operator*(const Matrix4x4& rhs);
        Matrix4x4 operator*(double factor);
        Matrix4x4 operator/(double factor);

        void identity();
        void add(const Matrix4x4& rhs);
        void sub(const Matrix4x4& rhs);
        void mul(const Matrix4x4& rhs);
        void mul(double factor);
        void div(double factor);

        double m11_, m12_, m13_, m14_,
               m21_, m22_, m23_, m24_,
               m31_, m32_, m33_, m34_,
               m41_, m42_, m43_, m44_;
    };

}

#endif  // CYRONENO_MATRIX_H_