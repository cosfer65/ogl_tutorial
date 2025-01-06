#ifndef __basevec_h__
#define __basevec_h__
#include <math.h>

template <typename T>
class basevec2 {
    T* data;
public:
    T x, y;
    basevec2(T _d = 0) :x(_d), y(_d), data(&x) {}
    basevec2(T _x, T _y) :x(_x), y(_y), data(&x) {}
    basevec2(const basevec2& _b) :x(_b.x), y(_b.y), data(&x) {}
    T& operator[](int index) {
        return data[index];
    }
    T operator[](int index) const {
        return data[index];
    }
    basevec2& operator=(const basevec2& v) {
        x = v.x; y = v.y;
        return *this;
    }
    basevec2& operator+=(const basevec2& s) {    // scalar multiply
        x += s.x; y += s.y;
        return *this;
    }
    basevec2& operator-=(const basevec2& s) {    // scalar multiply
        x -= s.x; y -= s.y;
        return *this;
    }
    basevec2& operator*=(T s) {    // scalar multiply
        x *= (T)s; y *= (T)s;
        return *this;
    }
    basevec2& operator/=(T s) {    // scalar divide
        x /= (T)s; y /= (T)s;
        return *this;
    }
    basevec2 operator-(void) {
        return basevec2(-x, -y);
    }
    operator T* () {
        return data;
    }
    basevec2& scale(T s) {
        x *= (T)s; y *= (T)s;
        return *this;
    }
    T length() const {
        return (T)sqrt(x * x + y * y);
    }
    basevec2& normalize() {
        T fLength = length();
        if (fLength > 0.0001)
            fLength = 1.0f / fLength;
        scale(fLength);
        return *this;
    }
    basevec2 inverse() {
        return basevec3(-x, -y);
    }
};

template <typename T>
class basevec3 {
    T* data;
public:
    T x, y, z;
    basevec3(T _d = 0) : x(_d), y(_d), z(_d), data(&x) {
    }
    basevec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z), data(&x) {
    }
    basevec3(const basevec3& v) : x(v.x), y(v.y), z(v.z), data(&x) {
    }
    basevec3(const basevec2<T>& v, T _z) : x(v.x), y(v.y), z(_z), data(&x) {
    }
    T& operator[](int index) {
        return data[index];
    }
    T operator[](int index) const {
        return data[index];
    }
    basevec3& operator=(const basevec3& v) {
        x = v.x; y = v.y; z = v.z;
        return *this;
    }
    basevec3<T>& operator=(T s) {
        x = y = z = (T)s;
        return *this;
    }
    basevec3& operator+=(const basevec3& s) {
        x += s.x; y += s.y; z += s.z;
        return *this;
    }
    basevec3& operator-=(const basevec3& s) {
        x -= s.x; y -= s.y; z -= s.z;
        return *this;
    }
    basevec3& operator*=(T s) {    // scalar multiply
        x *= (T)s; y *= (T)s; z *= (T)s;
        return *this;
    }
    basevec3& operator/=(T s) {    // scalar divide
        x /= (T)s; y /= (T)s; z /= (T)s;
        return *this;
    }
    basevec3 operator-(void) {
        return basevec3(-x, -y, -z);
    }
    operator T* () {
        return data;
    }
    basevec3& scale(T s) {
        x *= (T)s; y *= (T)s; z *= (T)s;
        return *this;
    }
    T length() const {
        return (T)sqrt(x * x + y * y + z * z);
    }
    basevec3& normalize() {
        T fLength = length();
        if (fLength > 0.0001)
            fLength = 1.0f / fLength;
        scale(fLength);
        return *this;
    }
    basevec3 inverse() {
        return basevec3(-x, -y, -z);
    }
};
template <typename T>
class basevec4 {
    T* data;
public:
    T x, y, z, w;
    basevec4(T _d = 0) : x(_d), y(_d), z(_d), w(_d), data(&x) {
    }
    basevec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w), data(&x) {
    }
    basevec4(const basevec4& v) : x(v.x), y(v.y), z(v.z), w(v.w), data(&x) {
    }
    basevec4(const basevec3<T>& v, T _w) : x(v.x), y(v.y), z(v.z), w(_w), data(&x) {
    }
    ~basevec4() {}

    T& operator[](int index) {
        return data[index];
    }
    T operator[](int index) const {
        return data[index];
    }
    basevec4& operator=(const basevec4& v) {
        x = v.x; y = v.y; z = v.z; w = v.w;
        return *this;
    }
    basevec4& operator+=(const basevec4& s) {    // scalar multiply
        x += s.x; y += s.y; z += s.z; w += s.w;
        return *this;
    }
    basevec4& operator-=(const basevec4& s) {    // scalar multiply
        x -= s.x; y -= s.y; z -= s.z; w -= s.w;
        return *this;
    }
    basevec4& operator*=(T s) {    // scalar multiply
        x *= (T)s; y *= (T)s; z *= (T)s; w *= (T)s;
        return *this;
    }
    basevec4& operator/=(T s) {    // scalar divide
        x /= (T)s; y /= (T)s; z /= (T)s; w /= (T)s;
        return *this;
    }
    basevec4 operator-(void) {
        return basevec4(-x, -y, -z, -w);
    }
    operator T* () {
        return data;
    }
    basevec4& scale(T s) {
        x *= (T)s; y *= (T)s; z *= (T)s; w *= (T)s;
        return *this;
    }
    T length() const {
        return (T)sqrt(x * x + y * y + z * z + w * w);
    }
    basevec4& normalize() {
        T fLength = length();
        if (fLength > 0.0001) {
            fLength = 1.0f / fLength;
            scale(fLength);
        }
        return *this;
    }
    basevec4 inverse() {
        return basevec4(-x, -y, -z, -w);
    }
};
// ADD, SUBTRACT, MULTIPLY
template <typename T>
inline basevec2<T> operator+(const basevec2<T>& m1, const basevec2<T>& m2) {
    basevec2<T> r(m1);
    r.x += m2.x; r.y += m2.y;
    return r;
}
template <typename T>
inline basevec2<T> operator-(const basevec2<T>& m1, const basevec2<T>& m2) {
    basevec2<T> r(m1);
    r.x -= m2.x; r.y -= m2.y;
    return r;
}

template <typename T>
inline basevec3<T> operator+(const basevec3<T>& m1, const basevec3<T>& m2) {
    basevec3<T> r(m1);
    r.x += m2.x; r.y += m2.y; r.z += m2.z;
    return r;
}
template <typename T>
inline basevec3<T> operator-(const basevec3<T>& m1, const basevec3<T>& m2) {
    basevec3<T> r(m1);
    r.x -= m2.x; r.y -= m2.y; r.z -= m2.z;
    return r;
}
template <typename T>
inline basevec3<T> operator*(const basevec3<T>& v1, const basevec3<T>& v2) {
    basevec3<T> vResult;
    vResult.x = v1.y * v2.z - v2.y * v1.z;
    vResult.y = -v1.x * v2.z + v2.x * v1.z;
    vResult.z = v1.x * v2.y - v2.x * v1.y;
    return vResult;
}

template <typename T>
inline basevec4<T> operator+(const basevec4<T>& m1, const basevec4<T>& m2) {
    basevec4<T> r(m1);
    r.x += m2.x; r.y += m2.y; r.z += m2.z; r.w += m2.w;
    return r;
}
template <typename T>
inline basevec4<T> operator-(const basevec4<T>& m1, const basevec4<T>& m2) {
    basevec4<T> r(m1);
    r.x -= m2.x; r.y -= m2.y; r.z -= m2.z; r.w -= m2.w;
    return r;
}
/*
* inline basevec3<T> operator*(const basevec3<T>& v1, const basevec3<T>& v2)
* is not defined in 4d
*/

// scalar functions with other value types
template <typename T, typename Q>
inline basevec3<T> operator*(const basevec3<T>& v1, Q s) {
    // scalar multiply
    basevec3<T> r(v1);
    r.x *= (T)s; r.y *= (T)s; r.z *= (T)s;
    return r;
}
template <typename T, typename Q>
inline basevec3<T> operator*(Q s, const basevec3<T>& v1)
{    // scalar multiply
    basevec3<T> r(v1);
    r.x *= (T)s; r.y *= (T)s; r.z *= (T)s;
    return r;
}
template <typename T, typename Q>
inline basevec3<T> operator/(const basevec3<T>& v1, Q s)
{    // scalar divide
    basevec3<T> r(v1);
    r.x /= (T)s; r.y /= (T)s; r.z /= (T)s;
    return r;
}

template <typename T, typename Q>
inline basevec4<T> operator*(const basevec4<T>& v1, Q s) {
    // scalar multiply
    basevec4<T> r(v1);
    r.x *= (T)s; r.y *= (T)s; r.z *= (T)s; r.w *= (T)s;
    return r;
}
template <typename T, typename Q>
inline basevec4<T> operator*(Q s, const basevec4<T>& v1)
{    // scalar multiply
    basevec4<T> r(v1);
    r.x *= (T)s; r.y *= (T)s; r.z *= (T)s; r.w *= (T)s;
    return r;
}
template <typename T, typename Q>
inline basevec4<T> operator/(const basevec4<T>& v1, Q s)
{    // scalar divide
    basevec4<T> r(v1);
    r.x /= (T)s; r.y /= (T)s; r.z /= (T)s; r.w /= (T)s;
    return r;
}
template <typename T>
inline T dot(const basevec2<T>& v1, const basevec2<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}
template <typename T>
inline T dot(const basevec3<T>& v1, const basevec3<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
template <typename T>
inline T dot(const basevec4<T>& v1, const basevec4<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}
template <typename T>
inline basevec3<T> cross(const basevec3<T>& v1, const basevec3<T>& v2) {
    basevec3<T> vResult;
    vResult.x = v1.y * v2.z - v1.z * v2.y;
    vResult.y = v1.z * v2.x - v1.x * v2.z;
    vResult.z = v1.x * v2.y - v1.y * v2.x;
    return vResult;
}
#endif // __basevec_h__
