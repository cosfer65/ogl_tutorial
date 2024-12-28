#include "shadow_mapping.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"
#include "font.h"
#include "buffers.h"
#include "gl_math.h"

using namespace atlas;

class atlas_app :public c_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    c_light* m_light;
    c_shader* m_shader;

    gl_prim* m_cube;
    gl_prim* m_cube2;
    gl_prim* m_cube3;
    gl_prim* m_ground;
    gl_prim* m_sphere;

    c_shader* depth_shader;
    gl_depth_buffer* d_buffer;

    gl_font* font2D;
public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "shadow mapping";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(15));
        m_cam = new gl_camera(vec3(-5, 10, 30), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new c_light(c_light::DIRLIGHT);
        m_light->set_position(vec3(-3, 5, 0));

        m_shader = new c_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shadow_mapping_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shadow_mapping_fs.glsl");
        m_shader->load();

        depth_shader = new c_shader;
        depth_shader->add_file(GL_VERTEX_SHADER, "resources/depth_vs.glsl");
        //depth_shader->add_file(GL_FRAGMENT_SHADER, "resources/depth_fs.glsl");
        depth_shader->load();

        m_cube = create_cube(GL_FILL, true);
        m_cube->set_scale(vec3(1));
        m_cube->move_to(vec3(-1.5f, 1.5f, 0));

        m_cube2 = create_cube(GL_FILL, true);
        m_cube2->set_scale(vec3(0.75));
        m_cube2->move_to(vec3(2.f, 1.5f, 0.75f));

        m_cube3 = create_cube(GL_FILL, true);
        m_cube3->move_to(vec3(0, .5f, 0));

        m_sphere = create_sphere(GL_FILL, true);
        m_sphere->set_scale(.5f);
        m_sphere->move_to(vec3(-2.2f, 2.8f, 0));

        m_ground = create_plane(GL_FILL, true);
        m_ground->set_scale(7.5f);

        d_buffer = new gl_depth_buffer;
        d_buffer->create(1024, 1024);

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
        float d15 = dtr(15);
        float d30 = dtr(30);
        float d45 = dtr(45);
        m_cube->rotate_by(vec3(fElapsed * d15, fElapsed * d30, fElapsed * d45));
        m_cube2->rotate_by(vec3(fElapsed * d45, -fElapsed * d30, fElapsed * d15));
    }
    void render_scene(c_shader* shader) {
        shader->set_vec3("objectColor", vec3(.4f, .65f, .9f));
        m_sphere->render(shader);
        m_cube->render(shader);
        m_cube2->render(shader);

        shader->set_vec3("objectColor", vec3(.75f, .65f, .9f));
        m_cube3->render(shader);

        shader->set_vec3("objectColor", vec3(.75f, .65f, .1f));
        m_ground->render(shader);
    }

    virtual void render()
    {
        // create the depth buffer to use in shadow calculations
        mat4 l_projection(ortho(-10, 10, -10, 10, 0.1f, 70.f));
        mat4 l_view(lookAt(m_light->get_position(), vec3(0.0f), vec3(0.0, 1.0, 0.0)));
        mat4 lightSpaceMatrix(l_view * l_projection);
        // render scene from light's point of view
        depth_shader->use();
        depth_shader->set_mat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, 1024, 1024);
        d_buffer->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        render_scene(depth_shader);
        glCullFace(GL_BACK);
        d_buffer->release();

        // -----------------------------------------------------------------------------------------
        // set the viewport to the whole window
        m_view->set_viewport();
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);


        // combine the view and camera matrices into one
        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

        // enable the shader
        m_shader->use();
        // set the combined view matrix
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_mat4("lightSpaceMatrix", lightSpaceMatrix); // pass the shadow matrix

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, d_buffer->texture());
        m_shader->set_int("shadowMap", 0);
        render_scene(m_shader);
        glUseProgram(0);

        // the following will be analyzed in the next chapter
        font2D->set_position(5, 5);
        font2D->render("press Esc to exit");
    }
};

atlas_app my_app;       // default