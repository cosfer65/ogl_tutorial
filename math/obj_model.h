#ifndef __obj_model_h__
#define __obj_model_h__

#include "amath.h"

#include <vector>
#include <string>

namespace atlas {

    class base_3d_object;
    typedef std::vector<base_3d_object*> obList;
    struct base_aterial;
    typedef std::vector<base_aterial*> matList;

    class base_3d_model {
    public:
        base_3d_model() {}

        std::string fname;

        obList objects;
        matList materials;
        vec3 bboxmin;
        vec3 bboxmax;
        vec3 geocenter;
    };

    class obj_model : public base_3d_model {
    public:
        obj_model() :base_3d_model() {

        }
        bool load(const std::string& fnm) {
            fname = fnm;
        }
    };

}

#endif // __obj_model_h__
