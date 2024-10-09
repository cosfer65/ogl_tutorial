#include "lights_materials.h"
#include "cg_camera.h"
#include "cg_light.h"
#include "cg_util.h"
#include "cg_primitives.h"

using namespace atlas;

class atlas_app :public cg_app {
    cg_viewport* m_view;
    cg_camera* m_cam;
    cg_light* m_light;

    // the shaders
    cg_shader* m_shader;
    cg_shader* mat_shader;
    cg_shader* map_shader;

    // the objects
    cg_gl_cube* m_cube;
    cg_gl_sphere* m_sphere;

    // the materials
    cg_material* m_mat1;
    cg_material* m_mat2;
    cg_material* m_mat3;
    cg_material* m_mat4;
    cg_material* m_mat5;

    GLuint   diffuse_texture;
    GLuint   specular_texture;

public:
    atlas_app() {
        m_view = new cg_viewport();
        m_window.szTitle = "GusOnGames (lights and materials)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual int init_game() {
        m_view->set_fov(dtr(15));
        m_cam = new cg_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

#if 1
        m_light = new cg_light(cg_light::SPOTLIGHT);
        // light source located at (10,10,10)
        m_light->set_position(vec3(10, 10, 10));
#else
        m_light = new cg_light(cg_light::DIRLIGHT);
        // we are holding the light source and pointing at the objects
        m_light->set_direction(vec3(0, 0, -1));
#endif
        // allow some ambience for the light
        // white light (we can experiment with this)
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        // create the simple light shader
        m_shader = new cg_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.hlsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_fs.hlsl");
        m_shader->load();

        // create the material shader
        mat_shader = new cg_shader;
        mat_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.hlsl");
        mat_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_material_fs.hlsl");
        mat_shader->load();

        map_shader = new cg_shader;
        map_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.hlsl");
        map_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_map_fs.hlsl");
        map_shader->load();

        // the cube
        m_cube = new cg_gl_cube;
        m_cube->create(GL_FILL);
        // the sphere
        m_sphere = new cg_gl_sphere(0.4f);
        m_sphere->create(GL_FILL);

        // create the materials
        // gold
        m_mat1 = new cg_material;
        m_mat1->set_ambient(0.24725f, 0.1995f, 0.0745f);
        m_mat1->set_diffuse(0.75164f, 0.60648f, 0.22648f);
        m_mat1->set_specular(0.628281f, 0.555802f, 0.366065f);
        m_mat1->set_shine(0.4f * 128);
        // silver
        m_mat2 = new cg_material;
        m_mat2->set_ambient(0.19225f, 0.19225f, 0.19225f);
        m_mat2->set_diffuse(0.50754f, 0.50754f, 0.50754f);
        m_mat2->set_specular(0.508273f, 0.508273f, 0.508273f);
        m_mat2->set_shine(0.4f * 128);
        // bronze
        m_mat3 = new cg_material;
        m_mat3->set_ambient(0.2125f, 0.1275f, 0.054f);
        m_mat3->set_diffuse(0.714f, 0.4284f, 0.18144f);
        m_mat3->set_specular(0.393548f, 0.271906f, 0.166721f);
        m_mat3->set_shine(0.2f * 128);

        // the color is based on the image
        diffuse_texture = load_texture("resources/planks.tga");
        specular_texture = load_texture("resources/planks_specular.tga");
        m_mat4 = new cg_material;
        m_mat4->set_ambient(0.2f, 0.2f, 0.2f);
        // use image to paint object
        m_mat4->set_diffuse(diffuse_texture, 0);
        m_mat4->set_specular(specular_texture, 1);
        m_mat4->set_shine(0.52f * 128);

        // the color is based on the image
        diffuse_texture = load_texture("resources/earth.tga");
        specular_texture = load_texture("resources/earth_specular.tga");
        m_mat5 = new cg_material;
        m_mat5->set_ambient(0.2f, 0.2f, 0.2f);
        // use image to paint object
        m_mat5->set_diffuse(diffuse_texture, 0);  // use GL_TEXTURE0 for texture
        m_mat5->set_specular(specular_texture, 1);// use GL_TEXTURE1 for specular
        m_mat5->set_shine(0.1f * 128);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void frame_move(float fElapsed) {
        m_cube->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        m_sphere->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
    }

    virtual void frame_render() {
        // set the viewport to the whole window
        m_view->set_viewport();

        // clear screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_view->perspective() * m_cam->perspective();

        float xpos = -4.f;

        // objects with simple light
        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);
        m_shader->set_vec3("objectColor", vec3(0.07568f, 0.41424f, 0.07568f));
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(m_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(m_shader);

        // use materials
        mat_shader->use();
        m_light->apply(mat_shader);
        mat_shader->set_mat4("camera", cam_matrix);
        mat_shader->set_vec3("cameraPos", m_cam->vLocation);

        xpos += 2;

        // first material
        m_mat1->apply(mat_shader);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(mat_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(mat_shader);

        xpos += 2;

        // second material
        m_mat2->apply(mat_shader);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(mat_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(mat_shader);

        xpos += 2;

        // third material
        m_mat3->apply(mat_shader);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(mat_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(mat_shader);

        //////////////////////////////////////////////////////////////////////////
        //
        map_shader->use();
        m_light->apply(map_shader);
        map_shader->set_mat4("camera", cam_matrix);
        map_shader->set_vec3("cameraPos", m_cam->vLocation);

        m_mat4->apply(map_shader);
        xpos += 2;
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(map_shader);

        m_mat5->apply(map_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(map_shader);
    }

    virtual void exit_game() {
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

atlas_app my_app;       // default