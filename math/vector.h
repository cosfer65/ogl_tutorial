#ifndef __vector_h__
#define __vector_h__

#include "basevec.h"

namespace atlas {
    typedef basevec2<int> ivec2;
    typedef basevec3<int> ivec3;
    typedef basevec2<float> vec2;
    typedef basevec3<float> vec3;
    typedef basevec4<float> vec4;

    // distance of v3 fom the line defined by v1,v2
    inline vec3 distance(const vec3& v1, const vec3& v2, const vec3& v3) {
        vec3 d = v2 - v1;
        d.normalize();
        vec3 v(v3 - v1);
        float t = dot(v, d);
        vec3 res = v1 + t * d;
        return res;
    }
}

#endif // __vector_h__