#include "car_control.h"
#include "world_objects.h"

using namespace atlas;

#define STEER_LEFT 0x01
#define STEER_RIGHT 0x02
#define STEER_CENTER 0x04

#define ACCELERATE  0x08
#define DECELERATE  0x10

class point_of_view {
protected:
    atlas::vec3		vPosition;		// position in earth coordinates
    atlas::vec3		vVelocity;		// velocity in earth coordinates
public:
    point_of_view() : vPosition(atlas::vec3(0)), vVelocity(atlas::vec3(0)) {}
    virtual ~point_of_view() {}

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
    virtual void step_simulation(float dt, unsigned int options) {
        // calculate the position in earth space:
        vPosition = vPosition + vVelocity * dt;
    }
};

class demo_car : public point_of_view {
    gl_viewport* m_view;
    gl_camera* m_cam;
    vec3 car_fwd;
    float lr_look_at;
    float steering_angle = 0;
    float wheelbase = 2.6f;

    void update_pov() {
        atlas::vec3 look_at = car_fwd;
        atlas::rotate_vector(look_at, atlas::vec3(0, 1, 0), lr_look_at);
        m_cam->setup_d(position(), look_at, vec3(0, 1, 0));
    }
public:
    demo_car() : point_of_view(), lr_look_at(0.f) {
        m_view = new gl_viewport();
        m_view->set_fov(dtr(90));
        m_cam = new gl_camera();
    }
    void init_simulation(const vec3& pos, const vec3& fwd, const vec3& up, const vec3& vel) {
        move_to(pos);
        set_velocity(vel);
        car_fwd = fwd;
        car_fwd.normalize();
    }
    virtual void step_simulation(float dt, unsigned int options) {
        vPosition = vPosition + vVelocity * dt;

        atlas::vec3 acceleration = vVelocity;
        acceleration.normalize();
        if (options & ACCELERATE) // fasler
            acceleration *= 1.f;
        else if (options & DECELERATE) // slower
            acceleration *= -1.f;
        else
            acceleration = atlas::vec3(0);
        vVelocity += acceleration * dt;

        if (options & STEER_LEFT) { // left
            steering_angle += 40.f;
            if (steering_angle > 40.f)  // maximum steering angle
                steering_angle = 40.f;
        }
        else if (options & STEER_RIGHT) { // right
            steering_angle -= 40.f;
            if (steering_angle < -40.f)
                steering_angle = -40.f;
        }
        else if (options & STEER_CENTER)
            steering_angle = 0;

        if (fabs(steering_angle) > 0.5f)
        {
            float speed = vVelocity.length();
            float sa = dtr(steering_angle); // convert to radians
            float radius = wheelbase / (float)sin(sa);        // calculate the turn radius
            float angular_v = speed / radius;          // calculate angular speed
            float dh = angular_v * dt;           // calculate the anlge of turn for the elapsed time
            atlas::rotate_vector(vVelocity, atlas::vec3(0, 1, 0), dh);  // and change the car direction
            atlas::rotate_vector(car_fwd, atlas::vec3(0, 1, 0), dh);  // and change the car direction
        }
        update_pov();
    }
    gl_viewport* viewport() {
        return m_view;
    }
    gl_camera* camera() {
        return m_cam;
    }

};

class car_control_app :public c_application {
    gl_viewport* m_view;
    gl_camera* m_cam;
    c_light* m_light;
    c_shader* m_shader;

    skybox* m_skybox;
    ground* m_ground;

    demo_car* m_car;

    std::vector<building*> buildings;
    std::vector<pavement*> pavements;

    gl_font* font2D;
public:
    car_control_app() {
        m_view = NULL;
        m_cam = NULL;
        m_window.szTitle = "GusOnGames car control";
        m_window.prefered_width = 800;
        m_window.prefered_height = 500;
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
        for (int row = -5; row < 6; row += 2) {
            for (int col = -5; col < 6; col += 2) {
                float xpos = row * 5.f;
                float zpos = col * 5.f;
                building* build1 = new building(idx);
                build1->create();
                build1->set_scale(vec3(4, 4, 4));

                build1->move_to(xpos, 1.05f, zpos);
                buildings.push_back(build1);
                idx++;

                pavement* p = new pavement;
                p->create();
                p->set_scale(vec3(5.5f, 0.05f, 5.5f));

                p->move_to(xpos, .025f, zpos);
                pavements.push_back(p);
            }
            ++idx;
        }

        // moving viewer
        m_car = new demo_car;
        m_car->init_simulation(vec3(0, 0.25, 35), vec3(0, 0, -1), vec3(0, 1, 0), vec3(0, 0, -1));
        m_view = m_car->viewport();
        m_cam = m_car->camera();
    }

    virtual int init_application() {
        // light
        m_light = new c_light(c_light::SPOTLIGHT);
        m_light->set_position(vec3(-100, 1000, 1000));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 0, 1));

        m_shader = new c_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/VertexShader.hlsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/FragmentShader.hlsl");
        m_shader->load();

        create_environment();

        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void step_simulation(float fElapsed) {
        unsigned int dir = 0;
        if (keyDown['Z'])
            dir |= STEER_LEFT;
        else if (keyDown['X'])
            dir |= STEER_RIGHT;
        else if (keyDown['S'])
            dir |= STEER_CENTER;
        else if (keyDown['D'])
            dir |= ACCELERATE;
        else if (keyDown['C'])
            dir |= DECELERATE;

        m_car->step_simulation(fElapsed, dir);
    }

    virtual void render() {
        m_view->set_viewport();

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_cam->perspective() * m_view->perspective();

        m_skybox->render(m_view, m_cam);

        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_cam->vLocation);

        m_shader->set_int("use_texture", 0);
        m_ground->render(m_shader);

        for (auto p : pavements)
            p->render(m_shader);

        m_shader->set_int("use_texture", 1);
        glActiveTexture(GL_TEXTURE0);
        for (auto build1 : buildings)
            build1->render(m_shader);

        m_shader->end();

        font2D->set_color(vec4(1));
        font2D->set_position(5, 20);
        font2D->render("press X/Z to turn right/left, S to center");
        font2D->set_position(5, 5);
        font2D->render("press C/D to go slower/faster");
    }

    virtual void resize_window(int width, int height) {
        if (m_view)
            m_view->set_window_aspect(width, height);
    }
};

car_control_app my_app;       // default
