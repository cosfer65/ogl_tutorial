#ifndef __geom_h__
#define __geom_h__

#include "amath.h"
#include "matrix.h"

namespace atlas {
    struct triangle {
    public:
        vec3 v1, v2, v3;
        vec3 norm;

        triangle() {}
        triangle(const vec3& _v1, const vec3& _v2, const vec3& _v3, const vec3& n) :
            v1(_v1), v2(_v2), v3(_v3), norm(n) {
        }
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

    // if any of the vertices is inside the spere we have penetration
    // else
    // if projection of center of cg_sphere P on edge AB
    // A + dot(AP,AB) / dot(AB,AB) * AB
    // lies on the edge and inside the cg_sphere we have collision
    inline bool point_in_triangle(const vec3* tvertices, vec3 pos) {
        vec3 u(tvertices[1] - tvertices[0]);
        vec3 v(tvertices[2] - tvertices[0]);
        vec3 n(cross(u, v));
        vec3 w = pos - tvertices[0];

        float nn = dot(n, n);
        //nn = nn * nn;
        float gamma = dot(cross(u, w), n) / nn;
        float beta = dot(cross(w, v), n) / nn;
        float alpha = 1 - gamma - beta;

        return ((0 <= alpha) && (alpha <= 1) && (0 <= beta) && (beta <= 1) && (0 <= gamma) && (gamma <= 1));
    }

    inline bool line_sphere_intersection(const vec3& s, const vec3& t, const vec3& sphere_pos, float sphere_radius) {
        float x0 = s.x;
        float y0 = s.y;
        float z0 = s.z;
        float x1 = t.x;
        float y1 = t.y;
        float z1 = t.z;
        float xc = sphere_pos.x;
        float yc = sphere_pos.y;
        float zc = sphere_pos.z;
        float R = sphere_radius;
        float A = (x0 - xc) * (x0 - xc) + (y0 - yc) * (y0 - yc) + (z0 - zc) * (z0 - zc) - R * R;
        float C = (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1) + (z0 - z1) * (z0 - z1);
        float B = (x1 - xc) * (x1 - xc) + (y1 - yc) * (y1 - yc) + (z1 - zc) * (z1 - zc) - A - C - R * R;

        vec2 res;
        return solve_quadratic(A, B, C, res);
    }
}

#endif // __geom_h__
