#include "matrix.h"


namespace cyro {

    //////////
    Matrix3x3::Matrix3x3()
        : m11_(0), m12_(0), m13_(0),
          m21_(0), m22_(0), m23_(0),
          m31_(0), m32_(0), m33_(0) {
    }

    Matrix3x3::Matrix3x3(const Matrix3x3& rhs)
        : m11_(rhs.m11_), m12_(rhs.m12_), m13_(rhs.m13_),
          m21_(rhs.m21_), m22_(rhs.m22_), m23_(rhs.m23_),
          m31_(rhs.m31_), m32_(rhs.m32_), m33_(rhs.m33_) {
    }

    Matrix3x3& Matrix3x3::operator=(const Matrix3x3& rhs) {
        m11_ = rhs.m11_; m12_ = rhs.m12_; m13_ = rhs.m13_;
        m21_ = rhs.m21_; m22_ = rhs.m22_; m23_ = rhs.m23_;
        m31_ = rhs.m31_; m32_ = rhs.m32_; m33_ = rhs.m33_;
        return *this;
    }

    Matrix3x3 Matrix3x3::operator+(const Matrix3x3& rhs) {
        Matrix3x3 m(*this);
        m.add(rhs);
        return m;
    }

    Matrix3x3 Matrix3x3::operator-(const Matrix3x3& rhs) {
        Matrix3x3 m(*this);
        m.sub(rhs);
        return m;
    }

    Matrix3x3 Matrix3x3::operator*(const Matrix3x3& rhs) {
        Matrix3x3 m(*this);
        m.mul(rhs);
        return m;
    }

    Matrix3x3 Matrix3x3::operator*(double factor) {
        Matrix3x3 m(*this);
        m.mul(factor);
        return m;
    }

    Matrix3x3 Matrix3x3::operator/(double factor) {
        Matrix3x3 m(*this);
        m.div(factor);
        return m;
    }

    void Matrix3x3::identity() {
        m11_ = 1; m12_ = 0; m13_ = 0;
        m21_ = 0; m22_ = 1; m23_ = 0;
        m31_ = 0; m32_ = 0; m33_ = 1;
    }

    void Matrix3x3::add(const Matrix3x3& rhs) {
        m11_ += rhs.m11_; m12_ += rhs.m12_; m13_ += rhs.m13_;
        m21_ += rhs.m21_; m22_ += rhs.m22_; m23_ += rhs.m23_;
        m31_ += rhs.m31_; m32_ += rhs.m32_; m33_ += rhs.m33_;
    }

    void Matrix3x3::sub(const Matrix3x3& rhs) {
        m11_ -= rhs.m11_; m12_ -= rhs.m12_; m13_ -= rhs.m13_;
        m21_ -= rhs.m21_; m22_ -= rhs.m22_; m23_ -= rhs.m23_;
        m31_ -= rhs.m31_; m32_ -= rhs.m32_; m33_ -= rhs.m33_;
    }

    void Matrix3x3::mul(const Matrix3x3& rhs) {
        auto m11 = m11_ * rhs.m11_ + m12_ * rhs.m21_ + m13_ * rhs.m31_;
        auto m12 = m11_ * rhs.m12_ + m12_ * rhs.m22_ + m13_ * rhs.m32_;
        auto m13 = m11_ * rhs.m13_ + m12_ * rhs.m23_ + m13_ * rhs.m33_;

        auto m21 = m21_ * rhs.m11_ + m22_ * rhs.m21_ + m23_ * rhs.m31_;
        auto m22 = m21_ * rhs.m12_ + m22_ * rhs.m22_ + m23_ * rhs.m32_;
        auto m23 = m21_ * rhs.m13_ + m22_ * rhs.m23_ + m23_ * rhs.m33_;

        auto m31 = m31_ * rhs.m11_ + m32_ * rhs.m21_ + m33_ * rhs.m31_;
        auto m32 = m31_ * rhs.m12_ + m32_ * rhs.m22_ + m33_ * rhs.m32_;
        auto m33 = m31_ * rhs.m13_ + m32_ * rhs.m23_ + m33_ * rhs.m33_;

        m11_ = m11; m12_ = m12; m13_ = m13;
        m21_ = m21; m22_ = m22; m23_ = m23;
        m31_ = m31; m32_ = m32; m33_ = m33;
    }

    void Matrix3x3::mul(double factor) {
        m11_ *= factor; m12_ *= factor; m13_ *= factor;
        m21_ *= factor; m22_ *= factor; m23_ *= factor;
        m31_ *= factor; m32_ *= factor; m33_ *= factor;
    }

    void Matrix3x3::div(double factor) {
        m11_ /= factor; m12_ /= factor; m13_ /= factor;
        m21_ /= factor; m22_ /= factor; m23_ /= factor;
        m31_ /= factor; m32_ /= factor; m33_ /= factor;
    }


    //////////
    Matrix4x4::Matrix4x4()
        : m11_(0), m12_(0), m13_(0), m14_(0),
          m21_(0), m22_(0), m23_(0), m24_(0),
          m31_(0), m32_(0), m33_(0), m34_(0),
          m41_(0), m42_(0), m43_(0), m44_(0) {}

    Matrix4x4::Matrix4x4(const Matrix4x4& rhs)
        : m11_(rhs.m11_), m12_(rhs.m12_), m13_(rhs.m13_), m14_(rhs.m14_),
          m21_(rhs.m21_), m22_(rhs.m22_), m23_(rhs.m23_), m24_(rhs.m24_),
          m31_(rhs.m31_), m32_(rhs.m32_), m33_(rhs.m33_), m34_(rhs.m34_),
          m41_(rhs.m41_), m42_(rhs.m42_), m43_(rhs.m43_), m44_(rhs.m44_) {}

    Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs) {
        m11_ = rhs.m11_; m12_ = rhs.m12_; m13_ = rhs.m13_; m14_ = rhs.m14_;
        m21_ = rhs.m21_; m22_ = rhs.m22_; m23_ = rhs.m23_; m24_ = rhs.m24_;
        m31_ = rhs.m31_; m32_ = rhs.m32_; m33_ = rhs.m33_; m34_ = rhs.m34_;
        m41_ = rhs.m41_; m42_ = rhs.m42_; m43_ = rhs.m43_; m44_ = rhs.m44_;
        return *this;
    }

    Matrix4x4 Matrix4x4::operator+(const Matrix4x4& rhs) {
        Matrix4x4 m(*this);
        m.add(rhs);
        return m;
    }

    Matrix4x4 Matrix4x4::operator-(const Matrix4x4& rhs) {
        Matrix4x4 m(*this);
        m.sub(rhs);
        return m;
    }

    Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) {
        Matrix4x4 m(*this);
        m.mul(rhs);
        return m;
    }

    Matrix4x4 Matrix4x4::operator*(double factor) {
        Matrix4x4 m(*this);
        m.mul(factor);
        return m;
    }

    Matrix4x4 Matrix4x4::operator/(double factor) {
        Matrix4x4 m(*this);
        m.div(factor);
        return m;
    }

    void Matrix4x4::identity() {
        m11_ = 1; m12_ = 0; m13_ = 0; m14_ = 0;
        m21_ = 0; m22_ = 1; m23_ = 0; m24_ = 0;
        m31_ = 0; m32_ = 0; m33_ = 1; m34_ = 0;
        m41_ = 0; m42_ = 0; m43_ = 0; m44_ = 1;
    }

    void Matrix4x4::add(const Matrix4x4& rhs) {
        m11_ += rhs.m11_; m12_ += rhs.m12_; m13_ += rhs.m13_; m14_ += rhs.m14_;
        m21_ += rhs.m21_; m22_ += rhs.m22_; m23_ += rhs.m23_; m24_ += rhs.m24_;
        m31_ += rhs.m31_; m32_ += rhs.m32_; m33_ += rhs.m33_; m34_ += rhs.m34_;
        m41_ += rhs.m41_; m42_ += rhs.m42_; m43_ += rhs.m43_; m44_ += rhs.m44_;
    }

    void Matrix4x4::sub(const Matrix4x4& rhs) {
        m11_ -= rhs.m11_; m12_ -= rhs.m12_; m13_ -= rhs.m13_; m14_ -= rhs.m14_;
        m21_ -= rhs.m21_; m22_ -= rhs.m22_; m23_ -= rhs.m23_; m24_ -= rhs.m24_;
        m31_ -= rhs.m31_; m32_ -= rhs.m32_; m33_ -= rhs.m33_; m34_ -= rhs.m34_;
        m41_ -= rhs.m41_; m42_ -= rhs.m42_; m43_ -= rhs.m43_; m44_ -= rhs.m44_;
    }

    void Matrix4x4::mul(const Matrix4x4& rhs) {
        auto m11 = m11_ * rhs.m11_ + m12_ * rhs.m21_ + m13_ * rhs.m31_ + m14_ * rhs.m41_;
        auto m12 = m11_ * rhs.m12_ + m12_ * rhs.m22_ + m13_ * rhs.m32_ + m14_ * rhs.m42_;
        auto m13 = m11_ * rhs.m13_ + m12_ * rhs.m23_ + m13_ * rhs.m33_ + m14_ * rhs.m43_;
        auto m14 = m11_ * rhs.m14_ + m12_ * rhs.m24_ + m13_ * rhs.m34_ + m14_ * rhs.m44_;

        auto m21 = m21_ * rhs.m11_ + m22_ * rhs.m21_ + m23_ * rhs.m31_ + m24_ * rhs.m41_;
        auto m22 = m21_ * rhs.m12_ + m22_ * rhs.m22_ + m23_ * rhs.m32_ + m24_ * rhs.m42_;
        auto m23 = m21_ * rhs.m13_ + m22_ * rhs.m23_ + m23_ * rhs.m33_ + m24_ * rhs.m43_;
        auto m24 = m21_ * rhs.m14_ + m22_ * rhs.m23_ + m23_ * rhs.m34_ + m24_ * rhs.m44_;

        auto m31 = m31_ * rhs.m11_ + m32_ * rhs.m21_ + m33_ * rhs.m31_ + m34_ * rhs.m41_;
        auto m32 = m31_ * rhs.m12_ + m32_ * rhs.m22_ + m33_ * rhs.m32_ + m34_ * rhs.m42_;
        auto m33 = m31_ * rhs.m13_ + m32_ * rhs.m23_ + m33_ * rhs.m33_ + m34_ * rhs.m43_;
        auto m34 = m31_ * rhs.m14_ + m32_ * rhs.m24_ + m33_ * rhs.m34_ + m34_ * rhs.m44_;

        auto m41 = m41_ * rhs.m11_ + m42_ * rhs.m21_ + m43_ * rhs.m31_ + m44_ * rhs.m41_;
        auto m42 = m41_ * rhs.m12_ + m42_ * rhs.m22_ + m43_ * rhs.m32_ + m44_ * rhs.m42_;
        auto m43 = m41_ * rhs.m13_ + m42_ * rhs.m23_ + m43_ * rhs.m33_ + m44_ * rhs.m43_;
        auto m44 = m41_ * rhs.m14_ + m42_ * rhs.m24_ + m43_ * rhs.m34_ + m44_ * rhs.m44_;

        m11_ = m11; m12_ = m12; m13_ = m13; m14_ = m14;
        m21_ = m21; m22_ = m22; m23_ = m23; m24_ = m24;
        m31_ = m31; m32_ = m32; m33_ = m33; m34_ = m34;
        m41_ = m41; m42_ = m42; m43_ = m43; m44_ = m44;
    }

    void Matrix4x4::mul(double factor) {
        m11_ *= factor; m12_ *= factor; m13_ *= factor; m14_ *= factor;
        m21_ *= factor; m22_ *= factor; m23_ *= factor; m24_ *= factor;
        m31_ *= factor; m32_ *= factor; m33_ *= factor; m34_ *= factor;
        m41_ *= factor; m42_ *= factor; m43_ *= factor; m44_ *= factor;
    }

    void Matrix4x4::div(double factor) {
        m11_ /= factor; m12_ /= factor; m13_ /= factor; m14_ /= factor;
        m21_ /= factor; m22_ /= factor; m23_ /= factor; m24_ /= factor;
        m31_ /= factor; m32_ /= factor; m33_ /= factor; m34_ /= factor;
        m41_ /= factor; m42_ /= factor; m43_ /= factor; m44_ /= factor;
    }
}