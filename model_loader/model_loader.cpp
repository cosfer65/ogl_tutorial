#include "model_loader.h"
#include "3d_models.h"
#include "camera.h"
#include "light.h"
#include "primitives.h"
#include "font.h"

using namespace atlas;

//std::string model_name("resources/artificial_horizon.stl");
//std::string model_name("resources/artificial_horizon.obj");
//std::string model_name("resources/car_01.obj");
//std::string model_name("resources/rafale.off");
std::string model_name("d:/3d_models/cone.obj");


float fscale = 1.f;

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;

    gl_shader* m_shader;

    gl_model* model1;
    float fov = 15.f;

    gl_font* font2D;
    gl_prim* m_ucs;
public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (model loader)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(fov));
        // m_cam = new gl_camera(vec3(-5, 10, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        m_cam = new gl_camera(vec3(0, 0, 50), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new gl_light(gl_light::SPOTLIGHT);
        // we are holding the light source and pointing at the objects
        m_light->set_position(vec3(-100, 0, 0));
        // allow some ambience for the light
        // white light (we can experiment with this)
        m_light->set_ambient(vec3(1));
        m_light->set_diffuse(vec3(1));
        m_light->set_specular(vec3(1));

        // create the simple light shader
        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/model_loader_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/model_loader_fs.glsl");
        m_shader->load();

        model1 = new gl_model();
        model1->load(model_name); //--------------------------------------
        model1->set_scale(vec3(fscale));

        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);

        m_ucs = create_UCS();
        m_ucs->set_scale(vec3(2));

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void onMouseMove(int dx, int dy, WPARAM extra_btn) {
        if (extra_btn & MK_LBUTTON) {
            model1->rotate_by(dtr(dy / 5.f), dtr(dx / 5.f), 0);
            m_ucs->rotate_by(dtr(dy / 5.f), dtr(dx / 5.f), 0);
        }
        if (extra_btn & MK_RBUTTON) {
            model1->rotate_by(0, 0, dtr(dy / 5.f));
            m_ucs->rotate_by(0, 0, dtr(dy / 5.f));
        }
    }

    virtual void onMouseWheel(int delta, WPARAM extra_btn) {
        if (delta > 0)
            fscale -= 0.05f;
        if (delta < 0)
            fscale += 0.05f;
        if (fscale < 0.05f)
            fov = 0.05f;
        model1->set_scale(vec3(fscale));
    }

    virtual void step_simulation(float fElapsed) {
        if (keyDown['T']) {
            base_3d_model::inv_trias = 1 - base_3d_model::inv_trias;
            keyDown['T'] = 0;
            model1->load(model_name); //--------------------------------------
            model1->set_scale(vec3(fscale));
        }
        if (keyDown['N']) {
            base_3d_model::inv_norms = 1 - base_3d_model::inv_norms;
            keyDown['N'] = 0;
            model1->load(model_name); //--------------------------------------
            model1->set_scale(vec3(fscale));
        }
    }

    virtual void render() {
        // set the viewport to the whole window
        m_view->set_viewport();

        // clear screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

        float xpos = 0;

        // objects with simple light
        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        model1->render(m_shader);

        m_shader->set_vec3("objectColor", vec3(1));
        m_ucs->move_to(-8, 0, 0);
        m_ucs->render(m_shader);

        m_shader->end();

        font2D->set_color(vec4(1));

        {
            char txt[100];
            sprintf(txt, "T:%d, N:%d", base_3d_model::inv_trias, base_3d_model::inv_norms);
            font2D->set_position(5, 80);
            font2D->render(txt);
        }

        font2D->set_position(5, 65);
        font2D->render("T to set CCW");
        font2D->set_position(5, 50);
        font2D->render("N to invert normal");

        font2D->set_position(5, 35);
        font2D->render("Left mouse button and drag to rotate around X/Y axes");
        font2D->set_position(5, 20);
        font2D->render("Right mouse button and drag to rotate around Z axes");
        font2D->set_position(5, 5);
        font2D->render("Mouse wheel to zoom");
    }

    virtual void exit_application() {
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

atlas_app my_app;       // default