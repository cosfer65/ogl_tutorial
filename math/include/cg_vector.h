#ifndef __cg_vector_h__
#define __cg_vector_h__

#include "cg_basevec.h"

namespace atlas {
    typedef basevec2<int> ivec2;
    typedef basevec3<float> vec3;
    typedef basevec4<float> vec4;

    template <typename T>
    inline T dot(const basevec2<T>& v1, const basevec2<T>& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
    template <typename T>
    inline T dot(const basevec3<T>& v1, const basevec3<T>& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    template <typename T>
    inline T dot(const basevec4<T>& v1, const basevec4<T>& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }
    template <typename T>
    inline basevec3<T> cross(const basevec3<T>& v1, const basevec3<T>& v2) {
        basevec3<T> vResult;
        vResult.x = v1.y * v2.z - v1.z * v2.y;
        vResult.y = v1.z * v2.x - v1.x * v2.z;
        vResult.z = v1.x * v2.y - v1.y * v2.x;
        return vResult;
    }

    // distance of v3 fom the line defined by v1,v2
    inline vec3 distance(const vec3& v1, const vec3& v2, const vec3& v3)
    {
        vec3 d = v2 - v1;
        d.normalize();
        vec3 v(v3 - v1);
        float t = dot(v, d);
        vec3 res = v1 + t * d;
        return res;
    }
}

#endif // __cg_vector_h__