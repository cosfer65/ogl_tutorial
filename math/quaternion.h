#ifndef __quaternion_h__
#define __quaternion_h__

#include "amath.h"
#include "matrix.h"

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

        vec3 get_vector(void) const {
            return vec3(x, y, z);
        }

        float get_scalar(void) const {
            return w;
        }

        quaternion operator~(void) const {
            return quaternion(w, -x, -y, -z);
        }

        mat4 matrix() const {
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

    inline quaternion operator + (const quaternion& q1, const quaternion& q2) {
        return quaternion(q1.w + q2.w,
            q1.x + q2.x,
            q1.y + q2.y,
            q1.z + q2.z);
    }

    inline quaternion operator - (const quaternion& q1, const quaternion& q2) {
        return quaternion(q1.w - q2.w,
            q1.x - q2.x,
            q1.y - q2.y,
            q1.z - q2.z);
    }

    inline quaternion operator * (const quaternion& q, float s) {
        return quaternion(q.w * s, q.x * s, q.y * s, q.z * s);
    }

    inline quaternion operator * (float s, const quaternion& q) {
        return quaternion(q.w * s, q.x * s, q.y * s, q.z * s);
    }

    inline quaternion operator * (const vec3& v, const quaternion& q) {
        return quaternion(-(q.x * v.x + q.y * v.y + q.z * v.z),
            q.w * v.x + q.z * v.y - q.y * v.z,
            q.w * v.y + q.x * v.z - q.z * v.x,
            q.w * v.z + q.y * v.x - q.x * v.y);
    }

    inline quaternion operator / (const quaternion& q, float s) {
        return quaternion(q.w / s, q.x / s, q.y / s, q.z / s);
    }

    inline quaternion operator * (const quaternion& q1, const quaternion& q2) {
        return quaternion(q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
            q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
            q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
    }
    inline quaternion operator * (const quaternion& q, const vec3& v) {
        return    quaternion(-(q.x * v.x + q.y * v.y + q.z * v.z),
            q.w * v.x + q.y * v.z - q.z * v.y,
            q.w * v.y + q.z * v.x - q.x * v.z,
            q.w * v.z + q.x * v.y - q.y * v.x);
    }

    inline float q_get_angle(const quaternion& q) {
        return (float)(2 * acos(q.w));
    }

    inline vec3 q_get_axis(const quaternion& q) {
        vec3 v;
        float m;

        v = q.get_vector();
        m = v.length();

        if (m <= tol)
            return vec3();
        else
            return v / m;
    }

    inline quaternion q_rotate(const quaternion& q1, const quaternion& q2) {
        return q1 * q2 * (~q1);
    }

    inline vec3 qv_rotate_inv(const quaternion& q, const vec3& v) {
        return ((~q) * v * q).get_vector();
    }

    inline vec3 qv_rotate(const quaternion& q, const vec3& v) {
        return (q * v * (~q)).get_vector();
    }

    inline quaternion make_q_from_euler_angles(float x, float y, float z) {
        double roll = deg_to_rad(z);
        double pitch = deg_to_rad(x);
        double yaw = deg_to_rad(y);

        double cyaw, cpitch, croll, syaw, spitch, sroll;
        double cyawcpitch, syawspitch, cyawspitch, syawcpitch;

        cyaw = cos(0.5f * yaw);
        cpitch = cos(0.5f * pitch);
        croll = cos(0.5f * roll);
        syaw = sin(0.5f * yaw);
        spitch = sin(0.5f * pitch);
        sroll = sin(0.5f * roll);

        cyawcpitch = cyaw * cpitch;
        syawspitch = syaw * spitch;
        cyawspitch = cyaw * spitch;
        syawcpitch = syaw * cpitch;

        quaternion q;
        q.w = (float)(cyawcpitch * croll + syawspitch * sroll);
        q.x = (float)(cyawcpitch * sroll - syawspitch * croll);
        q.y = (float)(cyawspitch * croll + syawcpitch * sroll);
        q.z = (float)(syawcpitch * croll - cyawspitch * sroll);
        q.normalize();

        return q;
    }

    inline vec3 make_euler_angles_from_q(const quaternion& q) {
        double r11, r21, r31, r32, r33, r12, r13;
        double q00, q11, q22, q33;
        double tmp;
        vec3 u;

        q00 = q.w * q.w;
        q11 = q.x * q.x;
        q22 = q.y * q.y;
        q33 = q.z * q.z;

        r11 = q00 + q11 - q22 - q33;
        r21 = 2 * (q.x * q.y + q.w * q.z);
        r31 = 2 * (q.x * q.z - q.w * q.y);
        r32 = 2 * (q.y * q.z + q.w * q.x);
        r33 = q00 - q11 - q22 + q33;

        tmp = fabs(r31);
        if (tmp > 0.999999) {
            r12 = 2 * (q.x * q.y - q.w * q.z);
            r13 = 2 * (q.x * q.z + q.w * q.y);

            u.x = 0.f;// rad_to_deg(0.0f); //roll
            u.y = (float)(-(pi / 2) * r31 / tmp);// rad_to_deg((float)(-(pi / 2) * r31 / tmp)); // pitch
            u.z = (float)atan2(-r12, -r31 * r13);// rad_to_deg((float)atan2(-r12, -r31 * r13)); // yaw
            return u;
        }

        u.x = (float)atan2(r32, r33);// rad_to_deg((float)atan2(r32, r33)); // roll
        u.y = (float)asin(-r31);// rad_to_deg((float)asin(-r31)); // pitch
        u.z = (float)atan2(r21, r11);// rad_to_deg((float)atan2(r21, r11)); // yaw
        return u;
    }

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

    inline void calculateTransformationMatrix(mat4& transformMatrix, const vec3& position, const quaternion& orientation)
    {
        transformMatrix[0] = 1 - 2 * orientation.y * orientation.y -
            2 * orientation.z * orientation.z;
        transformMatrix[1] = 2 * orientation.x * orientation.y -
            2 * orientation.w * orientation.z;
        transformMatrix[2] = 2 * orientation.x * orientation.z +
            2 * orientation.w * orientation.y;
        transformMatrix[3] = position.x;

        transformMatrix[4] = 2 * orientation.x * orientation.y +
            2 * orientation.w * orientation.z;
        transformMatrix[5] = 1 - 2 * orientation.x * orientation.x -
            2 * orientation.z * orientation.z;
        transformMatrix[6] = 2 * orientation.y * orientation.z -
            2 * orientation.w * orientation.x;
        transformMatrix[7] = position.y;

        transformMatrix[8] = 2 * orientation.x * orientation.z -
            2 * orientation.w * orientation.y;
        transformMatrix[9] = 2 * orientation.y * orientation.z +
            2 * orientation.w * orientation.x;
        transformMatrix[10] = 1 - 2 * orientation.x * orientation.x -
            2 * orientation.y * orientation.y;
        transformMatrix[11] = position.z;
    }
}
#endif // __quaternion_h__
