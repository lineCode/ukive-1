#ifndef CYRONENO_VECTOR_H_
#define CYRONENO_VECTOR_H_

#include <cmath>
#include <type_traits>


namespace cyro {

    template<typename T>
    class Vector2T {
    public:
        using F = std::enable_if<std::is_floating_point<T>::value>;

        Vector2T() : x(0), y(0) {}
        Vector2T(T x, T y) : x(x), y(y) {}

        Vector2T operator+(const Vector2T& rhs) const { return Vector2T(*this).add(rhs); }
        Vector2T operator-(const Vector2T& rhs) const { return Vector2T(*this).sub(rhs); }
        Vector2T operator-() const { return Vector2T(*this).inv(); }
        Vector2T operator*(T factor) const { return Vector2T(*this).mul(factor); }
        T operator*(const Vector2T& rhs) const { return Vector2T(*this).mul(rhs); }
        bool operator==(const Vector2T& rhs) const { return isEqual(rhs); }
        bool operator!=(const Vector2T& rhs) const { return !isEqual(rhs); }

        template<typename C>
        operator Vector2T<C>() const {
            return Vector2T<C>(x, y);
        }

        template<typename = typename F::type>
        Vector2T operator/(T factor) const { return Vector2T(*this).div(factor); }
        template<typename = typename F::type>
        Vector2T normalize() const { return Vector2T(*this).nor(); }

        Vector2T& add(const Vector2T& rhs) {
            x += rhs.x; y += rhs.y;
            return *this;
        }
        Vector2T& sub(const Vector2T& rhs) {
            x -= rhs.x; y -= rhs.y;
            return *this;
        }
        Vector2T& mul(T factor) {
            x *= factor; y *= factor;
            return *this;
        }
        Vector2T& inv() {
            x = -x; y = -y;
            return *this;
        }
        T mul(const Vector2T& rhs) {
            return x * rhs.x + y * rhs.y;
        }
        T lengthSq() const {
            return x*x + y*y;
        }
        bool isEqual(const Vector2T& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        template<typename = typename F::type>
        Vector2T& div(T factor) {
            x /= factor; y /= factor;
            return *this;
        }
        template<typename = typename F::type>
        Vector2T& nor() {
            auto l = length();
            x /= l; y /= l;
            return *this;
        }
        template<typename = typename F::type>
        T length() const {
            return sqrt(x*x + y*y);
        }

        T x, y;
    };


    template<typename T>
    class Vector3T {
    public:
        using F = std::enable_if<std::is_floating_point<T>::value>;

        Vector3T() : x(0), y(0), z(0) {}
        Vector3T(T x, T y, T z) : x(x), y(y), z(z) {}

        Vector3T operator+(const Vector3T& rhs) const { return Vector3T(*this).add(rhs); }
        Vector3T operator-(const Vector3T& rhs) const { return Vector3T(*this).sub(rhs); }
        Vector3T operator-() const { return Vector3T(*this).inv(); }
        Vector3T operator*(T factor) const { return Vector3T(*this).mul(factor); }
        T operator*(const Vector3T& rhs) const { return Vector3T(*this).mul(rhs); }
        Vector3T operator^(const Vector3T& rhs) const { return Vector3T(*this).cross(rhs); }
        bool operator==(const Vector3T& rhs) const { return isEqual(rhs); }
        bool operator!=(const Vector3T& rhs) const { return !isEqual(rhs); }

        template<typename C>
        operator Vector3T<C>() const {
            return Vector3T<C>(x, y, z);
        }

        template<typename = typename F::type>
        Vector3T operator/(T factor) const { return Vector3T(*this).div(factor); }
        template<typename = typename F::type>
        Vector3T normalize() const { return Vector3T(*this).nor(); }

        Vector3T& add(const Vector3T& rhs) {
            x += rhs.x; y += rhs.y; z += rhs.z;
            return *this;
        }
        Vector3T& sub(const Vector3T& rhs) {
            x -= rhs.x; y -= rhs.y; z -= rhs.z;
            return *this;
        }
        Vector3T& mul(T factor) {
            x *= factor; y *= factor; z *= factor;
            return *this;
        }
        Vector3T& inv() {
            x = -x; y = -y; z = -z;
            return *this;
        }
        Vector3T& cross(const Vector3T& rhs) {
            auto _x = y * rhs.z - z * rhs.y;
            auto _y = z * rhs.x - x * rhs.z;
            auto _z = x * rhs.y - y * rhs.x;
            x = _x; y = _y; z = _z;
            return *this;
        }
        T mul(const Vector3T& rhs) {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }
        T lengthSq() const {
            return x*x + y*y + z*z;
        }
        bool isEqual(const Vector3T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        template<typename = typename F::type>
        Vector3T& div(T factor) {
            x /= factor; y /= factor; z /= factor;
            return *this;
        }
        template<typename = typename F::type>
        Vector3T& nor() {
            auto l = length();
            x /= l; y /= l; z /= l;
            return *this;
        }
        template<typename = typename F::type>
        T length() const {
            return sqrt(x*x + y*y + z*z);
        }

        T x, y, z;
    };


    template<typename T>
    class Vector4T {
    public:
        using F = std::enable_if<std::is_floating_point<T>::value>;

        Vector4T() : x(0), y(0), z(0), w(0) {}
        Vector4T(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        Vector4T operator+(const Vector4T& rhs) const { return Vector4T(*this).add(rhs); }
        Vector4T operator-(const Vector4T& rhs) const { return Vector4T(*this).sub(rhs); }
        Vector4T operator-() const { return Vector4T(*this).inv(); }
        Vector4T operator*(T factor) const { return Vector4T(*this).mul(factor); }
        T operator*(const Vector4T& rhs) const { return Vector4T(*this).mul(rhs); }
        bool operator==(const Vector4T& rhs) const { return isEqual(rhs); }
        bool operator!=(const Vector4T& rhs) const { return !isEqual(rhs); }

        template<typename C>
        operator Vector4T<C>() const {
            return Vector4T<C>(x, y, z, w);
        }

        template<typename = typename F::type>
        Vector4T operator/(T factor) const { return Vector4T(*this).div(factor); }
        template<typename = typename F::type>
        Vector4T normalize() const { return Vector4T(*this).nor(); }

        Vector4T& add(const Vector4T& rhs) {
            x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
            return *this;
        }
        Vector4T& sub(const Vector4T& rhs) {
            x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
            return *this;
        }
        Vector4T& mul(T factor) {
            x *= factor; y *= factor; z *= factor; w *= factor;
            return *this;
        }
        Vector4T& inv() {
            x = -x; y = -y; z = -z; w = -w;
            return *this;
        }
        T mul(const Vector4T& rhs) {
            return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
        }
        T lengthSq() const {
            return x * x + y * y + z * z + w * w;
        }
        bool isEqual(const Vector4T& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        template<typename = typename F::type>
        Vector4T& div(T factor) {
            x /= factor; y /= factor; z /= factor; w /= factor;
            return *this;
        }
        template<typename = typename F::type>
        Vector4T& nor() {
            auto l = length();
            x /= l; y /= l; z /= l; w /= l;
            return *this;
        }
        template<typename = typename F::type>
        T length() const {
            return sqrt(x*x + y * y + z * z + w * w);
        }

        T x, y, z, w;
    };


    using Vector2 = Vector2T<double>;
    using Vector3 = Vector3T<double>;
    using Vector4 = Vector4T<double>;

}

#endif  // CYRONENO_VECTOR_H_