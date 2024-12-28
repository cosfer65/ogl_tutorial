#include "amath.h"
#include "matrix.h"

namespace atlas {
    //////////////////////////////////////////////////////////////////////////
    // translation (the last column)
    mat4 translation_matrix(float x, float y, float z) {
        mat4 m;
        m.loadIdentity();
        m[3] = x;
        m[7] = y;
        m[11] = z;
        return m;
    }
    void translate_matrix(mat4& matrix, const vec3& v) {
        matrix[3] = matrix[0] * v.x + matrix[1] * v.y + matrix[2] * v.z + matrix[3];
        matrix[7] = matrix[4] * v.x + matrix[5] * v.y + matrix[6] * v.z + matrix[7];
        matrix[11] = matrix[8] * v.x + matrix[9] * v.y + matrix[10] * v.z + matrix[11];
        matrix[15] = matrix[12] * v.x + matrix[13] * v.y + matrix[14] * v.z + matrix[15];
    }
    void remove_translation(mat4& m) {
        m[12] = m[13] = m[14] = m[3] = m[7] = m[11] = m[15] = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // scaling (the diagonal)
    mat4 scaling_matrix(float x, float y, float z) {
        mat4 m;
        m.loadIdentity();
        m[0] = x;
        m[5] = y;
        m[10] = z;
        return m;
    }

    //////////////////////////////////////////////////////////////////////////
    // rotation angle in radians
    mat4 rotation_matrix(float angle, float x, float y, float z) {
        float vecLength, sinSave, cosSave, oneMinusCos;
        float xx, yy, zz, xy, yz, zx, xs, ys, zs;
        mat4 ret;

        if (x == 0.0f && y == 0.0f && z == 0.0f) {
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
        ret[1] = (oneMinusCos * xy) - zs;
        ret[2] = (oneMinusCos * zx) + ys;
        ret[3] = 0.0f;

        ret[4] = (oneMinusCos * xy) + zs;
        ret[5] = (oneMinusCos * yy) + cosSave;
        ret[6] = (oneMinusCos * yz) - xs;
        ret[7] = 0.0f;

        ret[8] = (oneMinusCos * zx) - ys;
        ret[9] = (oneMinusCos * yz) + xs;
        ret[10] = (oneMinusCos * zz) + cosSave;
        ret[11] = 0.0f;

        ret[12] = 0.0f;
        ret[13] = 0.0f;
        ret[14] = 0.0f;
        ret[15] = 1.0f;
        return ret;
    }

    // apply rotation matrix on vector
    vec3 rotate_vector(const mat4& mMatrix, const vec3& vec)
    {
        mat3 m = to_mat3(mMatrix);
        vec3 vOut = m * vec;
        return vOut;
    }
};