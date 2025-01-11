#include "cubemap.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"
#include "font.h"
#include "skybox.h"

using namespace atlas;

class atlas_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_shader* m_shader;
    gl_skybox* m_skybox;

    gl_prim* m_sphere;
    gl_prim* m_cube;

    gl_font* font2D;
public:
    atlas_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (cubemap / skybox)";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(30));
        // m_cam = new gl_camera(vec3(0, 10, 30), vec3(0, 0, 0), vec3(0, 1, 0));
        m_cam = new gl_camera(vec3(0, 0, -20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_skybox = new gl_skybox;
        std::vector<std::string> faces
        {
            "resources/px.tga", "resources/nx.tga",
            "resources/py.tga", "resources/ny.tga",
            "resources/pz.tga", "resources/nz.tga"
        };

        m_skybox->load(faces);

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/cubemap_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/cubemap_fs.glsl");
        m_shader->load();

        m_sphere = create_sphere(GL_FILL, true);
        m_sphere->set_scale(vec3(3));
        m_sphere->move_to(vec3(2.5, 0, 0));

        m_cube = create_cube(GL_FILL, true);
        m_cube->set_scale(vec3(3));
        m_cube->move_to(vec3(-2.5, 0, 0));

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

        if (keyDown['M']) m_cam->rotate_locY(-d30 * fElapsed);//turn right
        if (keyDown['N']) m_cam->rotate_locY(d30 * fElapsed);//turn left

        if (keyDown['A']) m_cam->rotate_locX(d30 * fElapsed);//turn up
        if (keyDown['Z']) m_cam->rotate_locX(-d30 * fElapsed);//turn down

        if (keyDown['S']) m_cam->rotate_locZ(d30 * fElapsed);//rotate left
        if (keyDown['D']) m_cam->rotate_locZ(-d30 * fElapsed);//rotate right
        m_cube->rotate_by(vec3(d15 * fElapsed / 5, d30 * fElapsed / 5, 0));
    }
    virtual void render() {
        // set the viewport to the whole window
        m_view->set_viewport();
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);
        // we use 1 and GL_TEXTURE1 for demonstration purposes only
        m_shader->set_int("skybox", 1);
        glActiveTexture(GL_TEXTURE1);
        // use cubemap texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox->texture());
        // notify shader to use the texture when rendering
        m_shader->set_int("useColor", 0);
        // render our scene
        m_sphere->render(m_shader);
        m_cube->render(m_shader);

        // render the skybox
        m_skybox->render(m_view, m_cam);

        // notify shader to use color rendering
        m_shader->end();

        // the following will be analyzed in the next chapter
        font2D->set_position(5, 38);
        font2D->render("A-Z look up/down");
        font2D->set_position(5, 27);
        font2D->render("N-M look left/right");
        font2D->set_position(5, 16);
        font2D->render("S-D rotate left/right");
        font2D->set_position(5, 5);
        font2D->render("press Esc to exit");
    }
};

atlas_app my_app;       // default