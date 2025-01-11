#include "blending.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"
#include "font.h"

using namespace atlas;

static int source_mode = 6;  // set initial values for transparency modes (HUD)
static int dest_mode = 7;
static GLenum modes[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
};
const int g_modes = sizeof(modes) / sizeof(GLenum);
static std::string mode_names[] = {
    "GL_ZERO",
    "GL_ONE",
    "GL_SRC_COLOR",
    "GL_ONE_MINUS_SRC_COLOR",
    "GL_DST_COLOR",
    "GL_ONE_MINUS_DST_COLOR",
    "GL_SRC_ALPHA",
    "GL_ONE_MINUS_SRC_ALPHA",
    "GL_DST_ALPHA",
    "GL_ONE_MINUS_DST_ALPHA",
};

class c_background :public gl_prim {
protected:
public:
    c_background() {
    }
    virtual ~c_background() {
    }
    virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        draw_mode = GL_FILL;
        draw_elements = true;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;

            tmesh->addVertex(-1.f, -1.f, 0.f);
            tmesh->addVertex(1.f, -1.f, 0.f);
            tmesh->addVertex(1.f, 1.f, 0.f);
            tmesh->addVertex(-1.f, 1.f, 0.f);

            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);
            tmesh->addNormal(0, 0, -1);

            tmesh->addTexCoord(0, 0);
            tmesh->addTexCoord(1, 0);
            tmesh->addTexCoord(1, 1);
            tmesh->addTexCoord(0, 1);

            tmesh->addIndices(0, 1, 2);
            tmesh->addIndices(0, 2, 3);

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
};

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    gl_prim* m_cube;
    c_background* pglass;
    GLuint   texture;
    int draw_order;

    gl_font* font2D;
public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (blending)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(15));
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new gl_light(gl_light::DIRLIGHT);
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/blending_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/blending_fs.glsl");
        m_shader->load();

        m_cube = create_cube(GL_FILL);
        m_cube->move_to(vec3(0, 0, -2));
        m_cube->set_scale(vec3(1.5));

        pglass = new c_background;
        pglass->create();
        pglass->set_scale(vec3(2));

        texture = load_texture("resources/window.tga");

        draw_order = 0;
        // the following will be analyzed in the next chapter
        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);
        font2D->set_color(vec4(0));
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
        if (keyDown['N']) source_mode++;
        if (keyDown['M']) dest_mode++;
        if (keyDown['D']) draw_order = 1 - draw_order;
        source_mode = source_mode % g_modes;
        dest_mode = dest_mode % g_modes;
        keyDown['N'] = keyDown['M'] = keyDown['D'] = 0;
        m_cube->rotate_by(vec3(dtr(fElapsed * 15), dtr(fElapsed * 30), dtr(fElapsed * 45)));
    }
    virtual void render() {
        // set the viewport to the whole window
        m_view->set_viewport();
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);
        // set the texture we will use
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        // draw opaque objects first
        m_shader->set_vec4("objectColor", vec4(0, 1, 0, 1));
        // use object color parameter, do not look for texture
        m_shader->set_int("useColor", 1);
        m_cube->render(m_shader);

        // start blending
        glEnable(GL_BLEND);
        glBlendFunc(modes[source_mode], modes[dest_mode]);
        // and now transparent objects (sorted)
        if (draw_order == 0)
        {
            // ignore object color and use texture
            m_shader->set_int("useColor", 0);
            pglass->move_to(vec3(0, 0, -1.5f));
            pglass->render(m_shader);

            m_shader->set_vec4("objectColor", vec4(.9f, .1f, .1f, 0.5f));
            m_shader->set_int("useColor", 1);
            pglass->move_to(vec3(0, 0, 0));
            pglass->render(m_shader);
        }
        else
        {
            m_shader->set_vec4("objectColor", vec4(.9f, .1f, .1f, 0.5f));
            m_shader->set_int("useColor", 1);
            pglass->move_to(vec3(0, 0, -1.5f));
            pglass->render(m_shader);

            m_shader->set_int("useColor", 0);
            pglass->move_to(vec3(0, 0, 0));
            pglass->render(m_shader);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        // stop blending
        glDisable(GL_BLEND);

        glUseProgram(0);

        // the following will be analyzed in the next chapter
        font2D->set_position(5, 35);
        font2D->render("source:%s, destination:%s", mode_names[source_mode].c_str(), mode_names[dest_mode].c_str());
        font2D->set_position(5, 20);
        font2D->render("press M/N to change draw mode");
        font2D->set_position(5, 5);
        font2D->render("press D to change draw order");
    }
};

atlas_app my_app;       // default