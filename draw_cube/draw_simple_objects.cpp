#include "draw_simple_objects.h"
#include "cg_camera.h"
#include "cg_util.h"
#include "cg_primitives.h"

using namespace atlas;

class atlas_app :public cg_app {
    cg_viewport* m_view;
    cg_camera* m_cam;

    // wireframe shader
    cg_shader* m_shader;
    // texture shader
    cg_shader* mt_shader;

    // wireframe objects
    cg_gl_cube* simple_cube;
    cg_gl_sphere* simple_globe;
    // textured objects
    cg_gl_cube* simple_cube_t;
    cg_gl_sphere* simple_globe_t;

    GLuint   texture_globe;
    GLuint   texture_cube;

public:
    atlas_app() {
        m_view = new cg_viewport();
        m_window.szTitle = "GusOnGames (draw simple objects)";
    }
    virtual int init_game() {
        // zoom is the angle of the field of view
        m_view->set_fov(dtr(25));
        // camera position, look at point, and up vector orientation
        m_cam = new cg_camera(vec3(0, 0, -20), vec3(0, 0, 0), vec3(0, 1, 0));

        // create the basic shader
        m_shader = new cg_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/draw_cube_vs.hlsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/draw_cube_fs.hlsl");
        m_shader->load();

        // create the texture shader
        mt_shader = new cg_shader;
        mt_shader->add_file(GL_VERTEX_SHADER, "resources/draw_cubet_vs.hlsl");
        mt_shader->add_file(GL_FRAGMENT_SHADER, "resources/draw_cubet_fs.hlsl");
        mt_shader->load();

        // exture for sphere/globe
        texture_globe = load_texture("resources/earth2.tga");
        // texture for cube
        texture_cube = load_texture("resources/planks.tga");

        // create the wireframe objects
        simple_cube = new cg_gl_cube;
        simple_globe = new cg_gl_sphere(0.5);
        simple_cube->create(GL_LINE);
        simple_globe->create(GL_LINE);

        // create the textured objects
        simple_cube_t = new cg_gl_cube;
        simple_globe_t = new cg_gl_sphere(0.5);
        simple_cube_t->create(GL_FILL);
        simple_globe_t->create(GL_FILL);

        // position objects
        simple_cube->move_to(vec3(1.5, 1, -6.f));
        simple_globe->move_to(vec3(1.5, -1, -6.f));

        simple_cube_t->move_to(vec3(-1.5, 1, -6.f));
        simple_globe_t->move_to(vec3(-1.5, -1, -6.f));

        // OpenGL initialization we want for this sample
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void frame_move(float fElapsed) {
        // rotate the objects
        simple_cube->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        simple_globe->rotate_by(vec3(0, dtr(fElapsed * 20), 0));

        simple_cube_t->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        simple_globe_t->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
    }

    virtual void frame_render() {
        // set the viewport to the whole window
        m_view->set_viewport();

        // clear screen
        glClearColor(.5f, .5f, .5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // combine the view and camera matrices into one
        mat4 cam_matrix = m_view->perspective() * m_cam->perspective();

        // enable the shader
        m_shader->use();
        // set the combined view matrix
        m_shader->set_mat4("camera", cam_matrix);

        simple_cube->render(m_shader);
        simple_globe->render(m_shader);

        // enable the texture shader
        mt_shader->use();
        // set the combined view matrix
        mt_shader->set_mat4("camera", cam_matrix);

        // activate texture draw
        glActiveTexture(GL_TEXTURE0);

        // set the active texture
        glBindTexture(GL_TEXTURE_2D, texture_cube);
        // and draw
        simple_cube_t->render(mt_shader);

        glBindTexture(GL_TEXTURE_2D, texture_globe);
        simple_globe_t->render(mt_shader);
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

atlas_app my_app;       // default