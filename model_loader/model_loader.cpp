#include "model_loader.h"
#include "camera.h"
#include "light.h"
#include "primitives.h"

#include "3d_models.h"

using namespace atlas;

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;

    gl_shader* m_shader;

    gl_model* model1;

public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (lights and materials)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(15));
        m_cam = new gl_camera(vec3(-5, 10, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new gl_light(gl_light::DIRLIGHT);
        // we are holding the light source and pointing at the objects
        m_light->set_direction(vec3(0, 0, -1));
        // allow some ambience for the light
        // white light (we can experiment with this)
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        // create the simple light shader
        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.glsl");
        //m_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_material_fs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_fs.glsl");
        m_shader->load();

        model1 = new gl_model();
        model1->load_model("resources/car_01.obj", ivec3(1, 1, -1));
        model1->set_scale(vec3(0.2f, 0.2f, 0.2f));
        // model1->move_to(vec3(1, -1, 2));

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void step_simulation(float fElapsed) {
        // model1->rotate_by(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30));
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
        m_shader->set_vec3("objectColor", vec3(0.07568f, 0.01424f, 0.7568f));

        
        model1->render(m_shader);
    }

    virtual void exit_application() {
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

atlas_app my_app;       // default