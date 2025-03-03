#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "vector.h"
#include "shaders.h"

namespace atlas {
    class gl_effect {
    public:
        gl_effect() {}
        ~gl_effect() {}
        virtual void apply(gl_shader* shdr) = 0;
    };

    class gl_fog : public gl_effect {
        vec3 color;
        float linearStart;
        float linearEnd;
        float density;

        int equation;
    public:
        gl_fog() :color(1), linearStart(0), linearEnd(0), density(.025f), equation(0) {
        }
        void set_color(const vec3& c) {
            color = c;
        }
        void set_linear(float start, float end) {
            linearStart = start;
            linearEnd = end;
            equation = 0;
        }
        void set_density(float d) {
            density = d;
            equation = 1;
        }
        void set_exponential(float d) {
            density = d;
            equation = 2;
        }
        virtual void apply(gl_shader* shdr) {
            shdr->set_vec3("fogParams.color", color);
            shdr->set_float("fogParams.linearStart", linearStart);
            shdr->set_float("fogParams.linearEnd", linearEnd);
            shdr->set_float("fogParams.density", density);
            shdr->set_int("fogParams.equation", equation);
            shdr->set_int("fogParams.isEnabled", 1);
        }

    };
}

#endif // __EFFECTS_H__
