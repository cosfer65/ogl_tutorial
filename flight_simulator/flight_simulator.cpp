#include "flight_simulator.h"
#include "camera.h"
#include "light.h"
#include "primitives.h"
#include "skybox.h"
#include "font.h"

using namespace atlas;

#include "airplane.h"
#include "world_objects.h"
#include "instruments.inl"

void print_vec(vec3 v) {
    char str[100];
    sprintf(str, "%f, %f, %f", v.x, v.y, v.z);
    OutputDebugString(str);
}
void print_float(float v) {
    char str[100];
    sprintf(str, "%f", v);
    OutputDebugString(str);
}

class atlas_app :public gl_application {
    bool init_done = false;
    gl_model* model1;

    // main view
    gl_view_window main_view;
    gl_camera* main_camera;
    gl_viewport* main_viewport;
    gl_light* main_light;
    gl_shader* main_shader;

    skybox* m_skybox;

    // artificial horizon display
    artificial_horizon m_ah;
    roll_indicator m_roll_ind;
    compass_indicator m_compass_ind;
    altimeter m_altimeter;

    airplane m_airplane;
    vec3 euler_angles;
    stickSate tstate;
    stick_indicator m_stick_indicator;

    world* m_world;

    map_instrument m_map;

public:
    atlas_app() {
        m_window.szTitle = "GusOnGames (flight simulator)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }

    void create_world() {
        m_world = new world;
        ground* g = new ground;
        g->create();
        g->set_scale(vec3(10000, 0, 10000));

        m_world->add_object(g);

        building* bld;

        float x, z;
        int c = 0;
        for (x = -450.f; x < 451.f; x += 50.f) {
            for (z = -450.f; z < 451.f; z += 50.f) {
                bld = new building(c);
                c++;

                bld->create();
                float w = (float)(rand() % 15) + 15;
                float d = (float)(rand() % 15) + 15;
                float h = (float)(rand() % 25) + 25;
                bld->move_to(x, h / 2.f, z);
                bld->set_scale(vec3(w, h, d));
                m_world->add_object(bld);
            }
        }
    }

    void build_main_view() {
        main_viewport = new gl_viewport();
        main_viewport->set_perspective(dtr(30), 0.1f, 10000.f);
        main_camera = new gl_camera(vec3(0, 50, 600), vec3(0, 0, 0), vec3(0, 1, 0));

        m_skybox = new skybox;
        m_skybox->create();

        main_light = new gl_light(gl_light::SPOTLIGHT);
        // we are holding the light source and pointing at the objects
        main_light->set_position(vec3(-600, 1000, 1000));
        // allow some ambience for the light
        // white light (we can experiment with this)
        main_light->set_ambient(vec3(1, 1, 1));
        main_light->set_diffuse(vec3(1, 1, 1));
        main_light->set_specular(vec3(1, 1, 1));

        // create the simple light shader
        main_shader = new gl_shader;
        main_shader->add_file(GL_VERTEX_SHADER, "resources/VertexShader.glsl");
        main_shader->add_file(GL_FRAGMENT_SHADER, "resources/FragmentShader.glsl");
        main_shader->load();

        create_world();
    }

    void build_secondary_views() {
        m_ah.initialize();
        m_roll_ind.initialize();
        m_compass_ind.initialize();
        m_altimeter.initialize();
        m_stick_indicator.initialize();
        m_stick_indicator.set_stick_state(&tstate);
        m_map.initialize();
        m_map.set_world(m_world);
    }

    virtual int init_application() {
        build_main_view();
        build_secondary_views();

        model1 = new gl_model();
        model1->load("resources/flight_sim_plane.stl");
        model1->set_scale(vec3(0.2f, 0.2f, 0.2f));

        m_airplane.InitializeAirplane();
        tstate.thrust = m_airplane.GetThrust() * 1000.f;

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);
        init_done = true;
        resize_window(m_window.current_width, m_window.current_height);

        return 1;
    }

    virtual void onMouseMove(int dx, int dy, WPARAM extra_btn) {
        // ignore mouse move when left button is not pressed
        if (!(extra_btn & MK_LBUTTON)) {
            if (dx != 0) {
                tstate.x += dx;
                if (tstate.x < -1000) tstate.x = -1000;
                if (tstate.x > 1000) tstate.x = 1000;
                float lr = (float)tstate.x / 1000.f;
                if (lr < 0)
                    m_airplane.RollRight(-lr);
                if (lr > 0)
                    m_airplane.RollLeft(lr);
            }
            if (dy != 0) {
                tstate.y += dy;
                if (tstate.y < -1000) tstate.y = -1000;
                if (tstate.y > 1000) tstate.y = 1000;
                float ud = (float)tstate.y / 1000.f;
                if (ud < 0)
                    m_airplane.PitchDown(-ud);
                if (ud > 0)
                    m_airplane.PitchUp(ud);
            }
        }
    };

    virtual void onMouseWheel(int delta, WPARAM extra_btn) {
    };

    void draw_world() {
        main_view.start();
        main_viewport->set_viewport();

        mat4 cam_matrix = main_camera->perspective() * main_viewport->perspective();

        m_skybox->render(main_viewport, main_camera);

        // objects with simple light
        main_shader->use();
        main_light->apply(main_shader);
        main_shader->set_mat4("camera", cam_matrix);
        main_shader->set_vec3("cameraPos", main_camera->vLocation);

        m_world->render(main_shader);

        main_shader->end();
        main_view.end();
    }

    virtual void render() {
        glViewport(0, 0, m_window.current_width, m_window.current_height);
        glClearColor(0.3f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_world();
        m_ah.render();
        m_roll_ind.render();
        m_compass_ind.render();
        m_altimeter.render();
        m_stick_indicator.render();
        m_map.render();
    }

    virtual void exit_application() {
    }

    virtual void resize_window(int width, int height) {
        gl_application::resize_window(width, height);
        if (init_done) {
            int h2 = height / 2;

            main_view.set_position(0, h2);
            main_view.set_extent(width, h2);
            main_viewport->set_window_aspect(width, h2);
            main_viewport->set_position(0, h2);

            m_ah.resize_window(width, height);
            m_roll_ind.resize_window(width, height);
            m_compass_ind.resize_window(width, height);
            m_altimeter.resize_window(width, height);
            m_stick_indicator.resize_window(width, height);
            m_map.resize_window(width, height);
        }
    }

    virtual void step_simulation(float fElapsed) {
        if (keyDown['Z'])
            tstate.thrust -= fElapsed * 400.f;
        if (keyDown['A'])
            tstate.thrust += fElapsed * 400.f;
        if (tstate.thrust < 0.f) tstate.thrust = 0.f;
        if (tstate.thrust > 1000.f) tstate.thrust = 1000.f;
        m_airplane.SetThrust(tstate.thrust / 1000.f);

        m_airplane.StepSimulation(fElapsed);
        euler_angles = make_euler_angles_from_q(m_airplane.Airplane.qOrientation);
        model1->rotate_to(-euler_angles.y, -euler_angles.z, euler_angles.x);
        m_ah.rotate_to(-euler_angles.y, 0, euler_angles.x);
        m_roll_ind.rotate_to(0, 0, euler_angles.x); // done
        m_compass_ind.rotate_to(0, 0, euler_angles.z);
        m_altimeter.set_altitude(m_airplane.Airplane.vPosition.z);
        m_altimeter.set_climb(m_airplane.Airplane.vVelocity.z);

        // calculate pilot's view
        vec3 z = m_airplane.GetBodyZAxisVector();
        vec3 x = m_airplane.GetBodyXAxisVector();
        vec3 vForward = clf_vector3D(x.x, x.z, x.y);
        vec3 vUp = clf_vector3D(z.x, z.z, z.y);
        x = m_airplane.Airplane.vPosition * 0.3f; // convert ft to m
        vec3 vLocation = clf_vector3D(x.x, x.z, x.y);
        main_camera->setup_d(vLocation, vForward, vUp);

        // calculate map view
        x = m_airplane.Airplane.vPosition * 0.3f; // convert ft to m
        vLocation = clf_vector3D(x.x, 1500, x.y);
        vForward = clf_vector3D(0, -1, 0);
        x = m_airplane.GetBodyXAxisVector();
        vUp = clf_vector3D(x.x, 0, x.y);
        m_map.get_camera()->setup_d(vLocation, vForward, vUp);
    }
};

atlas_app my_app;       // default