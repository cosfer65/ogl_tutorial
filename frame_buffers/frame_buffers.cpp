#include "frame_buffers.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"
#include "font.h"
#include "buffers.h"

using namespace atlas;

class atlas_app :public c_application {
    gl_texture_buffer* m_tex_buffer; // the texture buffer
    gl_viewport* fb_view;            // the viewport to the texture image
    c_shader* m_shader_create;      // the shader to create the texture
    gl_prim* m_cube;              // rotating cube for the texture

    gl_viewport* m_view;             // normal view
    c_shader* m_shader_use;         // shader for the final render
    gl_prim* m_sphere;          // the object to use the texture

    gl_camera* m_cam;                // shared camer
    c_light* m_light;               // shared light

    gl_font* font2D;
public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (frame buffers)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(10));
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new c_light(c_light::DIRLIGHT);
        m_light->set_position(vec3(10, 10, 10));
        m_light->set_ambient(vec3(0.25f));

        // create frame buffer view and texture
        fb_view = new gl_viewport();
        fb_view->set_view_field(1, 1000);
        fb_view->set_fov(dtr(12));
        fb_view->set_window_aspect(512, 512);
        m_tex_buffer = new gl_texture_buffer();
        m_tex_buffer->create(512, 512);

        m_shader_create = new c_shader;
        m_shader_create->add_file(GL_VERTEX_SHADER, "resources/frame_buffers_vs.glsl");
        m_shader_create->add_file(GL_FRAGMENT_SHADER, "resources/frame_buffers_create_fs.glsl");
        m_shader_create->load();

        m_shader_use = new c_shader;
        m_shader_use->add_file(GL_VERTEX_SHADER, "resources/frame_buffers_vs.glsl");
        m_shader_use->add_file(GL_FRAGMENT_SHADER, "resources/frame_buffers_use_fs.glsl");
        m_shader_use->load();

        m_cube = create_cube(GL_FILL, true);
        m_cube->move_to(vec3(0, 0, -2));
        //m_cube->set_scale(vec3(1.5));

        m_sphere = create_sphere(GL_FILL, true);

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
        glClearColor(0.7f, 0.6f, 0.5f, 0.4f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);
        m_shader_create->use();
        m_light->apply(m_shader_create);
        mat4 cam_matrix = m_cam->perspective() * fb_view->perspective();
        m_shader_create->set_mat4("camera", cam_matrix);
        m_shader_create->set_vec3("cameraPos", m_cam->vLocation);
        m_shader_create->set_vec3("objectColor", vec3(.4f, .9f, .9f));
        m_cube->render(m_shader_create);
        glUseProgram(0);
        // and finally we stop redirection
        m_tex_buffer->release();

        // and now we use the image as texture and we draw the cube again
        glEnable(GL_DEPTH_TEST);
        // set the viewport to the whole window
        m_view->set_viewport();
        // clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);
        m_shader_use->use();
        m_light->apply(m_shader_use);
        cam_matrix = m_cam->perspective() * m_view->perspective();
        m_shader_use->set_mat4("camera", cam_matrix);
        m_shader_use->set_vec3("cameraPos", m_cam->vLocation);
        m_shader_use->set_int("useColor", 0);
        // set the texture we created
        glBindTexture(GL_TEXTURE_2D, m_tex_buffer->texture());
        m_sphere->render(m_shader_use);
        // release shader
        glUseProgram(0);

        // the following will be analyzed in the next chapter
        m_shader_use->set_int("useColor", 1);
        m_shader_use->set_vec4("objectColor", vec4(.9f, .1f, .1f, 1));
        glUseProgram(0);
        font2D->set_position(5, 5);
        font2D->render("press Esc to exit");
    }
};

atlas_app my_app;       // default