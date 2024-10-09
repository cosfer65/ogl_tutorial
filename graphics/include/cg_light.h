#ifndef __cg_light__
#define __cg_light__

#include "cg_math.h"
#include "cg_shader.h"

namespace atlas {
    class cg_light {
    protected:
        vec3 pos_or_dir;		// light location
        vec3 ambient;			// the ambience property of the light
        vec3 diffuse;			// the ambience property of the light
        vec3 specular;			// the ambience property of the light
    public:
        enum light_type { SPOTLIGHT = 1, DIRLIGHT };
        light_type _type;
        cg_light(light_type _t = SPOTLIGHT) :pos_or_dir(vec3(8, 2, 8)), ambient(vec3(0.4f)),
            diffuse(vec3(0.7f)), specular(vec3(0.95f)),
            _type(_t) {
        }
        ~cg_light() {
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
        void apply(cg_shader* shdr) {
            shdr->set_int("light.type", (int)_type);
            shdr->set_vec3("light.pos_or_dir", pos_or_dir);
            shdr->set_vec3("light.ambient", ambient);
            shdr->set_vec3("light.diffuse", diffuse);
            shdr->set_vec3("light.specular", specular);
        }
    };

    class cg_material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shine;

        int diffuse_map;
        int diffuse_index;
        int specular_map;
        int specular_index;
    public:
        cg_material() : ambient(vec3(0.2f, 0.2f, 0.2f)),
            diffuse(vec3(.95f, .95f, .95f)), specular(vec3(.5f, 0.5f, 0.5f)),
            shine(128.f), diffuse_map(-1), diffuse_index(-1),
            specular_map(-1), specular_index(-1) {
        }
        ~cg_material() {
        }
        void set_ambient(float a0, float a1, float a2) {
            ambient = vec3(a0, a1, a2);
        }
        void set_diffuse(float a0, float a1, float a2) {
            diffuse = vec3(a0, a1, a2);
        }
        void set_diffuse(int texture, int active_texture) {
            diffuse_map = texture;
            diffuse_index = active_texture;
        }
        void set_specular(float a0, float a1, float a2) {
            specular = vec3(a0, a1, a2);
        }
        void set_specular(int texture, int active_texture) {
            specular_map = texture;
            specular_index = active_texture;
        }
        void set_shine(float s) {
            shine = s;
        }
        void apply(cg_shader* shdr)
        {
            shdr->set_vec3("mat.ambient", ambient);
            shdr->set_vec3("mat.diffuse", diffuse);
            shdr->set_vec3("mat.specular", specular);
            shdr->set_float("mat.shine", shine);
            if (diffuse_index >= 0)
            {
                shdr->set_int("mat.diffuse_map", diffuse_index); // GL_TEXTURE0=0...
                glActiveTexture(GL_TEXTURE0 + diffuse_index);
                glBindTexture(GL_TEXTURE_2D, diffuse_map);
            }
            if (specular_index >= 0)
            {
                shdr->set_int("mat.specular_map", specular_index); // GL_TEXTURE0=0...
                glActiveTexture(GL_TEXTURE0 + specular_index);
                glBindTexture(GL_TEXTURE_2D, specular_map);
            }
        }
    };
}
#endif // __cg_light__
