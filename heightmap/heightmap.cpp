#include "heightmap.h"
#include "world_objects.h"
#include "matrix.h"
#include "camera.h"
#include "light.h"
#include "effects.h"
#include "gl_image.h"

using namespace atlas;

class heightmap :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    gl_prim* m_hmap;
    gl_prim* m_plane;
    float fscale = 1.f;
public:
    heightmap() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (model loader)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual ~heightmap() {
        delete m_view;
        delete m_cam;
        delete m_light;
        delete m_shader;
        delete m_hmap;
    }

    virtual int init_application() {
        m_view->set_fov(dtr(30));
        m_cam = new gl_camera(vec3(0, 30, 300), vec3(0, 0, 0), vec3(0, 1, 0));

        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-100, 10, 100));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 0, 1));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/generic_VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/dso_FragmentShader.glsl");
        m_shader->load();

        m_hmap = create_heightmap("resources/textures/heightmap.tga");// , GL_LINE, true);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 0;
    }

    virtual void onMouseMove(int dx, int dy, WPARAM extra_btn) {
        if (extra_btn & MK_LBUTTON) {
            m_hmap->rotate_by(dtr(dy / 5.f), dtr(dx / 5.f), 0);
        }
        if (extra_btn & MK_RBUTTON) {
            m_hmap->rotate_by(0, 0, dtr(dy / 5.f));
        }
    }

    virtual void onMouseWheel(int delta, WPARAM extra_btn) {
        if (delta > 0)
            fscale -= 0.05f;
        if (delta < 0)
            fscale += 0.05f;
        m_hmap->set_scale(vec3(fscale));
    }


    virtual void render() {
        m_view->set_viewport();
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        // m_shader->set_vec4("object_color", vec4(0.3f, 0.5f, 0.9f, 1.f));
        // m_shader->set_int("use_color_or_texture", 0);

        m_hmap->render(m_shader);

        m_shader->end();
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

heightmap my_app;       // default