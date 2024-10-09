#include "fonts.h"
#include "cg_camera.h"
#include "cg_light.h"
#include "cg_util.h"
#include "cg_primitives.h"
#include "cg_font.h"
#include "cg_buffers.h"

using namespace atlas;

class atlas_app :public cg_app {
    cg_viewport* m_view;
    cg_camera* m_cam;
    cg_light* m_light;
    cg_shader* m_shader;

    cg_font* font3D;
    cg_font* font2D;
public:
    atlas_app() {
        m_view = new cg_viewport();
        m_window.szTitle = "GusOnGames (text rendering)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_game() {
        m_view->set_fov(dtr(15));
        m_cam = new cg_camera(vec3(0, 5, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new cg_light(cg_light::SPOTLIGHT);
        m_light->set_position(vec3(-30, 30, 30));

        m_shader = new cg_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/fonts_vs.hlsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/fonts_fs.hlsl");
        m_shader->load();

        font3D = get_font_manager().create_font("Bookman3D", "Bookman Old Style", 20, .2f);
        font2D = get_font_manager().create_font("Consolas", "Consolas", 24);
        font2D->set_color(vec4(1, 0, 0.5f, 1));
        font2D->set_position(5, 7);

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
        float d15 = dtr(15);
        float d30 = dtr(30);
        float d45 = dtr(45);
        font3D->rotate_by(vec3(fElapsed * d15, fElapsed * d30, fElapsed * d45));
    }

    virtual void frame_render()
    {
        // -----------------------------------------------------------------------------------------
        // set the viewport to the whole window
        m_view->set_viewport();
        glClearColor(0.5f, 0.5f, 0.5f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);

        mat4 projection = m_view->perspective();
        mat4 view = m_cam->perspective();
        m_shader->set_mat4("projection", projection);
        m_shader->set_mat4("view", view);

        m_shader->set_vec4("objectColor", vec4(.1f, .1f, .9f, 1.f));
        font3D->render(m_shader, ALIGN_CENTER, "GusOnGames");
        m_shader->end();

        font2D->render("press Esc to exit");

        glUseProgram(0);
    }
};

atlas_app my_app;       // default