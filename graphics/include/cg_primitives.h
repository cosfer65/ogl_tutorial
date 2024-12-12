#ifndef __cg_prim__
#define __cg_prim__

#include "cg_math.h"
#include "cg_mesh.h"
#include "cg_shader.h"

namespace atlas {
    class cg_prim {
    protected:
        cg_mesh_sizes m_mesh_sizes;
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

        cg_prim() {
            vao = 0;
            scale = vec3(1);
            position = vec3(0);
            rotation = vec3(0);
            draw_mode = GL_FILL;
            draw_elements = true;
            rmat = rotation_matrix(rotation.x, 1, 0, 0) * rotation_matrix(rotation.y, 0, 1, 0) * rotation_matrix(rotation.z, 0, 0, 1);
            smat = scaling_matrix(scale.x, scale.y, scale.z);
            tmat = translation_matrix(position.x, position.y, position.z);
        }
        virtual ~cg_prim() { }

        virtual void create_from_mesh(cg_mesh* mesh, GLenum drmode = GL_FILL, bool dr_el = true);

        virtual void render(cg_shader* _shader) {
            if (!vao) return;

            // position object
            mat4 ob_matrix = tmat * rmat * smat;
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

        virtual void frame_move(float fElapsed) {}
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
            position += vec3(x,y,z);
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

    class cg_gl_cube :public cg_prim {
    protected:
    public:
        cg_gl_cube() : cg_prim() {
            draw_mode = GL_LINE;
        }
        virtual ~cg_gl_cube() {
        }
        virtual void create(GLenum drmode = GL_LINE, bool dr_el = true);
    };

    class cg_gl_sphere : public cg_prim {
    protected:
        float radius;
    public:
        cg_gl_sphere(float r = 0.5f) : radius(r) {}
        virtual ~cg_gl_sphere() {}

        float get_radius() {
            return radius;
        }
        void set_radius(float _r) {
            set_scale(_r / radius);
        }

        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true);
    };

    class cg_gl_cylinder : public cg_prim {
    protected:

        float length;
        float radius;

    public:
        cg_gl_cylinder(float rad = .5f, float len = 1.f) : length(len), radius(rad) /*, sectorCount(18)*/ {
            set_scale(vec3(rad / 0.5f, len, rad / 0.5f));
        }
        virtual ~cg_gl_cylinder() {}
        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true);
    };

    class cg_gl_plane:public cg_prim {
    protected:
        float minx;
        float maxx;
        float miny;
        float maxy;
        float minz;
        float maxz;
    public:
        cg_gl_plane() {
            set_extends(-2, 2, 0, 0, -2, 2);
        }
        virtual ~cg_gl_plane() {            
        }
        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true);
        void set_extends(float _minx, float _maxx, float _miny, float _maxy, float _minz, float _maxz) {
            minx = _minx;
            maxx = _maxx;
            miny = _miny;
            maxy = _maxy;
            minz = _minz;
            maxz = _maxz;
        }
    };
    class cg_gl_mesh :public cg_prim {
    protected:
    public:
        cg_gl_mesh() {
        }
        virtual ~cg_gl_mesh() {
        }
        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true);
    };

}

#endif //__cg_prim__
