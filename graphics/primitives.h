#ifndef __primitives__
#define __primitives__

#include "amath.h"
#include "matrix.h"
#include "mesh.h"
#include "material.h"
#include "shaders.h"

namespace atlas {
    class base_3d_model;

    class gl_prim {
    protected:
        mesh_sizes m_mesh_sizes;
        GLuint vao;

        // Specifies how polygons will be rasterized.
        // Accepted values are GL_POINT, GL_LINE, and GL_FILL.
        GLenum draw_mode;
        GLenum draw_type;
        bool draw_elements;
        vec3 position;
        vec3 scale;
        vec3 rotation;
        int use_vertex_color;

        cg_material* m_material; // allow drawable objects to have their own materials
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
            draw_type = GL_TRIANGLES;
            draw_elements = true;
            // matrices are row-major!
            rmat = rotation_matrix(rotation.x, 0, 0, 1) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 1, 0, 0);
            smat = scaling_matrix(scale.x, scale.y, scale.z);
            tmat = translation_matrix(position.x, position.y, position.z);
            use_vertex_color = 0;
        }
        virtual ~gl_prim() {}

        virtual void create_from_mesh(c_mesh* mesh, GLenum drmode = GL_FILL, bool dr_el = true);

        void set_material(cg_material* m) {
            m_material = m;
        }
        const cg_material* material() {
            return m_material;
        }

        void calc_ob_matrix() {
            mat4 ob_matrix = tmat * rmat * smat;
            ob_matrix.transpose();
        }

        virtual void render(gl_shader* _shader) {
            if (!vao) return;

            if (use_vertex_color) {
                _shader->set_int("use_vertex_color", 1);
            }
            else if (m_material) {
                m_material->apply(_shader);
                _shader->set_vec3("objectColor", m_material->get_ambient());
                _shader->set_int("use_vertex_color", 0);
            }

            // position object
            mat4 ob_matrix = tmat * rmat * smat;
            ob_matrix.transpose();    // convert for OpenGL!

            // pass transformation to shader
            _shader->set_mat4("model", ob_matrix);

            glBindVertexArray(vao);
            if (draw_elements)
            {
                // setup drawing
                if (draw_type == GL_TRIANGLES) {
                    glFrontFace(GL_CCW);
                    glPolygonMode(GL_FRONT, draw_mode);
                }
                glDrawElements(draw_type, (unsigned int)m_mesh_sizes.num_indices, GL_UNSIGNED_INT, 0);
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
        void set_draw_type(GLenum dt) {
            draw_type = dt;
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
        void rotate_to(float x, float y, float z) {
            rotation = vec3(x, y, z);
            rmat = rotation_matrix(rotation.x, 1, 0, 0) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 0, 0, 1);
        }
        void rotate_by(const vec3& _r) {
            rotation += _r;
            rmat = rotation_matrix(rotation.x, 1, 0, 0) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 0, 0, 1);
        }
        void rotate_by(float x, float y, float z) {
            rotation += vec3(x, y, z);
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

    class gl_model {
        std::vector<gl_prim*> m_objects;
        base_3d_model* loaded_model = nullptr;
    public:
        gl_model() {}
        gl_model(const std::string& model_file);

        ~gl_model() {
            clean_up();
        }
        void clean_up();
        void create(base_3d_model* model);
        void load(const std::string& model_file);
        void save(const std::string& model_file);

        void render(gl_shader* m_shader) {
            for (auto p : m_objects) {
                p->render(m_shader);
            }
        }
        void move_to(const vec3& m) {
            for (auto p : m_objects) {
                p->move_to(m);
            }
        }
        void rotate_by(float x, float y, float z) {
            for (auto p : m_objects) {
                p->rotate_by(vec3(x, y, z));
            }
        }
        void rotate_to(float x, float y, float z) {
            for (auto p : m_objects) {
                p->rotate_to(vec3(x, y, z));
            }
        }
        void set_scale(const vec3& s) {
            for (auto p : m_objects) {
                p->set_scale(s);
            }
        }
    };

    class gl_stencil :public gl_prim {
    protected:
    public:
        gl_stencil() {
        }
        virtual ~gl_stencil() {
        }
        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true);
        virtual void create_elliptic(GLenum drmode = GL_FILL, bool dr_el = true);
        virtual void r_create(GLenum drmode = GL_FILL, bool dr_el = true);
    };

    gl_prim* create_cube(GLenum drmode = GL_FILL, bool dr_el = true);
    gl_prim* create_sphere(GLenum drmode = GL_FILL, bool dr_el = true);
    gl_prim* create_cylinder(GLenum drmode = GL_FILL, bool dr_el = true);
    gl_prim* create_plane(GLenum drmode = GL_FILL, bool dr_el = true);
    gl_prim* create_cross(GLenum drmode = GL_FILL, bool dr_el = true);
    gl_prim* create_diamond(GLenum drmode = GL_FILL, bool dr_el = true);
    gl_prim* create_hbar(GLenum drmode = GL_FILL, bool dr_el = true);
}

#endif // __primitives__
