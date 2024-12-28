#ifndef __gl_math_h__
#define __gl_math_h__

#include "vector.h"
#include "matrix.h"

namespace atlas {
    //////////////////////////////////////////////////////////////////////////
    // view matrices, column major as OpenGL expects them
    // view matrix WITH camera (eye) positioning
    inline mat4 lookAt(const vec3& eyePosition3D, const vec3& center3D, const vec3& upVector3D) {
        vec3 forward(center3D - eyePosition3D);
        forward.normalize();
        // --------------------
        // right = forward x up                                                        8
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
    inline void frustum_matrix(float* matrix, float left, float right, float bottom, float top, float znear, float zfar) {
        float n2, rml, tmb, fmn;
        n2 = 2.0f * znear;
        rml = right - left;
        tmb = top - bottom;
        fmn = zfar - znear;

        memset(matrix, 0, 16 * sizeof(float));
        matrix[0] = n2 / rml;
        matrix[5] = n2 / tmb;
        matrix[8] = (right + left) / rml;
        matrix[9] = (top + bottom) / tmb;
        matrix[10] = -zfar / fmn;
        matrix[11] = -1.0;
        matrix[14] = -(znear * zfar) / fmn;
    }
    // perspective projection matrix
    inline void perspective_matrix(float* matrix, float fovyInRadians, float aspectRatio, float znear, float zfar) {
        float ymax, xmax;
        // half of the angle!!
        ymax = znear * (float)tan(fovyInRadians / 2.f);
        xmax = ymax * aspectRatio;
        frustum_matrix(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
    }
    inline mat4 perspective_matrix(float fovyInRadians, float aspectRatio, float znear, float zfar) {
        mat4 ret;
        perspective_matrix((float*)ret, fovyInRadians, aspectRatio, znear, zfar);
        return ret;
    }
    // orthographic projection matrix
    inline void ortho(float* matrix, float l, float r, float b, float t, float n, float f)
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
    inline mat4 ortho(float l, float r, float b, float t, float n, float f)
    {
        mat4 o;
        ortho(o, l, r, b, t, n, f);
        return o;
    }
}  // namespace atlas

#endif // __gl_math_h__
