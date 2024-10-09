#ifndef __cg_basematrix_h__
#define __cg_basematrix_h__

#include <string.h> // for memcpy

template <typename T>
class basemat3 {
public:
    T DetIJ(int i, int j) {
        int x, y, ii, jj;
        T ret, mat[2][2];
        x = 0;
        for (ii = 0; ii < 3; ii++)
        {
            if (ii == i) continue;
            y = 0;
            for (jj = 0; jj < 3; jj++)
            {
                if (jj == j) continue;
                mat[x][y] = data[(ii * 3) + jj];
                y++;
            }
            x++;
        }
        ret = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
        return ret;
    }
public:
    T data[9];
    basemat3() {
        memset(data, 0, 9 * sizeof(T));
    }
    basemat3(const basemat3& m) {
        memcpy(data, m.data, 9 * sizeof(T));
    }
    basemat3(T d) {
        data[0] = data[3] = data[6] = data[1] = data[4] = data[7] = data[2] = data[5] = data[8] = d;
    }
    basemat3(T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8) {
        data[0] = d0; data[3] = d3; data[6] = d6;
        data[1] = d1; data[4] = d4; data[7] = d7;
        data[2] = d2; data[5] = d5; data[8] = d8;
    }
    void loadIdentity() {
        static T identity[] = { T(1), T(0), T(0),
                                    T(0), T(1), T(0),
                                    T(0), T(0), T(1) };
        memcpy(data, identity, 9 * sizeof(T));
    }
    T det(void) {
        return	data[0] * data[4] * data[8] + data[3] * data[7] * data[2] +
            data[6] * data[1] * data[5] - data[2] * data[4] * data[6] -
            data[5] * data[7] * data[0] - data[8] * data[1] * data[3];
    }
    virtual T& operator[](int index) {
        return data[index];
    }
    virtual T operator[](int index) const {
        return data[index];
    }
    basemat3& operator = (const basemat3& m) {
        memcpy(data, m.data, 9 * sizeof(T));
    }
    operator T* () {
        return data;
    }
    basemat3& scale(T sc) {
        for (int i = 0; i < 9; ++i)
            data[i] *= sc;
        return *this;
    }
    basemat3& operator+=(basemat3 m) {
        for (int i = 0; i < 9; ++i)
            data[i] += m.data[i];
        return *this;
    }
    basemat3& operator-=(basemat3 m) {
        for (int i = 0; i < 9; ++i)
            data[i] -= m.data[i];
        return *this;
    }
    basemat3& operator*=(T s) {
        for (int i = 0; i < 9; ++i)
            data[i] *= s;
        return *this;
    }
    basemat3& operator/=(T s) {
        for (int i = 0; i < 9; ++i)
            data[i] /= s;
        return *this;
    }
    void transpose() {
        basemat3 t(data[0], data[3], data[6], data[1], data[4], data[7], data[2], data[5], data[8]);
        memcpy(data, t.data, 9 * sizeof(T));
    }
    basemat3& invert() {
        T mInverse[9];
        T d = det();
        if (d == 0) d = 1;
        d = 1.0f / d;
        // calculate inverse
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                T detij = DetIJ(j, i);
                mInverse[(i * 3) + j] = ((i + j) & 0x1) ? (-detij * d) : (detij * d);
            }
        }
        memcpy(data, mInverse, 9 * sizeof(T));
        return *this;
    }
};

template <typename T>
inline basemat3<T> operator-(const basemat3<T>& m1, const basemat3<T>& m2)
{
    return	basemat3<T>(m1.data[0] - m2.data[0], m1.data[3] - m2.data[3], m1.data[6] - m2.data[6],
        m1.data[1] - m2.data[1], m1.data[4] - m2.data[4], m1.data[7] - m2.data[7],
        m1.data[2] - m2.data[2], m1.data[5] - m2.data[5], m1.data[8] - m2.data[8]);
}
template <typename T>
inline basemat3<T> operator+(basemat3<T> m1, basemat3<T> m2)
{
    return	basemat3<T>(m1.data[0] + m2.data[0], m1.data[3] + m2.data[3], m1.data[6] + m2.data[6],
        m1.data[1] + m2.data[1], m1.data[4] + m2.data[4], m1.data[7] + m2.data[7],
        m1.data[2] + m2.data[2], m1.data[5] + m2.data[5], m1.data[8] + m2.data[8]);
}
template <typename T>
inline basemat3<T> operator*(basemat3<T> m1, basemat3<T> m2)
{
    return basemat3<T>(
        m1.data[0] * m2.data[0] + m1.data[3] * m2.data[1] + m1.data[6] * m2.data[2],
        m1.data[1] * m2.data[0] + m1.data[4] * m2.data[1] + m1.data[7] * m2.data[2],
        m1.data[2] * m2.data[0] + m1.data[5] * m2.data[1] + m1.data[8] * m2.data[2],

        m1.data[0] * m2.data[3] + m1.data[3] * m2.data[4] + m1.data[6] * m2.data[5],
        m1.data[1] * m2.data[3] + m1.data[4] * m2.data[4] + m1.data[7] * m2.data[5],
        m1.data[2] * m2.data[3] + m1.data[5] * m2.data[4] + m1.data[8] * m2.data[5],

        m1.data[0] * m2.data[6] + m1.data[3] * m2.data[7] + m1.data[6] * m2.data[8],
        m1.data[1] * m2.data[6] + m1.data[4] * m2.data[7] + m1.data[7] * m2.data[8],
        m1.data[2] * m2.data[6] + m1.data[5] * m2.data[7] + m1.data[8] * m2.data[8]);
}

template <typename T>
inline basevec3<T> operator*(basemat3<T> m, basevec3<T> u)
{
    return basevec3<T>(m.data[0] * u.x + m.data[3] * u.y + m.data[6] * u.z,
        m.data[1] * u.x + m.data[4] * u.y + m.data[7] * u.z,
        m.data[2] * u.x + m.data[5] * u.y + m.data[8] * u.z);
}

template <typename T>
inline basevec3<T> operator*(basevec3<T> u, basemat3<T> m)
{
    return basevec3<T>(u.x * m.data[0] + u.y * m.data[1] + u.z * m.data[2],
        u.x * m.data[3] + u.y * m.data[4] + u.z * m.data[5],
        u.x * m.data[6] + u.y * m.data[7] + u.z * m.data[8]);
}

template <typename T, typename Q>
inline basemat3<T> operator/(basemat3<T> m, Q s)
{
    return	basemat3<T>(m.data[0] / s, m.data[3] / s, m.data[6] / s,
        m.data[1] / s, m.data[4] / s, m.data[7] / s,
        m.data[2] / s, m.data[5] / s, m.data[8] / s);
}
template <typename T, typename Q>
inline basemat3<T> operator*(basemat3<T> m, Q s)
{
    return	basemat3<T>(m.data[0] * s, m.data[3] * s, m.data[6] * s,
        m.data[1] * s, m.data[4] * s, m.data[7] * s,
        m.data[2] * s, m.data[5] * s, m.data[8] * s);
}
template <typename T, typename Q>
inline basemat3<T> operator*(Q s, basemat3<T> m)
{
    return	basemat3<T>(m.data[0] * s, m.data[3] * s, m.data[6] * s,
        m.data[1] * s, m.data[4] * s, m.data[7] * s,
        m.data[2] * s, m.data[5] * s, m.data[8] * s);
}

///////////////////////////////////////////////////////////////////////////////////
/*  matrix layout must be the same as in OpenGL
0  4  8 12
1  5  9 13
2  6 10 14
3  7 11 15
*/
template <typename T>
class basemat4 {
    T data[16];
    T DetIJ(int i, int j) {
        int x, y, ii, jj;
        T ret, mat[3][3];
        x = 0;
        for (ii = 0; ii < 4; ii++)
        {
            if (ii == i) continue;
            y = 0;
            for (jj = 0; jj < 4; jj++)
            {
                if (jj == j) continue;
                mat[x][y] = data[(ii * 4) + jj];
                y++;
            }
            x++;
        }
        ret = mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]);
        ret -= mat[0][1] * (mat[1][0] * mat[2][2] - mat[2][0] * mat[1][2]);
        ret += mat[0][2] * (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]);
        return ret;
    }
public:
    basemat4() {
        memset(data, 0, sizeof(T) * 16);
    }
    basemat4(const basemat4& m1) {
        memcpy(data, m1.data, 16 * sizeof(T));
    }
    basemat4(T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7,
        T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15) {
        data[0] = d0; data[4] = d4; data[8] = d8;   data[12] = d12;
        data[1] = d1; data[5] = d5; data[9] = d9;   data[13] = d13;
        data[2] = d2; data[6] = d6; data[10] = d10; data[14] = d14;
        data[3] = d3; data[7] = d7; data[11] = d11; data[15] = d15;
    }
    virtual ~basemat4() {}
    T& operator[](int index) {
        return data[index];
    }
    T operator[](int index) const {
        return data[index];
    }
    basemat4& operator = (const basemat4& m1) {
        memcpy(data, m1.data, 16 * sizeof(T));
        return *this;
    }
    operator T* () {
        return data;
    }
    basemat4& scale(T sc) {
        for (int i = 0; i < 16; ++i)
            data[i] *= sc;
        return *this;
    }
    basemat4 operator + (basemat4& m1) {
        basemat4 r;
        for (int i = 0; i < 16; ++i)
            r[i] = data[i] + m1.data[i];
        return r;
    }
    basemat4 operator - (basemat4& m1) {
        basemat4 r;
        for (int i = 0; i < 16; ++i)
            r[i] = data[i] - m1.data[i];
        return r;
    }
    basemat4 operator *(T sc) {
        basemat4 r;
        for (int i = 0; i < 16; ++i)
            r[i] = data[i] * sc;
        return r;
    }
    basemat4 operator /(T sc) {
        basemat4 r;
        for (int i = 0; i < 16; ++i)
            r[i] = data[i] / sc;
        return r;
    }
    basevec4<T> operator *(basevec4<T>& v) {
        basevec4<T> mProduct;
        mProduct[0] = data[0] * v[0] + data[4] * v[1] + data[8] * v[2] + data[12] * v[3];
        mProduct[1] = data[1] * v[0] + data[5] * v[1] + data[9] * v[2] + data[13] * v[3];
        mProduct[2] = data[2] * v[0] + data[6] * v[1] + data[10] * v[2] + data[14] * v[3];
        mProduct[3] = data[3] * v[0] + data[7] * v[1] + data[11] * v[2] + data[15] * v[3];
        return mProduct;
    }
    void loadIdentity() {
        static T identity[] = { T(1), T(0), T(0), T(0),
                                T(0), T(1), T(0), T(0),
                                T(0), T(0), T(1), T(0),
                                T(0), T(0), T(0), T(1) };
        memcpy(data, identity, 16 * sizeof(T));
    }
    void transpose() {
        T temp;
        temp = data[1];	data[1] = data[4];	data[4] = temp;
        temp = data[2];	data[2] = data[8];	data[8] = temp;
        temp = data[3];	data[3] = data[12];	data[12] = temp;
        temp = data[6];	data[6] = data[9];	data[9] = temp;
        temp = data[7];	data[7] = data[13];	data[13] = temp;
        temp = data[11];	data[11] = data[14];	data[14] = temp;
    }
    basemat4& invert() {
        int i, j;
        T det, detij;
        T mInverse[16];

        // calculate 4x4 determinant
        det = 0.0f;
        for (i = 0; i < 4; i++)
        {
            det += (i & 0x1) ? (-data[i] * DetIJ(0, i)) : (data[i] * DetIJ(0, i));
        }
        det = 1.0f / det;

        // calculate inverse
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                detij = DetIJ(j, i);
                mInverse[(i * 4) + j] = ((i + j) & 0x1) ? (-detij * det) : (detij * det);
            }
        }
        memcpy(data, mInverse, 16 * sizeof(T));

        return *this;
    }
};

template <typename T>
inline basemat4<T> operator *(const basemat4<T>& m1, const basemat4<T>& m2) {
    basemat4<T> mProduct;

    mProduct[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    mProduct[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    mProduct[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    mProduct[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];

    mProduct[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    mProduct[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    mProduct[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    mProduct[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];

    mProduct[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    mProduct[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    mProduct[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    mProduct[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];

    mProduct[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
    mProduct[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
    mProduct[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
    mProduct[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

    return mProduct;
}

template <typename T>
basemat3<T> identity3() {
    static basemat3<T> identity[] = { T(1), T(0), T(0),
                                      T(0), T(1), T(0),
                                      T(0), T(0), T(1) };
    return identity;
}
template <typename T>
basemat4<T> identity4() {
    static basemat4<T> identity(T(1), T(0), T(0), T(0),
        T(0), T(1), T(0), T(0),
        T(0), T(0), T(1), T(0),
        T(0), T(0), T(0), T(1));
    return identity;
}

#endif // __cg_basematrix_h__