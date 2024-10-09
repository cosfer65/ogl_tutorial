#include "pch.h"
#include "cg_config.h"
#include "cg_quaternion.h"

namespace atlas {
    float q_get_angle(quaternion q) {
        return (float)(2 * acos(q.w));
    }

    vec3 q_get_axis(quaternion q) {
        vec3 v;
        float m;

        v = q.get_vector();
        m = v.length();

        if (m <= tol)
            return vec3();
        else
            return v / m;
    }

    quaternion q_rotate(quaternion q1, quaternion q2) {
        return q1 * q2 * (~q1);
    }

    vec3 qv_rotate_inv(quaternion q, vec3 v) {
        return ((~q) * v * q).get_vector();
    }

    vec3 qv_rotate(quaternion q, vec3 v) {
        return (q * v * (~q)).get_vector();
    }

    quaternion make_q_from_euler_angles(float x, float y, float z) {
        double roll = x;// deg_to_rad(x);
        double pitch = y;// deg_to_rad(y);
        double yaw = z;// deg_to_rad(z);

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

    vec3 make_euler_angles_from_q(quaternion q) {
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
}