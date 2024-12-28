#ifndef __primitives__
#define __primitives__

#include "amath.h"
#include "matrix.h"
#include "mesh.h"
#include "shaders.h"

namespace atlas {
    class gl_prim {
    protected:
        mesh_sizes m_mesh_sizes;
        GLuint vao;

        // Specifies how polygons will be rasterized.
        // Accepted values are GL_POINT, GL_LINE, and GL_FILL.
        GLenum draw_mode;
        bool draw_elements;
        vec3 position;
        vec3 scale;
        vec3 rotation;
    public:
        mat4 rmat;  // rotation matrix (local)
        mat4 tmat;  // translation matrix
        mat4 smat;  // scaling matrix

        gl_prim() {
            vao = 0;
            scale = vec3(1);
            position = vec3(0);
            rotation = vec3(0);
            draw_mode = GL_FILL;
            draw_elements = true;
            // matrices are row-major!
            rmat = rotation_matrix(rotation.x, 0, 0, 1) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 1, 0, 0);
            smat = scaling_matrix(scale.x, scale.y, scale.z);
            tmat = translation_matrix(position.x, position.y, position.z);
        }
        virtual ~gl_prim() {}

        virtual void create_from_mesh(c_mesh* mesh, GLenum drmode = GL_FILL, bool dr_el = true);

        void calc_ob_matrix() {
            mat4 ob_matrix = tmat * rmat * smat;
            ob_matrix.transpose();
        }

        virtual void render(c_shader* _shader) {
            if (!vao) return;

            // position object
            mat4 ob_matrix = tmat * rmat * smat;
            ob_matrix.transpose();    // convert for OpenGL!

            // pass transformation to shader
            _shader->set_mat4("model", ob_matrix);
            glBindVertexArray(vao);
            if (draw_elements)
            {
                // setup drawing
                glFrontFace(GL_CCW);
                glPolygonMode(GL_FRONT, draw_mode);
                glDrawElements(GL_TRIANGLES, (unsigned int)m_mesh_sizes.num_indices, GL_UNSIGNED_SHORT, 0);
            }
            else
            {
                // setup drawing
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                glPolygonMode(GL_FRONT_AND_BACK, draw_mode);
                glDrawArrays(GL_PATCHES, 0, (unsigned int)m_mesh_sizes.num_indices);
            }
            glBindVertexArray(0);
        }

        virtual void step_simulation(float fElapsed) {}
        void set_all(const vec3& _p, const vec3& _s, const vec3& _r) {
            position = _p;
            scale = _s;
            rotation = _r;
            tmat = translation_matrix(position.x, position.y, position.z);
            smat = scaling_matrix(scale.x, scale.y, scale.z);
            rmat = rotation_matrix(rotation.x, 1, 0, 0) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 0, 0, 1);
        }
        void rotate_to(const vec3& _r) {
            rotation = _r;
            rmat = rotation_matrix(rotation.x, 1, 0, 0) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 0, 0, 1);
        }
        void rotate_by(const vec3& _r) {
            rotation += _r;
            rmat = rotation_matrix(rotation.x, 1, 0, 0) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 0, 0, 1);
        }
        void move_to(const vec3& _r) {
            position = _r;
            tmat = translation_matrix(position.x, position.y, position.z);
        }
        void move_to(float x, float y, float z) {
            position = vec3(x, y, z);
            tmat = translation_matrix(x, y, z);
        }
        void move_by(const vec3& _r) {
            position += _r;
            tmat = translation_matrix(position.x, position.y, position.z);
        }
        void move_by(float x, float y, float z) {
            position += vec3(x, y, z);
            tmat = translation_matrix(position.x, position.y, position.z);
        }
        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        }
        void set_scale(float x, float y, float z) {
            scale = vec3(x, y, z);
            smat = scaling_matrix(scale.x, scale.y, scale.z);
        }
        void set_scale(const vec3& _s) {
            scale = _s;
            smat = scaling_matrix(scale.x, scale.y, scale.z);
        }
        void set_xscale(float _s) {
            scale.x = _s;
            smat = scaling_matrix(scale.x, scale.y, scale.z);
        }
        void set_yscale(float _s) {
            scale.y = _s;
            smat = scaling_matrix(scale.x, scale.y, scale.z);
        }
        void set_zscale(float _s) {
            scale.z = _s;
            smat = scaling_matrix(scale.x, scale.y, scale.z);
        }
        vec3& get_position() {
            return position;
        }
        vec3& get_scale() {
            return scale;
        }
        vec3& get_rotation() {
            return rotation;
        }
    };

    gl_prim* create_cube(GLenum drmode = GL_LINE, bool dr_el = true);
    gl_prim* create_sphere(GLenum drmode = GL_LINE, bool dr_el = true);
    gl_prim* create_cylinder(GLenum drmode = GL_LINE, bool dr_el = true);
    gl_prim* create_plane(GLenum drmode = GL_LINE, bool dr_el = true);
}

#endif // __primitives__