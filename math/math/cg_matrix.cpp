#include "pch.h"
#include "cg_config.h"
#include "cg_math.h"
#include "cg_matrix.h"

namespace atlas {
    //////////////////////////////////////////////////////////////////////////
    // translation
    mat4 translation_matrix(float x, float y, float z) {
        mat4 m;
        m.loadIdentity();
        m[12] = x;
        m[13] = y;
        m[14] = z;
        return m;
    }
    void translate_matrix(mat4& matrix, const vec3& v)
    {
        matrix[12] = matrix[0] * v.x + matrix[4] * v.y + matrix[8] * v.z + matrix[12];
        matrix[13] = matrix[1] * v.x + matrix[5] * v.y + matrix[9] * v.z + matrix[13];
        matrix[14] = matrix[2] * v.x + matrix[6] * v.y + matrix[10] * v.z + matrix[14];
        matrix[15] = matrix[3] * v.x + matrix[7] * v.y + matrix[11] * v.z + matrix[15];
    }
    void remove_translation(mat4& m) {
        m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = m[15] = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // scaling
    mat4 scaling_matrix(float x, float y, float z) {
        mat4 m;
        m.loadIdentity();
        m[0] = x;
        m[5] = y;
        m[10] = z;
        return m;
    }

    //////////////////////////////////////////////////////////////////////////
    // rotation
    // angle in radians
    mat4 rotation_matrix(float angle, float x, float y, float z) {
        float vecLength, sinSave, cosSave, oneMinusCos;
        float xx, yy, zz, xy, yz, zx, xs, ys, zs;
        mat4 ret;

        if (x == 0.0f && y == 0.0f && z == 0.0f)
        {
            ret.loadIdentity();
            return ret;
        }

        // Scale vector
        vecLength = (float)sqrt(x * x + y * y + z * z);

        // Rotation matrix is normalized
        x /= vecLength;
        y /= vecLength;
        z /= vecLength;

        sinSave = (float)sin(angle);
        cosSave = (float)cos(angle);
        oneMinusCos = 1.0f - cosSave;

        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * sinSave;
        ys = y * sinSave;
        zs = z * sinSave;

        ret[0] = (oneMinusCos * xx) + cosSave;
        ret[4] = (oneMinusCos * xy) - zs;
        ret[8] = (oneMinusCos * zx) + ys;
        ret[12] = 0.0f;

        ret[1] = (oneMinusCos * xy) + zs;
        ret[5] = (oneMinusCos * yy) + cosSave;
        ret[9] = (oneMinusCos * yz) - xs;
        ret[13] = 0.0f;

        ret[2] = (oneMinusCos * zx) - ys;
        ret[6] = (oneMinusCos * yz) + xs;
        ret[10] = (oneMinusCos * zz) + cosSave;
        ret[14] = 0.0f;

        ret[3] = 0.0f;
        ret[7] = 0.0f;
        ret[11] = 0.0f;
        ret[15] = 1.0f;
        return ret;
    }
#if 0
    // angle in degrees
    mat4 rotation_matrix(float angle, float x, float y, float z) {
        float q = float(angle) * pid180;
        return rotation_matrixR(q, x, y, z);
    }
#endif
    // rotation matrix from u2 to u1
    mat4 rotate_align(vec3 u1, vec3 u2) {
        vec3 axis = cross(u1, u2);
        float cosA = dot(u1, u2);
        float k = 1.0f / (1.0f + cosA);
        mat4 result(
            (axis.x * axis.x * k) + cosA, (axis.y * axis.x * k) - axis.z, (axis.z * axis.x * k) + axis.y, 0,
            (axis.x * axis.y * k) + axis.z, (axis.y * axis.y * k) + cosA, (axis.z * axis.y * k) - axis.x, 0,
            (axis.x * axis.z * k) - axis.y, (axis.y * axis.z * k) + axis.x, (axis.z * axis.z * k) + cosA, 0,
            0, 0, 0, 1
        );
        return result;
    }
    // apply rotation matrix on vector
    vec3 rotate_vector(const mat4& mMatrix, const vec3& vec)
    {
        vec3 vOut;
        vOut[0] = mMatrix[0] * vec.x + mMatrix[4] * vec.y + mMatrix[8] * vec.z;
        vOut[1] = mMatrix[1] * vec.x + mMatrix[5] * vec.y + mMatrix[9] * vec.z;
        vOut[2] = mMatrix[2] * vec.x + mMatrix[6] * vec.y + mMatrix[10] * vec.z;

        return vOut;
    }

    //////////////////////////////////////////////////////////////////////////
    // view matrices
    // view matrix WITH camera (eye) positioning
    mat4 lookAt(const vec3& eyePosition3D, const vec3& center3D, const vec3& upVector3D) {
        vec3 forward(center3D - eyePosition3D);
        forward.normalize();
        // --------------------
        // right = forward x up
        vec3 right = cross(forward, upVector3D);
        right.normalize();
        // --------------------
        // Recompute up as: up = right x forward
        vec3 up = cross(right, forward);
        up.normalize();
        // --------------------
        mat4 result;
        result[0] = right[0];
        result[4] = right[1];
        result[8] = right[2];
        result[12] = -dot(right, eyePosition3D); // 0.0;
        // --------------------
        result[1] = up[0];
        result[5] = up[1];
        result[9] = up[2];
        result[13] = -dot(up, eyePosition3D); // 0.0;
        // --------------------
        result[2] = -forward[0];
        result[6] = -forward[1];
        result[10] = -forward[2];
        result[14] = dot(forward, eyePosition3D); // 0.0;
        // --------------------
        result[3] = result[7] = result[11] = 0.0;
        result[15] = 1.0;
        // --------------------
        return result;
    }
    // viewport (frustum) matrix
    void frustum_matrix(float* matrix, float left, float right, float bottom, float top, float znear, float zfar)
    {
        float n2, rml, tmb, fmn;
        n2 = 2.0f * znear;
        rml = right - left;
        tmb = top - bottom;
        fmn = zfar - znear;

        memset(matrix, 0, 16 * sizeof(float));
#if 0
        matrix[0] = n2 / rml;
        matrix[5] = n2 / tmb;
        matrix[8] = (right + left) / rml;
        matrix[9] = (top + bottom) / tmb;
        matrix[10] = -(zfar + znear) / fmn;
        matrix[11] = -1.0;
        matrix[14] = -(n2 * zfar) / fmn;
#else
        matrix[0] = n2 / rml;
        matrix[5] = n2 / tmb;
        matrix[8] = (right + left) / rml;
        matrix[9] = (top + bottom) / tmb;
        matrix[10] = -zfar / fmn;
        matrix[11] = -1.0;
        matrix[14] = -(znear * zfar) / fmn;
#endif
    }

    // orthographic projection matrix
    void ortho(float* matrix, float l, float r, float b, float t, float n, float f)
    {
        // set OpenGL orthographic projection matrix
        matrix[0] = 2 / (r - l);
        matrix[1] = 0;
        matrix[2] = 0;
        matrix[3] = 0;

        matrix[4] = 0;
        matrix[5] = 2 / (t - b);
        matrix[6] = 0;
        matrix[7] = 0;

        matrix[8] = 0;
        matrix[9] = 0;
        matrix[10] = -2 / (f - n);
        matrix[11] = 0;

        matrix[12] = -(r + l) / (r - l);
        matrix[13] = -(t + b) / (t - b);
        matrix[14] = -(f + n) / (f - n);
        matrix[15] = 1;
    }
    mat4 ortho(float l, float r, float b, float t, float n, float f)
    {
        mat4 o;
        ortho(o, l, r, b, t, n, f);
        return o;
    }
    void ortho_matrix(float* matrix, float fovyInRadians, float aspectRatio, float znear, float zfar)
    {
        float ymax, xmax;
        // half of the angle!!
        ymax = zfar * (float)tan(fovyInRadians / 2.f);
        xmax = ymax * aspectRatio;
        ortho(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
    }
    mat4 ortho_matrix(float fovyInRadians, float aspectRatio, float znear, float zfar)
    {
        mat4 ret;
        ortho_matrix((float*)ret, fovyInRadians, aspectRatio, znear, zfar);
        return ret;
    }

    // perspective projection matrix
    void perspective_matrix(float* matrix, float fovyInRadians, float aspectRatio, float znear, float zfar)
    {
        float ymax, xmax;
        // half of the angle!!
        ymax = znear * (float)tan(fovyInRadians / 2.f);
        xmax = ymax * aspectRatio;
        frustum_matrix(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
    }
    mat4 perspective_matrix(float fovyInRadians, float aspectRatio, float znear, float zfar)
    {
        mat4 ret;
        perspective_matrix((float*)ret, fovyInRadians, aspectRatio, znear, zfar);
        return ret;
    }
};