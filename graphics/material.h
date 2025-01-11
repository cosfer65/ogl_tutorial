#ifndef __material_h__
#define __material_h__

#include "graphics.h"
#include "vector.h"
#include "shaders.h"

namespace atlas {

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
        vec3 get_ambient() const{
            return ambient;
        }
        vec3 get_diffuse() const {
            return diffuse;
        }
        vec3 get_specular() const {
            return specular;
        }

        void set_ambient(const vec3& a) {
            ambient = a;
        }
        void set_ambient(float a0, float a1, float a2) {
            ambient = vec3(a0, a1, a2);
        }
        void set_diffuse(const vec3& d) {
            diffuse = d;
        }
        void set_diffuse(float a0, float a1, float a2) {
            diffuse = vec3(a0, a1, a2);
        }
        void set_diffuse(int texture, int active_texture) {
            diffuse_map = texture;
            diffuse_index = active_texture;
        }
        void set_specular(const vec3& s) {
            specular = s;
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
        void apply(gl_shader* shdr)
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

#endif // __material_h__
