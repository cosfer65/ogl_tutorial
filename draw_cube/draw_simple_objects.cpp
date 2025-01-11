#include "draw_simple_objects.h"
#include "camera.h"
#include "util.h"
#include "primitives.h"

using namespace atlas;

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;

    // wireframe shader
    gl_shader* m_shader;
    // texture shader
    gl_shader* mt_shader;

    // wireframe objects
    gl_prim* simple_cube;
    gl_prim* simple_globe;
    // textured objects
    gl_prim* simple_cube_t;
    gl_prim* simple_globe_t;

    GLuint   texture_globe;
    GLuint   texture_cube;

public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (draw simple objects)";
    }
    virtual int init_application() {
        // zoom is the angle of the field of view
        m_view->set_fov(dtr(25));
        // camera position, look at point, and up vector orientation
        m_cam = new gl_camera(vec3(0, 0, -20), vec3(0, 0, -1), vec3(0, 1, 0));

        // create the basic shader
        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/draw_cube_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/draw_cube_fs.glsl");
        m_shader->load();

        // create the texture shader
        mt_shader = new gl_shader;
        mt_shader->add_file(GL_VERTEX_SHADER, "resources/draw_cubet_vs.glsl");
        mt_shader->add_file(GL_FRAGMENT_SHADER, "resources/draw_cubet_fs.glsl");
        mt_shader->load();

        // exture for sphere/globe
        texture_globe = load_texture("resources/earth2.tga");
        // texture for cube
        texture_cube = load_texture("resources/planks.tga");

        // create the wireframe objects
        simple_cube = create_cube(GL_LINE);
        simple_globe = create_sphere(GL_LINE);

        // create the textured objects
        simple_cube_t = create_cube(GL_FILL);
        simple_globe_t = create_sphere(GL_FILL);

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

    virtual void step_simulation(float fElapsed) {
        // rotate the objects
        simple_cube->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        simple_globe->rotate_by(vec3(0, dtr(fElapsed * 20), 0));

        simple_cube_t->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        simple_globe_t->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
    }

    virtual void render() {
        // set the viewport to the whole window
        m_view->set_viewport();

        // clear screen
        glClearColor(.5f, .5f, .5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // combine the view and camera matrices into one
        // these matrices are COLUMN MAJOR!
        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

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