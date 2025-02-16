#ifndef __WORLD_OBJECTS_H__
#define __WORLD_OBJECTS_H__

#include <map>

#include "camera.h"
#include "light.h"
#include "material.h"
#include "primitives.h"
#include "skybox.h"

#include "vector.h"

class resource_manager {
    resource_manager() {}
    static std::map<std::string, GLuint> m_textures;
    static resource_manager* m_manager;

public:
    ~resource_manager() {}

    static resource_manager* get_resource_manager() {
        if (m_manager == nullptr)
            m_manager = new resource_manager;
        return m_manager;
    }
    static GLuint load_texture(const std::string& fname);
};

class skybox {
    atlas::gl_skybox* m_sbox;
public:
    skybox() {
    }
    void create() {
        m_sbox = new atlas::gl_skybox;
        std::vector<std::string> faces
        {
            "resources/px.tga", "resources/nx.tga",
            "resources/py.tga", "resources/ny.tga",
            "resources/pz.tga", "resources/nz.tga"
        };
        m_sbox->load(faces);
    }
    void render(atlas::gl_viewport* vp, atlas::gl_camera* cam) {
        m_sbox->render(vp, cam);
    }
};

class world_object {
protected:
    atlas::vec3		vPosition;		// position in earth coordinates
    atlas::vec3		vVelocity;		// velocity in earth coordinates
    float m_angular_velocity;       // degrees/second
public:
    world_object() : vPosition(atlas::vec3(0)), vVelocity(atlas::vec3(0)), m_angular_velocity(60) {}
    virtual ~world_object() {}

    virtual void create() {}

    const atlas::vec3& position() {
        return vPosition;
    }
    virtual void move_to(const atlas::vec3& v) {
        vPosition = v;
    }
    virtual void move_to(float x, float y, float z) {
        vPosition = atlas::vec3(x, y, z);
    }
    const atlas::vec3& velocity() {
        return vVelocity;
    }
    void set_velocity(const atlas::vec3& v) {
        vVelocity = v;
    }
    void set_velocity(float x, float y, float z) {
        vVelocity = atlas::vec3(x, y, z);
    }
    float angular_velocity() {
        return m_angular_velocity;
    }
    void set_angular_velocity(float av) {
        m_angular_velocity = av;
    }
    virtual void step_simulation(float dt, unsigned int options) {
        // calculate the position in earth space:
        vPosition = vPosition + vVelocity * dt;
    }
    virtual void render(atlas::gl_shader* m_shader) {}
};

class ground :public world_object {
    atlas::cg_material* m_material;
    atlas::gl_prim* m_plane;
public:
    ground() {
    }
    virtual ~ground() {}
    virtual void create() {
        m_plane = atlas::create_plane(GL_FILL, true);
        m_plane->set_scale(200, 0, 200);

        m_material = new atlas::cg_material;
        m_material->set_ambient(0.23456f, 0.23456f, 0.23456f);
        m_material->set_diffuse(0.12345f, 0.12345f, 0.12345f);
        m_material->set_specular(0.34567f, 0.34567f, 0.34567f);
        m_material->set_shine(0.1f * 128);
    }
    void set_scale(const atlas::vec3& scale) {
        m_plane->set_scale(scale);
    }

    virtual void render(atlas::gl_shader* m_shader) {
        m_material->apply(m_shader);
        m_plane->render(m_shader);
    }
};

class pavement : public world_object {
    atlas::gl_prim* m_box;
    atlas::cg_material* m_material;
public:
    pavement() {
    }
    virtual ~pavement() {}

    virtual void create() {
        m_box = atlas::create_cube(GL_FILL, true);

        m_material = new atlas::cg_material;

        m_material->set_ambient(0.39225f, 0.39225f, 0.39225f);
        m_material->set_diffuse(0.50754f, 0.50754f, 0.50754f);
        m_material->set_specular(0.608273f, 0.608273f, 0.608273f);
        m_material->set_shine(0.4f * 128);
    }
    void set_scale(const atlas::vec3& scale) {
        m_box->set_scale(scale);
    }
    virtual void move_to(float x, float y, float z) {
        m_box->move_to(x, y, z);
    }
    virtual void render(atlas::gl_shader* m_shader) {
        m_material->apply(m_shader);
        m_box->render(m_shader);
    }
};

class building : public world_object {
    atlas::gl_prim* m_box;
    GLuint   m_texture;
public:
    building(int _i = 0) {
        if (_i % 2)
            m_texture = resource_manager::load_texture("resources/build1.tga");
        else
            m_texture = resource_manager::load_texture("resources/build2.tga");
    }
    virtual void create() {
        m_box = atlas::create_cube(GL_FILL, true);
    }
    void set_scale(const atlas::vec3& scale) {
        m_box->set_scale(scale);
    }
    void move_to(float x, float y, float z) {
        m_box->move_to(x, y, z);
    }
    GLuint texture() {
        return m_texture;
    }
    virtual void render(atlas::gl_shader* m_shader) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        m_shader->set_int("use_texture", 1);
        m_box->render(m_shader);
        m_shader->set_int("use_texture", 0);
        glActiveTexture(0);
    }
};

class world {
    std::vector<world_object*> m_objects;
public:
    world() {}
    ~world() {
        for (auto e : m_objects) {
            delete e;
        }   
    }
    void add_object(world_object* obj) {
        m_objects.push_back(obj);
    }
    void step_simulation(float dt, unsigned int options) {
        for (auto e : m_objects) {
            e->step_simulation(dt, options);
        }
    }
    void render(atlas::gl_shader* m_shader) {
        for (auto e : m_objects) {
            e->render(m_shader);
        }
    }
};

#endif // __WORLD_OBJECTS_H__
