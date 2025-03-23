#include "fonts.h"
#include "camera.h"
#include "light.h"
#include "primitives.h"
#include "font.h"

using namespace atlas;

class fonts_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    gl_font* font3D;
    gl_font* font2D;
public:
    fonts_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (text rendering)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual ~fonts_app() {
        delete m_view;
        delete m_cam;
        delete m_light;
        delete m_shader;
        delete font3D;
        delete font2D;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(15));
        m_cam = new gl_camera(vec3(0, 0, 25), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new gl_light(gl_light::DIRLIGHT);
        m_light->set_position(vec3(-100, 0, 0));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/generic_VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/fonts_FragmentShader.glsl");
        m_shader->load();

        font3D = get_font_manager().create_font("Bookman3D", "Bookman Old Style", 20, .2f);
        //font3D->set_scale(2);

        font2D = get_font_manager().create_font("Consolas", "Consolas", 24);
        font2D->set_color(vec4(1, 0, 0.5f, 1));
        font2D->set_position(5, 7);

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
        font3D->rotate_by(vec3(fElapsed * d15, fElapsed * d30, fElapsed * d45));
    }

    virtual void render()
    {
        // -----------------------------------------------------------------------------------------
        // set the viewport to the whole window
        m_view->set_viewport();
        glClearColor(0.5f, 0.5f, 0.5f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        // render the objects using color
        m_shader->set_int("use_color_or_texture", 1);
        m_shader->set_vec4("object_color", vec4(.1f, .25f, .9f, 1.f));
        font3D->render(m_shader, ALIGN_CENTER, "GusOnGames");
        m_shader->end();

        font2D->render("press Esc to exit");

        glUseProgram(0);
    }
};

fonts_app my_app;       // default
