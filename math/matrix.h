#ifndef __matrix_h__
#define __matrix_h__

#include "vector.h"
#include "basematrix.h"

namespace atlas {
    typedef basemat3<float> mat3;
    typedef basemat4<float> mat4;

    //////////////////////////////////////////////////////////////////////////
    // translation
    mat4 translation_matrix(float x, float y, float z);
    void translate_matrix(mat4& matrix, const vec3& v);
    void remove_translation(mat4& m);
    //////////////////////////////////////////////////////////////////////////
    // scaling
    mat4 scaling_matrix(float x, float y, float z);
    //////////////////////////////////////////////////////////////////////////
    // rotation
    // angle in radians
    mat4 rotation_matrix(float angle, float x, float y, float z);
    // apply rotation matrix on vector
    vec3 rotate_vector(const mat4& mMatrix, const vec3& vec);
    //////////////////////////////////////////////////////////////////////////

    // convert row-major matrix to col-major for OpenGL
    inline void getGLTransform(mat4& gl_matrix, const mat4& transformMatrix) {
        gl_matrix[0] = transformMatrix[0];
        gl_matrix[1] = transformMatrix[4];
        gl_matrix[2] = transformMatrix[8];
        gl_matrix[3] = 0;

        gl_matrix[4] = transformMatrix[1];
        gl_matrix[5] = transformMatrix[5];
        gl_matrix[6] = transformMatrix[9];
        gl_matrix[7] = 0;

        gl_matrix[8] = transformMatrix[2];
        gl_matrix[9] = transformMatrix[6];
        gl_matrix[10] = transformMatrix[10];
        gl_matrix[11] = 0;

        gl_matrix[12] = transformMatrix[3];
        gl_matrix[13] = transformMatrix[7];
        gl_matrix[14] = transformMatrix[11];
        gl_matrix[15] = 1;
    }
};

#endif // __matrix_h__
