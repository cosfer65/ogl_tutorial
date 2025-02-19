#include "stencil.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"

using namespace atlas;

class gl_background :public gl_prim {
protected:
public:
    gl_background() {
    }
    virtual ~gl_background() {
    }
    virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        draw_elements = true;
        draw_mode = GL_FILL;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;

            tmesh->addVertex(-1.f, -1.f, 0.f);
            tmesh->addVertex(1.f, -1.f, 0.f);
            tmesh->addVertex(1.f, 1.f, 0.f);
            tmesh->addVertex(-1.f, 1.f, 0.f);

            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);

            tmesh->addIndices(0, 1, 2);
            tmesh->addIndices(0, 2, 3);

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
};

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    gl_prim* m_cube;
    float xcen;
    float ycen;
    float xstep;
    float ystep;
    gl_stencil* pstencil;
    gl_background* pbackgound;

public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "stencil";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(25));
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(10, 10, 10));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/stencil_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/stencil_fs.glsl");
        m_shader->load();

        m_cube = create_cube(GL_FILL, true);
        m_cube->set_scale(vec3(1.2f, 1.2f, 1.2f));

        pbackgound = new gl_background;
        pbackgound->create();
        pbackgound->set_scale(vec3(10));
        pbackgound->move_to(vec3(0, 0, -20));

        // these hadle the movement of the stencl
        xcen = 0.f;
        ycen = 0.f;
        xstep = .2f;
        ystep = .125f;

        // the stencil object
        pstencil = new gl_stencil;
        pstencil->create_elliptic();
        pstencil->set_scale(vec3(0.25));

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }
    virtual void exit_application() {
    }
    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }

    virtual void step_simulation(float fElapsed) {
        m_cube->rotate_by(vec3(dtr(fElapsed * 15), dtr(fElapsed * 30), dtr(fElapsed * 45)));
        xcen += xstep * fElapsed;
        ycen += ystep * fElapsed;
        if (xcen > 0.25f) xstep = -xstep;
        if (xcen < -0.25f) xstep = -xstep;
        if (ycen > 0.25f) ystep = -ystep;
        if (ycen < -0.25f) ystep = -ystep;
        // move the window
        pstencil->move_to(xcen, ycen, 0);
    }

    virtual void render() {
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
        glStencilFunc(GL_NEVER, 1, 0xff);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

        // draw stencil pattern
        m_shader->use();
        mat4 ob_matrix;
        ob_matrix.loadIdentity();  // at the center of the screen
        m_shader->set_mat4("model", ob_matrix);
        m_shader->set_mat4("camera", ob_matrix);

        // disabling these to speed up drawing
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        pstencil->render(m_shader);

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
        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        // draw a rectangle behind the cube to act as background
        // this will make the stecil shape visible
        m_shader->set_vec3("objectColor", vec3(0.95f, 0.55f, 0.15f));
        pbackgound->render(m_shader);
        // and now draw the cube
        m_shader->set_vec3("objectColor", vec3(.1f, .2f, .99f));
        m_cube->render(m_shader);
        // stop using stncil
        glDisable(GL_STENCIL_TEST);
        glUseProgram(0);
    }
};

atlas_app my_app;       // default