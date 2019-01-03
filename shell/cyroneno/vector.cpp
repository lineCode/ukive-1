#include "vector.h"

#include <cmath>


namespace cyro {

    //////////
    Vector2::Vector2()
        : x_(0), y_(0) {
    }

    Vector2::Vector2(double x, double y)
        : x_(x), y_(y) {
    }

    Vector2::Vector2(const Vector2& rhs)
        : x_(rhs.x_), y_(rhs.y_) {
    }

    Vector2& Vector2::operator=(const Vector2& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        return *this;
    }

    Vector2 Vector2::operator+(const Vector2& rhs) const {
        Vector2 v(*this);
        v.add(rhs);
        return v;
    }

    Vector2 Vector2::operator-() const {
        Vector2 v(*this);
        v.inv();
        return v;
    }

    Vector2 Vector2::operator-(const Vector2& rhs) const {
        Vector2 v(*this);
        v.sub(rhs);
        return v;
    }

    Vector2 Vector2::operator*(double factor) const {
        Vector2 v(*this);
        v.mul(factor);
        return v;
    }

    Vector2 Vector2::operator/(double factor) const {
        Vector2 v(*this);
        v.div(factor);
        return v;
    }

    double Vector2::operator*(const Vector2& rhs) const {
        return x_ * rhs.x_ + y_ * rhs.y_;
    }

    Vector2 Vector2::normalize() const {
        Vector2 v(*this);
        v.nor();
        return v;
    }

    void Vector2::add(const Vector2& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
    }

    void Vector2::sub(const Vector2& rhs) {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
    }

    void Vector2::mul(double factor) {
        x_ *= factor;
        y_ *= factor;
    }

    void Vector2::div(double factor) {
        x_ /= factor;
        y_ /= factor;
    }

    void Vector2::inv() {
        x_ = -x_;
        y_ = -y_;
    }

    void Vector2::nor() {
        auto l = length();
        x_ /= l;
        y_ /= l;
    }

    double Vector2::length() const {
        return sqrt(x_* x_ + y_ * y_);
    }


    //////////
    Vector3::Vector3()
        : x_(0), y_(0), z_(0) {
    }

    Vector3::Vector3(double x, double y, double z)
        : x_(x), y_(y), z_(z) {
    }

    Vector3::Vector3(const Vector3& rhs)
        : x_(rhs.x_), y_(rhs.y_), z_(rhs.z_) {
    }

    Vector3& Vector3::operator=(const Vector3& rhs) {
        x_ = rhs.x_;
        y_ = rhs.y_;
        z_ = rhs.z_;
        return *this;
    }

    Vector3 Vector3::operator+(const Vector3& rhs) const {
        Vector3 v(*this);
        v.add(rhs);
        return v;
    }

    Vector3 Vector3::operator-() const {
        Vector3 v(*this);
        v.inv();
        return v;
    }

    Vector3 Vector3::operator-(const Vector3& rhs) const {
        Vector3 v(*this);
        v.sub(rhs);
        return v;
    }

    Vector3 Vector3::operator*(double factor) const {
        Vector3 v(*this);
        v.mul(factor);
        return v;
    }

    Vector3 Vector3::operator/(double factor) const {
        Vector3 v(*this);
        v.div(factor);
        return v;
    }

    double Vector3::operator*(const Vector3& rhs) const {
        return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
    }

    Vector3 Vector3::operator^(const Vector3& rhs) const {
        Vector3 v(*this);
        v.cross(rhs);
        return v;
    }

    Vector3 Vector3::normalize() const {
        Vector3 v(*this);
        v.nor();
        return v;
    }

    void Vector3::add(const Vector3& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
    }

    void Vector3::sub(const Vector3& rhs) {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        z_ -= rhs.z_;
    }

    void Vector3::mul(double factor) {
        x_ *= factor;
        y_ *= factor;
        z_ *= factor;
    }

    void Vector3::div(double factor) {
        x_ /= factor;
        y_ /= factor;
        z_ /= factor;
    }

    void Vector3::cross(const Vector3& rhs) {
        auto x = y_ * rhs.z_ - z_ * rhs.y_;
        auto y = z_ * rhs.x_ - x_ * rhs.z_;
        auto z = x_ * rhs.y_ - y_ * rhs.x_;
        x_ = x;
        y_ = y;
        z_ = z;
    }

    void Vector3::inv() {
        x_ = -x_;
        y_ = -y_;
        z_ = -z_;
    }

    void Vector3::nor() {
        auto l = length();
        x_ /= l;
        y_ /= l;
        z_ /= l;
    }

    double Vector3::length() const {
        return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }


    //////////
    Vector4::Vector4()
        : x_(0), y_(0), z_(0), w_(0) {
    }

    Vector4::Vector4(double x, double y, double z, double w)
        : x_(x), y_(y), z_(z), w_(w) {
    }

    Vector4::Vector4(const Vector4& rhs)
        : x_(rhs.x_), y_(rhs.y_), z_(rhs.z_), w_(rhs.w_) {
    }

    Vector4& Vector4::operator=(const Vector4& rhs) {
        x_ = rhs.x_;
        y_ = rhs.y_;
        z_ = rhs.z_;
        w_ = rhs.w_;
        return *this;
    }

    Vector4 Vector4::operator+(const Vector4& rhs) const {
        Vector4 v(*this);
        v.add(rhs);
        return v;
    }

    Vector4 Vector4::operator-() const {
        Vector4 v(*this);
        v.inv();
        return v;
    }

    Vector4 Vector4::operator-(const Vector4& rhs) const {
        Vector4 v(*this);
        v.sub(rhs);
        return v;
    }

    Vector4 Vector4::operator*(double factor) const {
        Vector4 v(*this);
        v.mul(factor);
        return v;
    }

    Vector4 Vector4::operator/(double factor) const {
        Vector4 v(*this);
        v.div(factor);
        return v;
    }

    double Vector4::operator*(const Vector4& rhs) const {
        return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_ + w_ * rhs.w_;
    }

    Vector4 Vector4::normalize() const {
        Vector4 v(*this);
        v.nor();
        return v;
    }

    void Vector4::add(const Vector4& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
        w_ += rhs.w_;
    }

    void Vector4::sub(const Vector4& rhs) {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        z_ -= rhs.z_;
        w_ -= rhs.w_;
    }

    void Vector4::mul(double factor) {
        x_ *= factor;
        y_ *= factor;
        z_ *= factor;
        w_ *= factor;
    }

    void Vector4::div(double factor) {
        x_ /= factor;
        y_ /= factor;
        z_ /= factor;
        w_ /= factor;
    }

    void Vector4::inv() {
        x_ = -x_;
        y_ = -y_;
        z_ = -z_;
        w_ = -w_;
    }

    void Vector4::nor() {
        auto l = length();
        x_ /= l;
        y_ /= l;
        z_ /= l;
        w_ /= l;
    }

    double Vector4::length() const {
        return sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
    }

}