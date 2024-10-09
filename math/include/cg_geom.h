#ifndef __cg_geom_h__
#define __cg_geom_h__

#include "cg_matrix.h"

namespace atlas {
    struct triangle {
    public:
        vec3 v1, v2, v3;
        vec3 norm;
        //vec2 t1, t2, t3; // texture coords

        triangle() {}
        triangle(const vec3& _v1, const vec3& _v2, const vec3& _v3, const vec3& n) :
            v1(_v1), v2(_v2), v3(_v3), norm(n) {}
        ~triangle() {}
    };

    // Given three points on a plane in counter clockwise order, calculate the unit normal
    inline vec3 calc_normal(const vec3& vP1, const vec3& vP2, const vec3& vP3) {
        vec3 vNormal;
        vec3 vV1(vP2 - vP1);
        vec3 vV2(vP3 - vP1);
        vNormal = cross(vV1, vV2);
        vNormal.normalize();
        return vNormal;
    }
    // project point P on vector defined by the vertices A,B
    inline vec3 project_point(vec3 P, vec3 A, vec3 B) {
        vec3 e(B - A);
        vec3 v(A + dot(P - A, e) / dot(e, e) * e);
        return v;
    }
    // project b on a
    inline vec3 project_v_on_v(vec3 a, vec3 b)
    {
        vec3 c = dot(a, b) * a / (a.length() * a.length());
        return c;
    }

    // check if point P is between points A and B
    inline bool is_between(vec3 P, vec3 A, vec3 B) {
        if (dot(A - P, B - P) < 0)
            return true;
        return false;
    }
    inline float SignedVolume(const vec3& a, const vec3& b, const vec3& c, const vec3& d) {
        return (float)(1.0 / 6.0) * dot(cross(b - a, c - a), d - a);
    }

    inline bool triangle_line_intersect(const vec3 p[3], const vec3 q[2])
    {
        float v1 = SignedVolume(q[0], p[0], p[1], p[2]);
        float v2 = SignedVolume(q[1], p[0], p[1], p[2]);
        if (v1 * v2 > 0)
            return false;

        float s1 = SignedVolume(q[0], q[1], p[0], p[1]);
        float s2 = SignedVolume(q[0], q[1], p[1], p[2]);
        float s3 = SignedVolume(q[0], q[1], p[2], p[0]);
        if (s1 * s2 > 0 && s1 * s3 > 0)
            return true;

        return false;
    }

    inline bool triangles_intersect(const vec3 tria1[3], const vec3 tria2[3])
    {
        vec3 e1[] = { tria2[0], tria2[1] };
        if (triangle_line_intersect(tria1, e1))
            return true;
        vec3 e2[] = { tria2[1], tria2[2] };
        if (triangle_line_intersect(tria1, e2))
            return true;
        vec3 e3[] = { tria2[2], tria2[0] };
        if (triangle_line_intersect(tria1, e3))
            return true;
        return false;
    }
}

#endif // __cg_geom_h__
