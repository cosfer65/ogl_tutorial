#ifndef __instruments_h__
#define __instruments_h__

struct stickSate {
    stickSate() {
        x = y = 0;
        thrust = 0.f;
    }
    int x;
    int y;
    float thrust;
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
        //m_stencil->set_scale(vec3(0.15f));
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

        m_view.set_position(2*h4, h4);
        m_view.set_extent(h4, h4);
        m_viewport->set_window_aspect(h4, h4);
        m_viewport->set_position(2*h4, h4);
    }
};

#endif // __instruments_h__