#ifndef __instruments_h__
#define __instruments_h__

#include "skybox.h"
#include "font.h"

struct stickSate {
    stickSate() {
    }
    int x = 0;
    int y = 0;
    float thrust = 0.f;
};

class gl_view_window {
    int left, bottom;
    int width, height;
public:
    gl_view_window() {}
    ~gl_view_window() {}
    void set_position(int l, int b) {
        left = l;
        bottom = b;
    }
    void set_extent(int w, int h) {
        width = w;
        height = h;
    }
    void start() {
        glScissor(left, bottom, width, height);
        glEnable(GL_SCISSOR_TEST);
    }
    void end() {
        glDisable(GL_SCISSOR_TEST);
    }
};

class pilot_view {
    gl_view_window m_view;
    gl_viewport* m_viewport;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;
    skybox* m_skybox;

    world* m_world = nullptr;

public:
    pilot_view() {}
    ~pilot_view() { }
    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_perspective(dtr(30), 0.1f, 10000.f);
        m_cam = new gl_camera(vec3(0, 0, 30), vec3(0, 0, 0), vec3(0, 1, 0));

        m_skybox = new skybox;
        m_skybox->create();

        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-3000, 3000, 3000));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 1, 1));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/FragmentShader.glsl");
        m_shader->load();
    }

    void set_world(world* w) {
        m_world = w;
    }
    gl_camera* get_camera() {
        return m_cam;
    }
    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_skybox->render(m_viewport, m_cam);

        m_shader->use();
        m_light->apply(m_shader);
        mat4 cam_matrix = m_cam->perspective() * m_viewport->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);
        if (m_world) {
            m_world->render(m_shader);
        }
        m_shader->end();
        m_view.end();
    }
    void resize_window(int width, int height) {
        int h2 = height / 2;

        m_view.set_position(0, h2);
        m_view.set_extent(width, h2);
        m_viewport->set_window_aspect(width, h2);
        m_viewport->set_position(0, h2);
    }
};
    

class artificial_horizon {
    gl_view_window m_view;
    gl_camera* m_camera;
    gl_viewport* m_viewport;
    gl_shader* m_shader;

    gl_model* m_ball;
    gl_stencil* m_stencil;

    float m_pitch, m_yaw, m_roll;

public:
    artificial_horizon() {}
    ~artificial_horizon() {}

    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_fov(dtr(6.5));
        m_camera = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/artificial_horizon_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/artificial_horizon_fs.glsl");
        m_shader->load();

        m_ball = new gl_model();
        m_ball->load("resources/artificial_horizon.obj");

        m_stencil = new gl_stencil;
        m_stencil->create_elliptic();
        m_stencil->set_scale(vec3(0.75f));
    }

    void rotate_by(float pitch, float yaw, float roll) {
        m_pitch += pitch;
        m_yaw += yaw;
        m_roll += roll;

        m_ball->rotate_by(pitch, yaw, roll);
    }
    void rotate_to(float pitch, float yaw, float roll) {
        m_pitch = pitch;
        m_yaw = yaw;
        m_roll = roll;

        m_ball->rotate_to(pitch, yaw, roll);
    }

    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();

        // GL_STENCIL_BUFFER_BIT  needs mask=0xFF
        glStencilMask(0xff);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // we are using stencil
        glEnable(GL_STENCIL_TEST);
        // prepare the stencil buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glStencilFunc(GL_NEVER, 1, 0xff);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

        mat4 cam_matrix = m_camera->perspective() * m_viewport->perspective();

        m_shader->use();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_camera->vLocation);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        m_stencil->render(m_shader);

        // return to normal drawing
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
        glStencilMask(0x00); // do NOT draw on black
        // draw only where stencil's value is 1
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // enable them before we start normal draing
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        m_ball->render(m_shader);
        // stop using stncil
        glDisable(GL_STENCIL_TEST);
        m_shader->end();

        m_view.end();
    }

    void resize_window(int width, int height) {
        int h4 = height / 4;

        m_view.set_position(0, h4);
        m_view.set_extent(h4, h4);
        m_viewport->set_window_aspect(h4, h4);
        m_viewport->set_position(0, h4);
    }
};

class roll_indicator {
    gl_view_window m_view;
    gl_camera* m_camera;
    gl_viewport* m_viewport;
    gl_shader* m_shader;

    gl_model* m_ball;

    float m_pitch, m_yaw, m_roll;
public:
    roll_indicator() {}
    ~roll_indicator() {}
    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_fov(dtr(6.5));
        m_camera = new gl_camera(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/artificial_horizon_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/artificial_horizon_fs.glsl");
        m_shader->load();

        m_ball = new gl_model();
        m_ball->load("resources/flight_sim_plane.stl");
        m_ball->set_scale(0.15f);
    }

    void rotate_by(float pitch, float yaw, float roll) {
        m_pitch += pitch;
        m_yaw += yaw;
        m_roll += roll;

        m_ball->rotate_by(pitch, yaw, roll);
    }
    void rotate_to(float pitch, float yaw, float roll) {
        m_pitch = pitch;
        m_yaw = yaw;
        m_roll = roll;

        m_ball->rotate_to(pitch, yaw, roll);
    }

    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();

        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_camera->perspective() * m_viewport->perspective();

        m_shader->use();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_camera->vLocation);

        m_ball->render(m_shader);

        m_shader->end();

        m_view.end();
    }

    void resize_window(int width, int height) {
        int h4 = height / 4;

        m_view.set_position(h4, h4);
        m_view.set_extent(h4, h4);
        m_viewport->set_window_aspect(h4, h4);
        m_viewport->set_position(h4, h4);
    }
};

class gl_disk :public gl_prim {
protected:
    gl_texture m_texture;
public:
    gl_disk() {
    }
    virtual ~gl_disk() {
    }
    virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        draw_elements = dr_el;
        draw_mode = drmode;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;
            for (float a = 0; a <= 360; a += 10) {
                float da = dtr(a);
                float xin = 2 * (float)(cos(da)) / 3;
                float yin = 2 * (float)(sin(da)) / 3;
                float xout = (float)(cos(da));
                float yout = (float)(sin(da));
                tmesh->addVertex(xin, yin, 0);
                tmesh->addTexCoord((1 + xin) / 2.f, (1 + yin) / 2.f);
                tmesh->addNormal(0, -1, 0);

                tmesh->addVertex(xout, yout, 0);
                tmesh->addTexCoord((1 + xout) / 2.f, (1 + yout) / 2.f);
                tmesh->addNormal(0, -1, 0);
            }

            for (int i = 0; i <= 70; ++i) {
                tmesh->addIndices(i, i + 1, i + 2);
                tmesh->addIndices(i + 2, i + 1, i + 3);
            }

            create_from_mesh(tmesh, draw_mode, draw_elements);
            delete tmesh;
        }
        m_material = new cg_material;
        m_material->set_ambient(vec3(0.75f));
        m_texture.load("resources/compass.tga");
    }
    virtual void render(gl_shader* _shader) {
        if (!vao) return;

        m_texture.bind();
        _shader->set_int("use_texture", 1);
        gl_prim::render(_shader);
        _shader->set_int("use_texture", 0);
        m_texture.release();
    }
};

class gl_rect :public gl_prim {
protected:
    gl_texture m_texture;
public:
    gl_rect() {
    }
    virtual ~gl_rect() {
    }
    virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {
        draw_elements = dr_el;
        draw_mode = drmode;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;
            tmesh->addVertex(-1, -1, 0);
            tmesh->addTexCoord(0, 0);
            tmesh->addNormal(0, -1, 0);

            tmesh->addVertex(1, -1, 0);
            tmesh->addTexCoord(1, 0);
            tmesh->addNormal(0, -1, 0);

            tmesh->addVertex(1, 1, 0);
            tmesh->addTexCoord(1, 1);
            tmesh->addNormal(0, -1, 0);

            tmesh->addVertex(-1, 1, 0);
            tmesh->addTexCoord(0, 1);
            tmesh->addNormal(0, -1, 0);

            tmesh->addIndices(0, 1, 2);
            tmesh->addIndices(0, 2, 3);

            create_from_mesh(tmesh, draw_mode, draw_elements);
            delete tmesh;
        }
        m_material = new cg_material;
        m_material->set_ambient(vec3(0.75f));
        m_texture.load("resources/compass.tga");
    }
    virtual void render(gl_shader* _shader) {
        if (!vao) return;

        m_texture.bind();
        gl_prim::render(_shader);
        m_texture.release();
    }
};

class compass_indicator {
    gl_view_window m_view;
    gl_camera* m_camera;
    gl_viewport* m_viewport;
    gl_shader* m_shader;

    gl_model* m_ball;
    gl_disk* m_disk;

    float m_pitch, m_yaw, m_roll;
public:
    compass_indicator() {}
    ~compass_indicator() {}
    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_fov(dtr(6.5));
        m_camera = new gl_camera(vec3(0, 0, 30), vec3(0, 0, 0), vec3(0, 1, 0));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/compass_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/compass_fs.glsl");
        m_shader->load();

        m_ball = new gl_model();
        m_ball->load("resources/flight_sim_plane.stl");
        m_ball->set_scale(0.1f);
        m_ball->rotate_by(dtr(90), 0, 0);

        m_disk = new gl_disk();
        m_disk->create();
        m_disk->set_scale(vec3(1.5f));
    }

    void rotate_by(float pitch, float yaw, float roll) {
        m_pitch += pitch;
        m_yaw += yaw;
        m_roll += roll;

        //m_ball->rotate_by(pitch, yaw, roll);
        m_disk->rotate_by(pitch, yaw, roll);
    }
    void rotate_to(float pitch, float yaw, float roll) {
        m_pitch += pitch;
        m_yaw += yaw;
        m_roll += roll;

        //m_ball->rotate_by(pitch, yaw, roll);
        m_disk->rotate_to(pitch, yaw, roll);
    }

    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();

        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_camera->perspective() * m_viewport->perspective();

        m_shader->use();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_camera->vLocation);

        m_ball->render(m_shader);

        m_disk->render(m_shader);

        m_shader->end();

        m_view.end();
    }
    void resize_window(int width, int height) {
        int h4 = height / 4;

        m_view.set_position(2 * h4, h4);
        m_view.set_extent(h4, h4);
        m_viewport->set_window_aspect(h4, h4);
        m_viewport->set_position(2 * h4, h4);
    }
};

class altimeter {
    gl_view_window m_view;
    gl_viewport* m_viewport;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    gl_font* font3D;

    float m_altitude;
    float m_climb;
    float m_speed;

public:
    altimeter() {}
    ~altimeter() {}
    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_fov(dtr(6.5));

        m_cam = new gl_camera(vec3(0, 0, 30), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-30, 30, 30));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/fonts_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/fonts_fs.glsl");
        m_shader->load();

        font3D = get_font_manager().create_font("Bookman3D", "Bookman Old Style", 12, .2f);
        font3D->set_scale(vec3(0.4f));
    }

    void set_altitude(float a) {
        m_altitude = a;
    }
    void set_climb(float a) {
        m_climb = a;
    }
    void set_speed(float a) {
        m_speed = a;
    }
    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();

        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);

        mat4 cam_matrix = m_cam->perspective() * m_viewport->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        char txt[100];
        m_shader->set_vec4("objectColor", vec4(.9f, .9f, .9f, 1.f));

        font3D->move_to(0, 2.5f, 0);
        font3D->render(m_shader, ALIGN_CENTER, "Altitude");
        font3D->move_to(0, 1.5f, 0);
        sprintf(txt, "%d ft", (int)m_altitude);
        font3D->render(m_shader, ALIGN_CENTER, txt);

        font3D->move_to(0, 0.f, 0);
        font3D->render(m_shader, ALIGN_CENTER, "Climb");
        font3D->move_to(0, -1.f, 0);
        sprintf(txt, "%d ft/sec", (int)m_climb);
        font3D->render(m_shader, ALIGN_CENTER, txt);

        font3D->move_to(0, -2.5f, 0);
        font3D->render(m_shader, ALIGN_CENTER, "Speed");
        font3D->move_to(0, -3.5f, 0);
        sprintf(txt, "%d kn", (int)(m_speed * 0.5924838f));
        font3D->render(m_shader, ALIGN_CENTER, txt);

        m_shader->end();

        m_view.end();
    }
    void resize_window(int width, int height) {
        int h4 = height / 4;

        m_view.set_position(0, 0);
        m_view.set_extent(h4, h4);
        m_viewport->set_window_aspect(h4, h4);
        m_viewport->set_position(0, 0);
    }
};

class stick_indicator {
    gl_view_window m_view;
    gl_viewport* m_viewport;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    stickSate* m_stick_state = nullptr; // access to airplane's stick
    gl_prim* m_cross;
    gl_prim* m_diamond;
    gl_prim* m_thrust;

public:
    stick_indicator() {}
    ~stick_indicator() {}
    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_fov(dtr(6.5));

        m_cam = new gl_camera(vec3(0, 0, 30), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-30, 30, 30));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/stick_vs.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/stick_fs.glsl");
        m_shader->load();

        m_cross = create_cross();
        m_cross->set_scale(vec3(3));

        m_diamond = create_diamond();
        m_diamond->set_scale(vec3(0.5f));

        m_thrust = create_hbar();
        m_thrust->set_scale(vec3(0.75f));
    }

    void set_stick_state(stickSate* s) {
        m_stick_state = s;
    }
    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();

        glClearColor(0.1f, 0.1f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);

        mat4 cam_matrix = m_cam->perspective() * m_viewport->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        m_shader->set_vec4("objectColor", vec4(.85f, .85f, .85f, 1.f));
        m_cross->render(m_shader);

        if (m_stick_state) {
            float x, y, t;
            x = ((float)m_stick_state->x) / 500.f;
            y = ((float)m_stick_state->y) / 500.f;
            t = m_stick_state->thrust / 500.f;
            m_diamond->move_to(x, y, 0);
            m_shader->set_vec4("objectColor", vec4(.27f, .37f, .47f, .57f));
            m_diamond->render(m_shader);

            m_thrust->move_to(0, t - 1, 0);
            m_shader->set_vec4("objectColor", vec4(.67f, .37f, .17f, .57f));
            m_thrust->render(m_shader);
        }

        m_shader->end();

        m_view.end();
    }
    void resize_window(int width, int height) {
        int h4 = height / 4;

        m_view.set_position(h4, 0);
        m_view.set_extent(h4, h4);
        m_viewport->set_window_aspect(h4, h4);
        m_viewport->set_position(h4, 0);
    }
};

class map_instrument {
public:
    gl_view_window m_view;
    gl_viewport* m_viewport;
    gl_camera* m_cam;
    gl_light* m_light;
    gl_shader* m_shader;

    world* m_world = nullptr;

public:
    map_instrument() {}
    ~map_instrument() {}
    void initialize() {
        m_viewport = new gl_viewport();
        m_viewport->set_perspective(dtr(20), 0.1f, 10000.f);

        m_cam = new gl_camera(vec3(0, 0, 30), vec3(0, 0, 0), vec3(0, 1, 0));
        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-30, 30, 30));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/FragmentShader.glsl");

        m_shader->load();
    }
    gl_camera* get_camera() {
        return m_cam;
    }
    void set_world(world* w) {
        m_world = w;
    }
    virtual void render() {
        m_view.start();
        m_viewport->set_viewport();

        glClearColor(0.1f, 0.1f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();
        m_light->apply(m_shader);

        mat4 cam_matrix = m_cam->perspective() * m_viewport->perspective();
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        if (m_world) {
            //m_shader->set_vec4("objectColor", vec4(.85f, .85f, .85f, 1.f));
            m_world->render(m_shader);
        }

        m_shader->end();

        m_view.end();
    }

    void resize_window(int width, int height) {
        int h2 = height / 2;
        int w2 = width / 2;

        m_view.set_position(w2, 0);
        m_view.set_extent(w2, h2);
        m_viewport->set_window_aspect(w2, h2);
        m_viewport->set_position(w2, 0);
    }
};

#endif // __instruments_h__