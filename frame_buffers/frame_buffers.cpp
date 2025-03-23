#include "frame_buffers.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"
#include "font.h"
#include "buffers.h"

using namespace atlas;

class frame_buffers_app :public gl_application {
    gl_texture_buffer* m_tex_buffer; // the texture buffer
    gl_viewport* fb_view;            // the viewport to the texture image
    gl_shader* m_shader;             // the shader to draw
    gl_prim* m_cube;                 // rotating cube for the texture

    gl_viewport* m_view;             // normal view
    gl_prim* m_sphere;               // the object to use the texture

    gl_camera* m_cam;                // shared camer
    gl_light* m_light;               // shared light

    gl_font* font2D;
public:
    frame_buffers_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (frame buffers)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual ~frame_buffers_app() {
        delete m_view;
        delete m_cam;
        delete m_light;
        delete m_shader;
        delete m_cube;
        delete m_sphere;
        delete font2D;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(10));
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new gl_light(gl_light::DIRLIGHT);
        m_light->set_position(vec3(10, 10, 10));
        m_light->set_ambient(vec3(0.25f));

        // create frame buffer view and texture
        fb_view = new gl_viewport();
        fb_view->set_view_field(1, 1000);
        fb_view->set_fov(dtr(12));
        fb_view->set_window_aspect(512, 512);
        m_tex_buffer = new gl_texture_buffer();
        m_tex_buffer->create(512, 512);

        // shader
        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/generic_VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/frame_buffers_FragmentShader.glsl");
        m_shader->load();

        m_cube = create_cube(GL_FILL, true);
        m_cube->move_to(vec3(0, 0, -2));

        m_sphere = create_sphere(GL_FILL, true);
        m_sphere->set_scale(vec3(2));

        // the following will be analyzed in the next chapter
        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);
        font2D->set_color(vec4(1));

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
        m_sphere->rotate_by(vec3(0, dtr(fElapsed * 30), 0));
    }

    virtual void render() {
        // we will draw on an off-screen image, which we will use
        // as a texture for the object we will draw later

        // here we go
        // first we 'redirect' our rendering to the texture buffer
        m_tex_buffer->bind();
        // and we get on with rendering as usual
        glEnable(GL_DEPTH_TEST);
        // set the viewport to the image size
        fb_view->set_viewport();
        // clear the screen
        glClearColor(0.5f, 0.5f, 0.9f, 0.9f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);

        m_shader->use();
        m_light->apply(m_shader);
        mat4 cam_matrix = m_cam->perspective() * fb_view->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);
        m_shader->set_vec4("object_color", vec4(.9f, .5f, .1f, 1));
        m_shader->set_int("use_color_or_texture", 1);
        m_cube->render(m_shader);
        // and stop redirection
        m_tex_buffer->release();

        // and now we use the image as texture and we draw the cube again
        glEnable(GL_DEPTH_TEST);
        // set the viewport to the whole window
        m_view->set_viewport();
        // clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);

        m_light->apply(m_shader);
        cam_matrix = m_cam->perspective() * m_view->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);
        m_shader->set_int("use_color_or_texture", 0);
        // set the texture we created
        glBindTexture(GL_TEXTURE_2D, m_tex_buffer->texture());
        m_sphere->render(m_shader);

        m_shader->end();

        font2D->set_position(5, 5);
        font2D->render("press Esc to exit");
    }
};


frame_buffers_app my_app;       // default