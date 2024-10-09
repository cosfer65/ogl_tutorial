#include "common.h"
#include "math.h"

mat4 translation_matrix(float x, float y, float z) {
    mat4 m;
    m.loadIdentity();
    m[12] = x;
    m[13] = y;
    m[14] = z;
    return m;
}

// viewport (frustum) matrix
static void frustum_matrix(float* matrix, float left, float right, float bottom, float top, float znear, float zfar)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;

    matrix[0] = temp / temp2;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;

    matrix[4] = 0.0;
    matrix[5] = temp / temp3;
    matrix[6] = 0.0;
    matrix[7] = 0.0;

    matrix[8] = (right + left) / temp2;
    matrix[9] = (top + bottom) / temp3;
    matrix[10] = (-zfar - znear) / temp4;
    matrix[11] = -1.0;

    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = (-temp * zfar) / temp4;
    matrix[15] = 0.0;
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
    float ymax, xmax;
    // half of the angle!!
    ymax = znear * (float)tan(fovyInRadians / 2.f);
    xmax = ymax * aspectRatio;
    frustum_matrix(ret, -xmax, xmax, -ymax, ymax, znear, zfar);
    return ret;
}