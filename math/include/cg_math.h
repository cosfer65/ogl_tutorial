// called cronusmath to avoid conflict with std lib
#ifndef __cg_math_h__
#define __cg_math_h__

#include <math.h>

namespace atlas {
    float const pi = 3.1415926535897932384626433832795f;
    float const tol = 0.00001f;		// float type tolerance
    float const pi2 = 6.283185307179586476925286766559f;
    float const pid180 = 0.017453292519943295769236907684886f;
    float const pid180inv = 57.295779513082320876798154814105f;

    inline float deg_to_rad(float deg) {
        return deg * pid180;
    }
#define dtr deg_to_rad

    inline float rad_to_deg(float rad) {
        return rad * pid180inv;
    }
#define rtd rad_to_deg

    inline int dti(double d) {   // approximate float to integer
        return (int)floor(d + 0.5);
    }

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
};

#pragma comment( lib, "cg_math.lib" )

#endif // __cg_math_h__
