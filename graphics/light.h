#ifndef __light_h__
#define __light_h__

#include "vector.h"
#include "shaders.h"

namespace atlas {
    class gl_effect {
    public:
        gl_effect() {}
        ~gl_effect() {}
        virtual void apply(gl_shader* shdr) = 0;
    };

    class gl_light : public gl_effect {
    protected:
        vec3 pos_or_dir;		// light location
        vec3 ambient;			// the ambience property of the light
        vec3 diffuse;			// the ambience property of the light
        vec3 specular;			// the ambience property of the light
    public:
        enum light_type { SPOTLIGHT = 1, DIRLIGHT };
        light_type _type;
        gl_light(light_type _t = SPOTLIGHT) :pos_or_dir(vec3(8, 2, 8)), ambient(vec3(0.4f)),
            diffuse(vec3(0.7f)), specular(vec3(0.95f)),
            _type(_t) {
        }
        ~gl_light() {
        }
        void set_ambient(const vec3& a) {
            ambient = a;
        }
        vec3 get_ambient() {
            return ambient;
        }
        void set_diffuse(const vec3& d) {
            diffuse = d;
        }
        vec3 get_diffuse() {
            return diffuse;
        }
        void set_specular(const vec3& s) {
            specular = s;
        }
        vec3 get_specular() {
            return specular;
        }
        void set_position(const vec3& p) {
            pos_or_dir = p;
        }
        vec3 get_position() {
            return pos_or_dir;
        }
        void set_direction(const vec3& d) {
            pos_or_dir = d;
            pos_or_dir.normalize();
        }
        vec3 get_direction() {
            return pos_or_dir;
        }
        virtual void apply(gl_shader* shdr) {
            shdr->set_int("light.type", (int)_type);
            shdr->set_vec3("light.pos_or_dir", pos_or_dir);
            shdr->set_vec3("light.ambient", ambient);
            shdr->set_vec3("light.diffuse", diffuse);
            shdr->set_vec3("light.specular", specular);
        }
    };

    class gl_fog : public gl_effect {
        vec3 color;
        float linearStart;
        float linearEnd;
        float density;

        int equation;
    public:
        gl_fog():color(1), linearStart(0), linearEnd(0), density(.025f), equation(0) {
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
#endif // __light_h__
