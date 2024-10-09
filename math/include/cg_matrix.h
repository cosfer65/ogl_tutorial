#ifndef __cg_matrix_h__
#define __cg_matrix_h__

#include "cg_vector.h"
#include "cg_basematrix.h"

namespace atlas {
    typedef basemat3<float> mat3;
    typedef basemat4<float> mat4;

    //////////////////////////////////////////////////////////////////////////
    // translation
    mat4 translation_matrix(float x, float y, float z);
    void translate_matrix(mat4& matrix, const vec3& v);
    //////////////////////////////////////////////////////////////////////////
    // scaling
    mat4 scaling_matrix(float x, float y, float z);
    //////////////////////////////////////////////////////////////////////////
    // rotation
    // angle in radians
    mat4 rotation_matrixR(float angle, float x, float y, float z);
    // angle in degrees
    mat4 rotation_matrix(float angle, float x, float y, float z);
    // rotation matrix from u2 to u1
    mat4 rotate_align(vec3 u1, vec3 u2);
    // apply rotation matrix on vector
    vec3 rotate_vector(const mat4& mMatrix, const vec3& vec);
    //////////////////////////////////////////////////////////////////////////
// view matrices
// view matrix WITH camera (eye) positioning
    mat4 lookAt(const vec3& eyePosition3D, const vec3& center3D, const vec3& upVector3D);
    // viewport (frustum) matrix
    void frustum_matrix(float* matrix, float left, float right, float bottom, float top, float znear, float zfar);
    // orthographic projection matrix
    void ortho(float* matrix, float l, float r, float b, float t, float n, float f);
    mat4 ortho(float l, float r, float b, float t, float n, float f);
    void ortho_matrix(float* matrix, float fovyInRadians, float aspectRatio, float znear, float zfar);
    mat4 ortho_matrix(float fovyInRadians, float aspectRatio, float znear, float zfar);
    // perspective projection matrix
    void perspective_matrix(float* matrix, float fovyInRadians, float aspectRatio, float znear, float zfar);
    mat4 perspective_matrix(float fovyInRadians, float aspectRatio, float znear, float zfar);

    void remove_translation(mat4& m);
};

#endif // __cg_matrix_h__
