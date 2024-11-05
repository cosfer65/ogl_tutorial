#ifndef __cg_quaternion_h__
#define __cg_quaternion_h__

#include "cg_math.h"
#include "cg_matrix.h"

namespace atlas {
    class quaternion : public basevec4<float> {
    public:
        quaternion() : basevec4<float>(0) {
        }
        quaternion(float scalar, float i, float j, float k) : basevec4<float>(i, j, k, scalar) {
        }
        float magnitude(void) {
            return length();
        }

        inline vec3 get_vector(void) {
            return vec3(x, y, z);
        }

        inline float get_scalar(void) {
            return w;
        }

        quaternion operator~(void) const {
            return quaternion(w, -x, -y, -z);
        }

        mat4 matrix() {
            mat4 R;
            R[0] = 1 - 2 * y * y - 2 * z * z;
            R[1] = 2 * x * y - 2 * w * z;
            R[2] = 2 * x * z + 2 * w * y;
            R[3] = 0;

            R[4] = 2 * x * y + 2 * w * z;
            R[5] = 1 - 2 * x * x - 2 * z * z;
            R[6] = 2 * y * z - 2 * w * x;
            R[7] = 0;

            R[8] = 2 * x * z - 2 * w * y;
            R[9] = 2 * y * z + 2 * w * x;
            R[10] = 1 - 2 * x * x - 2 * y * y;
            R[11] = 0;

            R[12] = R[13] = R[14] = 0;
            R[15] = 1;
            return R;
        }
    };

    inline quaternion operator + (quaternion q1, quaternion q2) {
        return quaternion(q1.w + q2.w,
            q1.x + q2.x,
            q1.y + q2.y,
            q1.z + q2.z);
    }

    inline quaternion operator - (quaternion q1, quaternion q2) {
        return quaternion(q1.w - q2.w,
            q1.x - q2.x,
            q1.y - q2.y,
            q1.z - q2.z);
    }

    inline quaternion operator * (quaternion q, float s) {
        return quaternion(q.w * s, q.x * s, q.y * s, q.z * s);
    }

    inline quaternion operator * (float s, quaternion q) {
        return quaternion(q.w * s, q.x * s, q.y * s, q.z * s);
    }

    inline quaternion operator * (vec3 v, quaternion q) {
        return quaternion(-(q.x * v.x + q.y * v.y + q.z * v.z),
            q.w * v.x + q.z * v.y - q.y * v.z,
            q.w * v.y + q.x * v.z - q.z * v.x,
            q.w * v.z + q.y * v.x - q.x * v.y);
    }

    inline quaternion operator / (quaternion q, float s) {
        return quaternion(q.w / s, q.x / s, q.y / s, q.z / s);
    }

    inline quaternion operator * (quaternion q1, quaternion q2) {
        return quaternion(q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
            q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
            q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
    }
    inline quaternion operator * (quaternion q, vec3 v) {
        return	quaternion(-(q.x * v.x + q.y * v.y + q.z * v.z),
            q.w * v.x + q.y * v.z - q.z * v.y,
            q.w * v.y + q.z * v.x - q.x * v.z,
            q.w * v.z + q.x * v.y - q.y * v.x);
    }

    float q_get_angle(quaternion q);
    vec3 q_get_axis(quaternion q);
    quaternion q_rotate(quaternion q1, quaternion q2);
    vec3 qv_rotate_inv(quaternion q, vec3 v);
    vec3 qv_rotate(quaternion q, vec3 v);
    quaternion make_q_from_euler_angles(float x, float y, float z);
    vec3 make_euler_angles_from_q(quaternion q);

    inline void rotate_vector(vec3& v, const vec3& rotation_axis, float angle) {
        // build the rotation quaternion
        quaternion q;
        q.w = (float)cos(angle / 2);
        q.x = (float)sin(angle / 2) * rotation_axis.x;
        q.y = (float)sin(angle / 2) * rotation_axis.y;
        q.z = (float)sin(angle / 2) * rotation_axis.z;
        // rotate the vector
        v = qv_rotate(q, v);
    }
}
#endif // __cg_quaternion__