#include "visual_effects.h"
#include "world_objects.h"
#include "camera.h"
#include "light.h"
#include "effects.h"

using namespace atlas;

class visual_effects :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;
    gl_fog* m_fog;

    skybox* m_skybox;
    ground* m_ground;

    std::vector<building*> buildings;
    std::vector<pavement*> pavements;
public:
    visual_effects() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (visual effects)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual ~visual_effects() {
        delete m_view;
        delete m_cam;
        delete m_light;
        delete m_shader;
        delete m_skybox;
        delete m_ground;
        for (auto p : pavements)
            delete p;
        for (auto b : buildings)
            delete b;
    }

    void create_environment() {
        // skybox
        m_skybox = new skybox;
        m_skybox->create();

        // ground
        m_ground = new ground;
        m_ground->create();

        // buildings and pavements
        int idx = 0;
        for (int row = -1; row < 2; row += 2) {
            for (int col = -5; col < 6; col += 2) {
                float xpos = row * 14.f;
                float zpos = col * 14.f;
                building* build1 = new building(idx);
                build1->create();
                build1->set_scale(vec3(12, 12, 12));

                build1->move_to(xpos, 6.1f, zpos);
                buildings.push_back(build1);
                idx++;

                pavement* p = new pavement;
                p->create();
                p->set_scale(vec3(15, 0.1f, 15));

                p->move_to(xpos, .025f, zpos);
                pavements.push_back(p);
            }
            ++idx;
        }
    }

    virtual int init_application() {
        m_view->set_fov(dtr(30));
        m_cam = new gl_camera(vec3(0, 2, 100), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-100, 1000, 1000));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 0, 1));

        m_fog = new gl_fog;
        m_fog->set_color(vec3(1, 1, 1));
        m_fog->set_linear(0, 200);

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/generic_VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/car_control_FragmentShader.glsl");
        m_shader->load();

        create_environment();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 0;
    }
    virtual void render() {
        m_view->set_viewport();
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

        m_skybox->render_with_effects(m_fog, m_view, m_cam);

        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        m_fog->apply(m_shader);

        m_shader->set_int("use_color_or_texture", 1);
        m_ground->render(m_shader);

        for (auto p : pavements)
            p->render(m_shader);

        m_shader->set_int("use_color_or_texture", 0);
        glActiveTexture(GL_TEXTURE0);
        for (auto build1 : buildings)
            build1->render(m_shader);

        m_shader->set_int("use_color_or_texture", 1);

        m_shader->end();
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

visual_effects my_app;       // default