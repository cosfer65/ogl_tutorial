#include "common.h"
#include "pmath.h"

mat4 translation_matrix(float x, float y, float z) {
    mat4 m;
    m.loadIdentity();
    m[3] = x;
    m[7] = y;
    m[11] = z;
    return m;
}

// viewport (frustum) matrix
static void frustum_matrix(float* matrix, float left, float right, float bottom, float top, float znear, float zfar)
{
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