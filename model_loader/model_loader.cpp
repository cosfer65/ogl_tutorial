#include "model_loader.h"
#include "camera.h"
#include "light.h"
#include "util.h"
#include "primitives.h"

using namespace atlas;

static std::vector<atlas::vec3> vertices = {
    {-18.3805f, 0.0000f, 24.3798f},
    {-18.3805f, 0.0000f, -14.4791f},
    {-3.8473f, 0.0000f, -14.4791f},
    {-3.8473f, 0.0000f, 24.3798f},
    {-18.3805f, 5.7016f, 24.3798f},
    {-3.8473f, 5.7016f, 24.3798f},
    {-3.8473f, 5.7016f, -14.4791f},
    {-18.3805f, 5.7016f, -14.4791f},

    { -17.6351f, 5.6500f, 17.3526f },
    {-17.6351f, 5.6500f, -5.3794f},
    {-4.3287f, 5.6500f, -5.3794f},
    {-4.3287f, 5.6500f, 17.3526f},
    {-17.6351f, 11.4303f, 17.3526f},
    {-4.3287f, 11.4303f, 17.3526f},
    {-4.3287f, 11.4303f, -5.3794f},
    {-17.6351f, 11.4303f, -5.3794f}
};

static std::vector<atlas::vec3> normals = {
    {0.0000f, -1.0000f, -0.0000f},
    {0.0000f, 1.0000f, -0.0000f},
    {0.0000f, 0.0000f, 1.0000f},
    {1.0000f, 0.0000f, -0.0000f},
    {0.0000f, 0.0000f, -1.0000f},
    {-1.0000f, 0.0000f, -0.0000f},

    { 0.0000f, -1.0000f, -0.0000f },
    {0.0000f, 1.0000f, -0.0000f},
    {0.0000f, 0.0000f, 1.0000f},
    {1.0000f, 0.0000f, -0.0000f},
    {0.0000f, 0.0000f, -1.0000f},
    {-1.0000f, 0.0000f, -0.0000f}
};

static std::vector<std::vector<atlas::ivec3>> faces1 = {
    { {1,1,1}, {2,2,1}, {3,3,1} },
    { {3,3,1}, {4,4,1}, {1,1,1} },
    { {5,4,2}, {6,1,2}, {7,2,2} },
    { {7,2,2}, {8,3,2}, {5,4,2} },
    { {1,4,3}, {4,1,3}, {6,2,3} },
    { {6,2,3}, {5,3,3}, {1,4,3} },
    { {4,4,4}, {3,1,4}, {7,2,4} },
    { {7,2,4}, {6,3,4}, {4,4,4} },
    { {3,4,5}, {2,1,5}, {8,2,5} },
    { {8,2,5}, {7,3,5}, {3,4,5} },
    { {2,4,6}, {1,1,6}, {5,2,6} },
    { {5,2,6}, {8,3,6}, {2,4,6} }
};

static std::vector<std::vector<atlas::ivec3>> faces2 = {
{ {9,5,7}, {10,6,7}, {11,7,7} },
{ {11,7,7}, {12,8,7}, {9,5,7} },
{ {13,8,8}, {14,5,8}, {15,6,8} },
{ {15,6,8}, {16,7,8}, {13,8,8} },
{ {9,8,9}, {12,5,9}, {14,6,9} },
{ {14,6,9}, {13,7,9}, {9,8,9} },
{ {12,8,10}, {11,5,10}, {15,6,10} },
{ {15,6,10}, {14,7,10}, {12,8,10} },
{ {11,8,11}, {10,5,11}, {16,6,11} },
{ {16,6,11}, {15,7,11}, {11,8,11} },
{ {10,8,12}, {9,5,12}, {13,6,12} },
{ { 13,6,12 }, {16,7,12}, {10,8,12} }
};

gl_prim* create_prim(const std::vector<atlas::vec3>& verts, const std::vector<atlas::vec3>& norms, const std::vector<std::vector<atlas::ivec3>>& fcs) {
    c_mesh* m_mesh = new c_mesh;

    int count = 0;
    for (auto& f : fcs) {
        for (int i = 0; i < 3; ++i) {
            int v = f[i].x - 1;
            m_mesh->addVertex(verts[v]);
            v = f[i].z - 1;
            m_mesh->addNormal(norms[v]);
        }
        m_mesh->addIndices(count, count + 1, count + 2);
        count += 3;
    }
    gl_prim* prim = new gl_prim;
    prim->create_from_mesh(m_mesh, GL_FILL);
    delete m_mesh;
    return prim;
}


class atlas_app :public c_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    c_light* m_light;

    // the shaders
    c_shader* m_shader;
    c_shader* mat_shader;
    c_shader* map_shader;

    // the objects
    gl_prim* m_cube;
    gl_prim* m_sphere;

    gl_prim* m_car;
    gl_prim* m_car2;

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
        m_view = new gl_viewport();
        m_window.szTitle = "GusOnGames (lights and materials)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual int init_application() {
        m_view->set_fov(dtr(15));
        m_cam = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

#if 1
        m_light = new c_light(c_light::SPOTLIGHT);
        // light source located at (10,10,10)
        m_light->set_position(vec3(10, 10, 10));
#else
        m_light = new c_light(c_light::DIRLIGHT);
        // we are holding the light source and pointing at the objects
        m_light->set_direction(vec3(0, 0, -1));
#endif
        // allow some ambience for the light
        // white light (we can experiment with this)
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        // create the simple light shader
        m_shader = new c_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_fs.glsl");
        m_shader->load();

        // create the material shader
        mat_shader = new c_shader;
        mat_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.glsl");
        mat_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_material_fs.glsl");
        mat_shader->load();

        map_shader = new c_shader;
        map_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.glsl");
        map_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_map_fs.glsl");
        map_shader->load();

        // the cube
        m_cube = create_cube(GL_FILL);
        // the sphere
        m_sphere = create_sphere(GL_FILL);

        m_car = create_prim(vertices, normals, faces1);
        m_car2 = create_prim(vertices, normals, faces2);
        m_car->move_to(1, -1, 2);
        m_car2->move_to(1, -1, 2);

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

    virtual void step_simulation(float fElapsed) {
        m_cube->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        m_sphere->rotate_by(vec3(0, dtr(fElapsed * 20), 0));

        // m_car->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
        // m_car2->rotate_by(vec3(0, dtr(fElapsed * 20), 0));
        m_car->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
        m_car2->rotate_by(vec3(dtr(fElapsed * 10), dtr(fElapsed * 20), dtr(fElapsed * 30)));
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

        m_car->set_scale(0.02f, 0.02f, 0.02f);
        m_car->render(m_shader);
        m_shader->set_vec3("objectColor", vec3(0.07568f, 0.41424f, 0.7568f));
        m_car2->set_scale(0.02f, 0.02f, 0.02f);
        m_car2->render(m_shader);

        // m_cube->set_scale(2, .7f, 4.5f);
        // m_cube->move_to(vec3(xpos, 0, -6.f));
        // m_cube->render(m_shader);
        //
        // m_cube->set_scale(2, .7f, 2.5f);
        // m_cube->move_to(vec3(xpos, 0.7f, -6.f));
        // m_cube->render(m_shader);
    }
    virtual void _render() {
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

    virtual void exit_application() {
    }

    virtual void resize_window(int width, int height) {
        m_view->set_window_aspect(width, height);
    }
};

atlas_app my_app;       // default