#include "frame_buffers.h"
#include "cg_camera.h"
#include "cg_light.h"
#include "cg_util.h"
#include "cg_primitives.h"
#include "cg_font.h"
#include "cg_buffers.h"

using namespace atlas;

class atlas_app :public cg_app {
    cg_texture_buffer* m_tex_buffer; // the texture buffer
    cg_viewport* fb_view;            // the viewport to the texture image
    cg_shader* m_shader_create;      // the shader to create the texture
    cg_gl_cube* m_cube;              // rotating cube for the texture

    cg_viewport* m_view;             // normal view
    cg_shader* m_shader_use;         // shader for the final render
    cg_gl_sphere* m_sphere;          // the object to use the texture

    cg_camera* m_cam;                // shared camer
    cg_light* m_light;               // shared light

    cg_font* font2D;
public:
    atlas_app() {
        m_view = new cg_viewport();
        m_window.szTitle = "GusOnGames (frame buffers)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_game() {
        m_view->set_fov(dtr(10));
        m_cam = new cg_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new cg_light(cg_light::DIRLIGHT);
        m_light->set_position(vec3(10, 10, 10));
        m_light->set_ambient(vec3(0.25f));

        // create frame buffer view and texture
        fb_view = new cg_viewport();
        fb_view->set_view_field(1, 1000);
        fb_view->set_fov(dtr(12));
        fb_view->set_window_aspect(512, 512);
        m_tex_buffer = new cg_texture_buffer();
        m_tex_buffer->create(512, 512);

        m_shader_create = new cg_shader;
        m_shader_create->add_file(GL_VERTEX_SHADER, "resources/frame_buffers_vs.hlsl");
        m_shader_create->add_file(GL_FRAGMENT_SHADER, "resources/frame_buffers_create_fs.hlsl");
        m_shader_create->load();

        m_shader_use = new cg_shader;
        m_shader_use->add_file(GL_VERTEX_SHADER, "resources/frame_buffers_vs.hlsl");
        m_shader_use->add_file(GL_FRAGMENT_SHADER, "resources/frame_buffers_use_fs.hlsl");
        m_shader_use->load();

        m_cube = new cg_gl_cube;
        m_cube->create(GL_FILL, true);
        m_cube->move_to(vec3(0, 0, -2));
        //m_cube->set_scale(vec3(1.5));

        m_sphere = new cg_gl_sphere(1);
        m_sphere->create(GL_FILL, true);

        // the following will be analyzed in the next chapter
        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);
        font2D->set_color(vec4(1));

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
        m_sphere->rotate_by(vec3(0, dtr(fElapsed * 30), 0));
    }
    virtual void frame_render() {
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
        mat4 cam_matrix = fb_view->perspective() * m_cam->perspective();
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
        cam_matrix = m_view->perspective() * m_cam->perspective();
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