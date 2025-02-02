#include "model_loader.h"
#include "3d_models.h"
#include "camera.h"
#include "light.h"
#include "primitives.h"
#include "font.h"

using namespace atlas;

//std::string model_name("resources/car_01.obj");
//std::string model_name("resources/rafale.stl");
std::string model_name("resources/homer_out.off");
//std::string model_name("resources/ico_sps.off");

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;

    gl_shader* m_shader;

    gl_model* model1;
    float fov = 15.f;

    gl_font* font2D;
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
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new gl_light(gl_light::SPOTLIGHT);
        // we are holding the light source and pointing at the objects
        m_light->set_position(vec3(10, 10, 10));
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
        model1->load(model_name); 
        //model1->set_scale(vec3(0.5f, 0.5f, 0.5f));

        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void onMouseMove(int dx, int dy, WPARAM extra_btn) {
        if (extra_btn & MK_LBUTTON)
            model1->rotate_by(dtr(dy / 5.f), dtr(dx / 5.f), 0);
        if (extra_btn & MK_RBUTTON)
            model1->rotate_by(0, 0, dtr(dy / 5.f));
    };
    virtual void onMouseWheel(int delta, WPARAM extra_btn) {
        if (delta > 0)
            fov += 1.f;
        if (delta < 0)
            fov -= 1.f;
        if (fov > 160.f)
            fov = 160.f;
        if (fov < 4.f)
            fov = 4.f;
        m_view->set_fov(dtr(fov));
    };

    virtual void step_simulation(float fElapsed) {
        bool reload = false;
        if (keyDown['R']) {
            base_3d_model::inv_x = 1;
            base_3d_model::inv_y = 1;
            base_3d_model::inv_z = 1;
            base_3d_model::flip_xy = 0;
            base_3d_model::flip_xz = 0;
            base_3d_model::flip_yz = 0;
            reload = true;
            keyDown['R'] = 0;
        }
        if (keyDown['S']) {
            // model1->save("resources/model.stl");
            keyDown['S'] = 0;
            reload = true;
        }
        if (keyDown['X']) {
            base_3d_model::inv_x *= -1;
            keyDown['X'] = 0;
            reload = true;
        }
        if (keyDown['Y']) {
            base_3d_model::inv_y *= -1;
            keyDown['Y'] = 0;
            reload = true;
        }
        if (keyDown['Z']) {
            base_3d_model::inv_z *= -1;
            keyDown['Z'] = 0;
            reload = true;
        }
        if (keyDown['1']) {
            base_3d_model::flip_xy = 1 - base_3d_model::flip_xy;
            base_3d_model::flip_xz = 0;
            base_3d_model::flip_yz = 0;
            keyDown['1'] = 0;
            reload = true;
        }
        if (keyDown['2']) {
            base_3d_model::flip_xz = 1 - base_3d_model::flip_xz;
            base_3d_model::flip_xy = 0;
            base_3d_model::flip_yz = 0;
            keyDown['2'] = 0;
            reload = true;
        }
        if (keyDown['3']) {
            base_3d_model::flip_yz = 1 - base_3d_model::flip_yz;
            base_3d_model::flip_xy = 0;
            base_3d_model::flip_xz = 0;
            keyDown['3'] = 0;
            reload = true;
        }
        if (reload) {
            model1->load(model_name);
            // model1->set_scale(vec3(0.2f, 0.2f, 0.2f));
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

        m_shader->end();

        font2D->set_color(vec4(1));

        {
            char txt[100];
            sprintf(txt, "x:%d, y:%d, z:%d, xy:%d, xz%d, yz:%d", 
                base_3d_model::inv_x, base_3d_model::inv_y, base_3d_model::inv_z, 
                base_3d_model::flip_xy, base_3d_model::flip_xz, base_3d_model::flip_yz);
            font2D->set_position(5, 110);
            font2D->render(txt);
        }

        font2D->set_position(5, 95);
        font2D->render("S to save model");
        font2D->set_position(5, 80);
        font2D->render("R to reset model");
        font2D->set_position(5, 65);
        font2D->render("X/Y/Z to invert axis");
        font2D->set_position(5, 50);
        font2D->render("flips: 1-yz, 2-xy, 3-xz");

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