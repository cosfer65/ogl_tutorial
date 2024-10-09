#include "stencil.h"
#include "cg_camera.h"
#include "cg_light.h"
#include "cg_util.h"
#include "cg_primitives.h"

using namespace atlas;

class cg_gl_stencil :public cg_prim {
protected:
public:
    cg_gl_stencil() {
    }
    virtual ~cg_gl_stencil() {
    }
    virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        draw_elements = true;
        draw_mode = GL_FILL;

        if (vao == 0)
        {
            cg_plane_mesh* tmesh = new cg_plane_mesh;
            tmesh = new cg_plane_mesh;

            tmesh->addVertex(-.2f, -.25f, 0);
            tmesh->addVertex(.2f, -.25f, 0);
            tmesh->addVertex(.2f, .25f, 0);
            tmesh->addVertex(-.2f, .25f, 0);

            tmesh->addIndices(0, 1, 2);
            tmesh->addIndices(0, 2, 3);

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
    virtual void render(cg_shader* _shader) {
        if (!vao) return;

        glBindVertexArray(vao);
        glFrontFace(GL_CCW);
        glPolygonMode(GL_FRONT, draw_mode);
        glDrawElements(GL_TRIANGLES, (unsigned int)m_mesh_sizes.num_indices, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
};

class cg_gl_background :public cg_prim {
protected:
public:
    cg_gl_background() {
    }
    virtual ~cg_gl_background() {
    }
    virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        draw_elements = true;
        draw_mode = GL_FILL;

        if (vao == 0)
        {
            cg_plane_mesh* tmesh = new cg_plane_mesh;
            tmesh = new cg_plane_mesh;

            tmesh->addVertex(-1.f, -1.f, 0.f);
            tmesh->addVertex(1.f, -1.f, 0.f);
            tmesh->addVertex(1.f, 1.f, 0.f);
            tmesh->addVertex(-1.f, 1.f, 0.f);

            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addTexCoord(0, 0);
            tmesh->addTexCoord(0, 1);
            tmesh->addTexCoord(1, 1);
            tmesh->addTexCoord(1, 0);

            tmesh->addIndices(0, 1, 2);
            tmesh->addIndices(0, 2, 3);

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
    virtual void render(cg_shader* _shader) {
        if (!vao) return;
        //if (!_shader) return;

        // position object
        mat4 ob_matrix = tmat * rmat * smat;
        // pass transformation to shader
        _shader->set_mat4("model", ob_matrix);

        glBindVertexArray(vao);
        glFrontFace(GL_CCW);
        glPolygonMode(GL_FRONT, draw_mode);
        glDrawElements(GL_TRIANGLES, (unsigned int)m_mesh_sizes.num_indices, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
};

class atlas_app :public cg_app {
    cg_viewport* m_view;
    cg_camera* m_cam;
    cg_light* m_light;
    cg_shader* m_shader;

    cg_gl_cube* m_cube;
    float xcen;
    float ycen;
    float xstep;
    float ystep;
    cg_gl_stencil* pstencil;
    cg_gl_background* pbackgound;

public:
    atlas_app() {
        m_view = new cg_viewport();
        m_window.szTitle = "Chapter 26a";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_game() {
        m_view->set_fov(dtr(15));
        m_cam = new cg_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new cg_light(cg_light::SPOTLIGHT);
        m_light->set_position(vec3(10, 10, 10));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        m_shader = new cg_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/stencil_vs.hlsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/stencil_fs.hlsl");
        m_shader->load();

        m_cube = new cg_gl_cube;
        m_cube->create(GL_FILL, true);
        m_cube->set_scale(vec3(1, 1.125f, 1.25f));
        m_cube->move_to(vec3(0));

        pbackgound = new cg_gl_background;
        pbackgound->create();
        pbackgound->set_scale(vec3(3));
        pbackgound->move_to(vec3(0, 0, -2));

        // these hadle the movement of the stencl
        xcen = 0.f;
        ycen = 0.f;
        xstep = .2f;
        ystep = .125f;

        // the stencil object
        pstencil = new cg_gl_stencil;
        pstencil->create();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }
    virtual void exit_game() {
    }
    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }

    virtual void frame_move(float fElapsed) {
        m_cube->rotate_by(vec3(dtr(fElapsed * 15), dtr(fElapsed * 30), dtr(fElapsed * 45)));
        xcen += xstep * fElapsed;
        ycen += ystep * fElapsed;
        if (xcen > 0.25f) xstep = -xstep;
        if (xcen < -0.25f) xstep = -xstep;
        if (ycen > 0.25f) ystep = -ystep;
        if (ycen < -0.25f) ystep = -ystep;
    }
    virtual void frame_render() {
        // set the viewport to the whole window
        m_view->set_viewport();

        // GL_STENCIL_BUFFER_BIT  needs mask=0xFF
        glStencilMask(0xff);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // we are using stencil
        glEnable(GL_STENCIL_TEST);
        // prepare the stencil buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glStencilFunc(GL_NEVER, 1, 0xFF);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

        // draw stencil pattern
        m_shader->use();
        mat4 ob_matrix;
        ob_matrix.loadIdentity();  // at the center of the screen
        m_shader->set_mat4("model", ob_matrix);
        m_shader->set_mat4("camera", ob_matrix);
        // move the window
        m_shader->set_vec3("displacement", vec3(xcen, ycen, 0));
        // disabling these to speed up drawing
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        pstencil->render(NULL);

        // return to normal drawing
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
        glStencilMask(0x00); // do NOT draw on black
        // draw only where stencil's value is 1
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // enable them before we start normal draing
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // stop moving around
        m_shader->set_vec3("displacement", vec3(0, 0, 0));
        // and proceed with normal drawing
        mat4 cam_matrix = m_view->perspective() * m_cam->perspective();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        // draw a rectangle behind the cube to act as background
        // this will make the stecil shape visible
        m_shader->set_vec3("objectColor", vec3(0.75f, 0.85f, 0.85f));
        pbackgound->render(m_shader);
        // and now draw the cube
        m_shader->set_vec3("objectColor", vec3(.1f, .2f, .9f));
        m_cube->render(m_shader);
        // stop using stncil
        glDisable(GL_STENCIL_TEST);
        glUseProgram(0);
    }
};

atlas_app my_app;       // default