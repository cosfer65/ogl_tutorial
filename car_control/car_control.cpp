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
    gl_viewport* m_view;
    gl_camera* m_cam;

public:
    point_of_view() {
        m_view = new gl_viewport();
        m_view->set_fov(dtr(90));
        m_cam = new gl_camera();
    }
    virtual ~point_of_view() {}

    virtual void create() {}

    virtual void init_simulation(const vec3& pos, const vec3& fwd, const vec3& up) {
        m_cam->setup(pos, fwd, up);
    }

    const atlas::vec3& position() {
        return m_cam->vLocation;
    }
    void move_to(const atlas::vec3& v) {
        m_cam->vLocation = v;
        m_cam->setup_d();
    }
    void move_to(float x, float y, float z) {
        m_cam->vLocation = atlas::vec3(x, y, z);
        m_cam->setup_d();
    }
    virtual void update_view(const vec3& pos, const vec3& fwd, const vec3& up) {
        m_cam->setup_d(pos, fwd, up);
    }

    gl_viewport* viewport() {
        return m_view;
    }
    gl_camera* camera() {
        return m_cam;
    }
};

class moving_car {
    atlas::vec3		vPosition;		// position in earth coordinates
    atlas::vec3		vVelocity;		// velocity in earth coordinates
    vec3 car_fwd;                   // forward looking vector (up is always (0,1,0))

    float steering_angle = 0;
    float wheelbase = 2.6f;

    point_of_view* p_view = nullptr;

    gl_model* car_model;

public:
    moving_car() {
    }
    point_of_view* pov() {
        return p_view;
    }
    void init_simulation(const vec3& pos, const vec3& fwd, const vec3& up, const vec3& vel) {
        if (!p_view)
            p_view = new point_of_view;
        p_view->update_view(pos, fwd, up);
        p_view->viewport()->set_fov(dtr(45));

        vPosition = pos;
        vVelocity = vel;

        car_fwd = fwd;
        car_fwd.normalize();

        if (!car_model)
            car_model = new gl_model("resources/car_01.obj");
        car_model->set_scale(vec3(0.4f, 0.4f, 0.4f));
        car_model->move_to(vPosition);
    }
    const atlas::vec3& position() {
        return vPosition;		// position in earth coordinates
    }
    const atlas::vec3& velocity() {
        return vVelocity;		// velocity in earth coordinates
    }

    void step_simulation(float dt, unsigned int options) {
        vPosition = vPosition + vVelocity * dt;

        atlas::vec3 acceleration = vVelocity;
        acceleration.normalize(); // unit vector parallel to our velocity

        if (options & ACCELERATE) // fasler
            acceleration *= 1.f;
        else if (options & DECELERATE) // slower
            acceleration *= -1.f;
        else
            acceleration = atlas::vec3(0);

        vVelocity += acceleration * dt;
        // no negative velocity
        if (vVelocity.length() < 0.001 && (options & DECELERATE))
            vVelocity = vec3(0);
        float steering_turn_speed = 20; // degrees per second
        if (options & STEER_LEFT) { // left
            steering_angle += steering_turn_speed * dt;
            if (steering_angle > 40.f)  // maximum steering angle
                steering_angle = 40.f;
        }
        else if (options & STEER_RIGHT) { // right
            steering_angle -= steering_turn_speed * dt;
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
            atlas::rotate_vector(car_fwd, atlas::vec3(0, 1, 0), dh);  // and change the car fwd direction vector
            car_model->rotate_by(0, dh, 0);        // turn the car
        }
        car_model->move_to(vPosition + vec3(0, 0.34f, 0));
        p_view->update_view(vPosition + vec3(0, 1.1f, 0), car_fwd, atlas::vec3(0, 1, 0));
    }
    gl_viewport* viewport() {
        return p_view->viewport();
    }
    gl_camera* camera() {
        return p_view->camera();
    }

    void render(gl_shader* shader) {
        car_model->render(shader);
    }
};

// camera that follows the action from a distance
class moving_camera : public point_of_view {
    atlas::vec3 tLocation;      // the tagret location
    atlas::vec3 tUp;            // target up vector
    atlas::vec3 tLookAt;        // target point of interest
    float m_speed = 0.005f;      // camera transition speed
public:
    moving_camera() : point_of_view() {
    }
    ~moving_camera() {}
    void move_to(const atlas::vec3& pos) {
        tLocation = pos;
    }
    void look_at(const atlas::vec3& lookat) {
        tLookAt = lookat;
    }
    void orient(const atlas::vec3& up) {
        tUp = up;
    }
    void set_speed(float spd) {
        m_speed = spd;
    }
    // actual camera location is set gradualy using camera speed
    void step_simulation(float fElapsed) {
        atlas::vec3 dist(tLocation - m_cam->vLocation);
        if (dist.length() < 0.001f)
            return;
        // here we set only the camera position
        atlas::vec3 pos(m_cam->vLocation + m_speed * dist);
        m_cam->setup(pos, tLookAt, tUp);
    }
};

class car_control_app :public gl_application {
    point_of_view* m_pov[2];
    int view_point;

    moving_camera* p_moving_camera = nullptr;
    gl_light* m_light;
    gl_shader* m_shader;

    skybox* m_skybox;
    ground* m_ground;

    moving_car* m_car;

    std::vector<building*> buildings;
    std::vector<pavement*> pavements;

    gl_font* font2D;
public:
    car_control_app() {
        m_window.szTitle = "GusOnGames car control (multiple views)";
        m_window.prefered_width = 1000;
        m_window.prefered_height = 750;
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
                float xpos = row * 14.f;
                float zpos = col * 14.f;
                building* build1 = new building(idx);
                build1->create();
                build1->set_scale(vec3(12, 12, 12));

                build1->move_to(xpos, 6.1f, zpos);
                buildings.push_back(build1);
                idx++;

                pavement* p = new pavement;
                p->create();
                p->set_scale(vec3(15, 0.1f, 15));

                p->move_to(xpos, .025f, zpos);
                pavements.push_back(p);
            }
            ++idx;
        }

        // moving viewer
        m_car = new moving_car;
        m_car->init_simulation(vec3(0, 0, 85), vec3(0, 0, -1), vec3(0, 1, 0), vec3(0, 0, -1));
        m_pov[0] = m_car->pov();
        m_pov[0]->viewport()->set_window_aspect(m_window.prefered_width, m_window.prefered_height);
        p_moving_camera = new moving_camera;
        p_moving_camera->init_simulation(vec3(0, 5, 65), vec3(0, 0, -1), vec3(0, 1, 0));
        m_pov[1] = p_moving_camera;
        m_pov[1]->viewport()->set_window_aspect(m_window.prefered_width, m_window.prefered_height);
        view_point = 1;
    }

    virtual int init_application() {
        // light
        m_light = new gl_light(gl_light::SPOTLIGHT);
        m_light->set_position(vec3(-100, 1000, 1000));
        m_light->set_ambient(vec3(1, 1, 1));
        m_light->set_diffuse(vec3(1, 1, 1));
        m_light->set_specular(vec3(1, 0, 1));

        m_shader = new gl_shader;
        m_shader->add_file(GL_VERTEX_SHADER, "resources/VertexShader.glsl");
        m_shader->add_file(GL_FRAGMENT_SHADER, "resources/FragmentShader.glsl");
        // m_shader->add_file(GL_VERTEX_SHADER, "resources/lights_materials_vs.glsl");
        // m_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_material_fs.glsl");
        // m_shader->add_file(GL_FRAGMENT_SHADER, "resources/lights_materials_fs.glsl");
        m_shader->load();

        create_environment();

        font2D = get_font_manager().create_font("Consolas", "Consolas", 12);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }

    virtual void restart_simulation() {
        m_car->init_simulation(vec3(0, 0, 85), vec3(0, 0, -1), vec3(0, 1, 0), vec3(0, 0, -1));
        p_moving_camera->init_simulation(vec3(0, 5, 65), vec3(0, 0, -1), vec3(0, 1, 0));
        view_point = 1;
        b_paused = false;
    }

    virtual void step_simulation(float fElapsed) {
        if (keyDown['R']) {
            keyDown['R'] = 0;
            restart_simulation();
        }

        if (keyDown['P']) {
            b_paused ^= true;
            keyDown['P'] = 0;
        }
        if (b_paused)
            return;

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
        else if (keyDown['V']) {
            view_point = 1 - view_point;
            keyDown['V'] = 0;
        }

        m_car->step_simulation(fElapsed, dir);

        // position the moving camera
        // 1. get moving object velocity vector
        atlas::vec3 s(m_car->velocity());
        // and make it unit length
        s.normalize();
        // 2. create the view distance vector
        s.scale(3.5f);
        // 3. and calculate the camera position
        atlas::vec3 loc(m_car->position() - s);
        // raise the view camera at a certain height
        loc.y = 2.f;
        // apply all these to the moving camera
        p_moving_camera->move_to(loc);
        p_moving_camera->look_at(m_car->position());
        p_moving_camera->orient(atlas::vec3(0, 1, 0));
        // and let it move
        p_moving_camera->step_simulation(fElapsed);
    }

    virtual void render() {
        if (!m_pov[view_point])
            return;
        m_pov[view_point]->viewport()->set_viewport();

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 cam_matrix = m_pov[view_point]->camera()->perspective() * m_pov[view_point]->viewport()->perspective();

        m_skybox->render(m_pov[view_point]->viewport(), m_pov[view_point]->camera());

        m_shader->use();
        m_light->apply(m_shader);
        m_shader->set_mat4("camera", cam_matrix);
        m_shader->set_vec3("cameraPos", m_pov[view_point]->camera()->vLocation);

        m_shader->set_int("use_texture", 0);
        m_ground->render(m_shader);

        for (auto p : pavements)
            p->render(m_shader);

        m_shader->set_int("use_texture", 1);
        glActiveTexture(GL_TEXTURE0);
        for (auto build1 : buildings)
            build1->render(m_shader);

        m_shader->set_int("use_texture", 0);
        m_car->render(m_shader);

        m_shader->end();

        font2D->set_color(vec4(1));
        font2D->set_position(5, 35);
        font2D->render("press X/Z to turn right/left, S to center");
        font2D->set_position(5, 20);
        font2D->render("press C/D to go slower/faster");
        font2D->set_position(5, 5);
        font2D->render("P to toggle pause state / R to reset simulation / V to toggle view point");
    }

    virtual void resize_window(int width, int height) {
        if (m_pov[view_point])
            m_pov[view_point]->viewport()->set_window_aspect(width, height);
    }
};

car_control_app my_app;       // default