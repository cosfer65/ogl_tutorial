#pragma once

float const pi = 3.1415926535897932384626433832795f;
float const tol = 0.00001f;		// float type tolerance
float const pi2 = 6.283185307179586476925286766559f;
float const pid180 = 0.017453292519943295769236907684886f;
float const pid180inv = 57.295779513082320876798154814105f;

inline float deg_to_rad(float deg) {
    return deg * pid180;
}
#define dtr deg_to_rad

inline float rad_to_deg(float rad) {
    return rad * pid180inv;
}
#define rtd rad_to_deg

inline int dti(double d) {   // approximate float to integer
    return (int)floor(d + 0.5);
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

///////////////////////////////////////////////////////////////////////////////////
// matrices are row-major

template <typename T>
inline T det3(T data[9]) {
    return    data[0] * data[4] * data[8] + data[1] * data[5] * data[6] + data[2] * data[3] * data[7]
        - data[2] * data[4] * data[6] - data[0] * data[5] * data[7] - data[1] * data[3] * data[8];
}

template <typename T>
inline T DetIJ3(T data[9], int r, int c) {
    int rr, cc;
    T ret, mat[4];
    int w = 0;
    for (rr = 0; rr < 3; ++rr) {
        if (rr == r) continue; // skip row
        for (cc = 0; cc < 3; cc++) {
            if (cc == c) continue;  // skip column
            mat[w] = data[(rr * 3) + cc];
            ++w;
        }
    }
    ret = mat[0] * mat[3] - mat[1] * mat[2];
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
/*  matrix layout is row major
0  1 2 3
4  5 6 7
8  9 10 11
12 13 14 15
*/

template <typename T>
inline T DetIJ4(T data[16], int r, int c) {
    T ret, mat[9];
    int w = 0;
    for (int rr = 0; rr < 4; rr++) {
        if (rr == r) continue;
        for (int cc = 0; cc < 4; cc++) {
            if (cc == c) continue;
            mat[w] = data[rr * 4 + cc];
            w++;
        }
    }
    ret = det3(mat);
    return ret;
}

template <typename T>
inline T det4(T data[16]) {
    T det = 0;
    for (int i = 0; i < 4; i++)
    {
        T v = data[i] * DetIJ4(data, 0, i);
        det += (i & 0x1) ? -v : v;
    }
    return det;
}

template <typename T>
class basemat4 {
public:
    T data[16];
    basemat4() {
        memset(data, 0, sizeof(T) * 16);
    }
    basemat4(const basemat4& m1) {
        memcpy(data, m1.data, 16 * sizeof(T));
    }
    basemat4(T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7,
        T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15) {
        data[0] = d0; data[1] = d1; data[2] = d2; data[3] = d3;
        data[4] = d4; data[5] = d5; data[6] = d6; data[7] = d7;
        data[8] = d8; data[9] = d9; data[10] = d10; data[11] = d11;
        data[12] = d12; data[13] = d13; data[14] = d14; data[15] = d15;
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
    void loadIdentity() {
        static T identity[] = { T(1), T(0), T(0), T(0),
                                T(0), T(1), T(0), T(0),
                                T(0), T(0), T(1), T(0),
                                T(0), T(0), T(0), T(1) };
        memcpy(data, identity, 16 * sizeof(T));
    }
    basemat4& operator+=(basemat4 m) {
        for (int i = 0; i < 16; ++i)
            data[i] += m.data[i];
        return *this;
    }
    basemat4& operator-=(basemat4 m) {
        for (int i = 0; i < 16; ++i)
            data[i] -= m.data[i];
        return *this;
    }
    basemat4& operator*=(T s) {
        for (int i = 0; i < 16; ++i)
            data[i] *= s;
        return *this;
    }
    basemat4& operator/=(T s) {
        for (int i = 0; i < 16; ++i)
            data[i] /= s;
        return *this;
    }
    void transpose() {
        T temp[16];
        temp[0] = data[0]; temp[1] = data[4]; temp[2] = data[8]; temp[3] = data[12];
        temp[4] = data[1]; temp[5] = data[5]; temp[6] = data[9]; temp[7] = data[13];
        temp[8] = data[2]; temp[9] = data[6]; temp[10] = data[10]; temp[11] = data[14];
        temp[12] = data[3]; temp[13] = data[7]; temp[14] = data[11]; temp[15] = data[15];
        memcpy(data, temp, 16 * sizeof(T));
    }
    basemat4& inverse() {
        int i, j;
        T det, detij;
        T mInverse[16];

        // calculate 4x4 determinant
        det = det4(data);
        det = 1.0f / det;

        // calculate inverse
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                detij = DetIJ4(data, j, i);
                mInverse[(i * 4) + j] = ((i + j) & 0x1) ? (-detij * det) : (detij * det);
            }
        }
        memcpy(data, mInverse, 16 * sizeof(T));

        return *this;
    }
};

template <typename T>
inline basemat4<T> operator + (const basemat4<T>& m1, const basemat4<T>& m2) {
    basemat4<T> r;
    for (int i = 0; i < 16; ++i)
        r[i] = m1[i] + m2[i];
    return r;
}

template <typename T>
inline basemat4<T> operator - (const basemat4<T>& m1, const basemat4<T>& m2) {
    basemat4<T> r;
    for (int i = 0; i < 16; ++i)
        r[i] = m1[i] - m2[i];
    return r;
}

template <typename T, typename Q>
inline basemat4<T> operator * (const basemat4<T>& m1, Q sc) {
    basemat4<T> r;
    for (int i = 0; i < 16; ++i)
        r[i] = m1[i] * sc;
    return r;
}
template <typename T, typename Q>
inline basemat4<T> operator * (Q sc, const basemat4<T>& m1) {
    basemat4<T> r;
    for (int i = 0; i < 16; ++i)
        r[i] = m1[i] * sc;
    return r;
}

template <typename T, typename Q>
inline basemat4<T> operator / (const basemat4<T>& m1, Q sc) {
    basemat4<T> r;
    for (int i = 0; i < 16; ++i)
        r[i] = m1[i] / sc;
    return r;
}

template <typename T>
inline basemat4<T> operator *(const basemat4<T>& m2, const basemat4<T>& m1) {
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
inline basemat4<T> identity4() {
    static basemat4<T> identity(T(1), T(0), T(0), T(0),
        T(0), T(1), T(0), T(0),
        T(0), T(0), T(1), T(0),
        T(0), T(0), T(0), T(1));
    return identity;
}

typedef basemat4<float> mat4;

mat4 translation_matrix(float x, float y, float z);
void perspective_matrix(float* matrix, float fovyInRadians, float aspectRatio, float znear, float zfar);
mat4 perspective_matrix(float fovyInRadians, float aspectRatio, float znear, float zfar);