#include "draw_simple_objects.h"
#include "camera.h"
#include "util.h"
#include "primitives.h"

using namespace atlas;

class draw_simple_objects_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;

    gl_shader* m_shader;

    gl_prim* m_cube;
    gl_prim* m_globe;

    gl_prim* m_cube_textured;
    gl_prim* m_globe_textured;

    GLuint   texture_globe;
    GLuint   texture_cube;
public:
    draw_simple_objects_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (draw simple objects)";
    }
    virtual ~draw_simple_objects_app() {
        delete m_view;
        delete m_cam;
        delete m_shader;
        delete m_cube;
        delete m_globe;
        delete m_cube_textured;
        delete m_globe_textured;
    }
    virtual int init_application() {
        // zoom is the angle of the field of view
        m_view->set_fov(dtr(20));
        // camera position, look at point, and up vector orientation
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        // create the basic shader
        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/generic_VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/dso_FragmentShader.glsl");
        m_shader->load();

        // create the wireframe objects
        m_cube = create_cube(GL_LINE);
        m_globe = create_sphere(GL_LINE);
        // create the filled objects
        m_cube_textured = create_cube(GL_FILL);
        m_globe_textured = create_sphere(GL_FILL);

        // position objects
        m_cube->move_to(vec3(-1.5, 1, 6.f));
        m_globe->move_to(vec3(-1.5, -1, 6.f));
        m_cube_textured->move_to(vec3(1.5, 1, 6.f));
        m_globe_textured->move_to(vec3(1.5, -1, 6.f));

        // exture for sphere/globe
        texture_globe = load_texture("resources/textures/earth2.tga");
        // texture for cube
        texture_cube = load_texture("resources/textures/planks.tga");

        // OpenGL initialization we want for this sample
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void step_simulation(float fElapsed) {
        // rotate the objects
        m_cube->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        m_globe->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
        m_cube_textured->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        m_globe_textured->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
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

        // render the objects using color
        m_shader->set_int("use_color_or_texture", 1);
        m_shader->set_vec4("object_color", vec4(.9f, .9f, .9f, 0.75f));
        m_cube->render(m_shader);
        m_globe->render(m_shader);

        // render the objects using texture
        m_shader->set_int("use_color_or_texture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_cube);
        m_cube_textured->render(m_shader);
        glBindTexture(GL_TEXTURE_2D, texture_globe);
        m_globe_textured->render(m_shader);

        m_shader->end();
    }
    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

draw_simple_objects_app my_app;       // default