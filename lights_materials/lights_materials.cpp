#include "lights_materials.h"
#include "camera.h"
#include "light.h"
#include "material.h"
#include "util.h"
#include "primitives.h"

using namespace atlas;

class lights_materials_app :public gl_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    gl_light* m_light;

    // the shader
    gl_shader* m_shader;

    // the objects
    gl_prim* m_cube;
    gl_prim* m_sphere;

    // the materials
    cg_material* m_mat1;
    cg_material* m_mat2;
    cg_material* m_mat3;
    cg_material* m_mat4;
    cg_material* m_mat5;

public:
    lights_materials_app() {
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (lights and materials)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 750;
    }
    virtual ~lights_materials_app() {
        delete m_view;
        delete m_cam;
        delete m_light;
        delete m_shader;
        delete m_cube;
        delete m_sphere;
        delete m_mat1;
        delete m_mat2;
        delete m_mat3;
        delete m_mat4;
        delete m_mat5;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(15));
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

#if 1
        m_light = new gl_light(gl_light::SPOTLIGHT);
        // light source located at (10,10,10)
        m_light->set_position(vec3(10, 10, 10));
#else
        m_light = new gl_light(gl_light::DIRLIGHT);
        // we are holding the light source and pointing at the objects
        m_light->set_direction(vec3(0, 0, -1));
#endif
        // allow some ambience for the light
        // white light (we can experiment with this)
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        // create the shader
        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/generic_VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/lights_materials_FragmentShader.glsl");
        m_shader->load();

        // the cube
        m_cube = create_cube(GL_FILL);
        // the sphere
        m_sphere = create_sphere(GL_FILL);

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

        GLuint   diffuse_texture;
        GLuint   specular_texture;

        // the color is based on the image
        diffuse_texture = load_texture("resources/textures/planks.tga");
        specular_texture = load_texture("resources/textures/planks_specular.tga");
        m_mat4 = new cg_material;
        m_mat4->set_ambient(0.2f, 0.2f, 0.2f);
        // use image to paint object
        m_mat4->set_diffuse(diffuse_texture, 0);
        m_mat4->set_specular(specular_texture, 1);
        m_mat4->set_shine(0.52f * 128);

        // the color is based on the image
        diffuse_texture = load_texture("resources/textures/earth.tga");
        specular_texture = load_texture("resources/textures/earth_specular.tga");
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

    virtual void step_simulation(float fElapsed) {
        m_cube->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        m_sphere->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
    }

    virtual void render() {
        // set the viewport to the whole window
        m_view->set_viewport();

        // clear screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

        float xpos = -4.f;

        // objects with simple light
        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);
        m_shader->set_vec4("object_color", vec4(0.07568f, 0.41424f, 0.07568f, 1));
        m_shader->set_int("coloring_type", 0);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(m_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(m_shader);

        // use materials
        xpos += 2;
        m_shader->set_int("coloring_type", 1);
        // first material
        m_mat1->apply(m_shader);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(m_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(m_shader);

        xpos += 2;

        // second material
        m_mat2->apply(m_shader);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(m_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(m_shader);

        xpos += 2;

        // third material
        m_mat3->apply(m_shader);
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(m_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(m_shader);

        //////////////////////////////////////////////////////////////////////////
        //
        m_shader->set_int("coloring_type", 2);
        m_mat4->apply(m_shader);
        xpos += 2;
        m_cube->move_to(vec3(xpos, 1, -6.f));
        m_cube->render(m_shader);

        m_mat5->apply(m_shader);
        m_sphere->move_to(vec3(xpos, -1, -6.f));
        m_sphere->render(m_shader);
    }

    virtual void exit_application() {
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

lights_materials_app my_app;       // default