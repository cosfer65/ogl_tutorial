#include "flight_simulator.h"
#include "camera.h"
#include "light.h"
#include "primitives.h"

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

class flight_sim :public gl_application {
    bool init_done = false;

    // main view
    pilot_view main_view;

    airplane m_airplane;

    // displayed instruments
    artificial_horizon m_ah;
    roll_indicator m_roll_ind;
    compass_indicator m_compass_ind;
    altimeter m_altimeter;
    stickSate tstate;
    stick_indicator m_stick_indicator;
    map_instrument m_map;

    world* m_world;

public:
    flight_sim() {
        m_window.szTitle = "GusOnGames (flight simulator)";
        m_window.prefered_width = 1200;
        m_window.prefered_height = 700;
    }
    virtual ~flight_sim() {
        delete m_world;
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
        for (x = -1500.f; x < 1510.f; x += 160.f) {
            for (z = -1500.f; z < 1510.f; z += 160.f) {
                bld = new building(c);
                c++;

                bld->create();
                int wd = 25;
                int hd = 50;
                float w = (float)(rand() % wd) + wd;
                float d = (float)(rand() % wd) + wd;
                float h = (float)(rand() % hd) + hd;
                bld->move_to(x, h / 2.f, z);
                bld->set_scale(vec3(w, h, d));
                m_world->add_object(bld);
            }
        }
    }

    virtual int init_application() {
        main_view.initialize();
        create_world();
        main_view.set_world(m_world);

        m_ah.initialize();
        m_roll_ind.initialize();
        m_compass_ind.initialize();
        m_altimeter.initialize();
        m_stick_indicator.initialize();
        m_stick_indicator.set_stick_state(&tstate);
        m_map.initialize();
        m_map.set_world(m_world);

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

    virtual void render() {
        glViewport(0, 0, m_window.current_width, m_window.current_height);
        glClearColor(0.3f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        main_view.render();
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

            main_view.resize_window(width, height);
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

        m_airplane.StepSimulation(2*fElapsed/3);
        vec3 euler_angles = make_euler_angles_from_q(m_airplane.Airplane.qOrientation);
        m_ah.rotate_to(-euler_angles.y, 0, euler_angles.x);
        m_roll_ind.rotate_to(0, 0, euler_angles.x); // done
        m_compass_ind.rotate_to(0, 0, euler_angles.z);
        m_altimeter.set_altitude(m_airplane.Airplane.vPosition.z);
        m_altimeter.set_climb(m_airplane.Airplane.vVelocity.z);
        m_altimeter.set_speed(m_airplane.Airplane.fSpeed);

        // calculate pilot's view
        vec3 z = m_airplane.GetBodyZAxisVector();
        vec3 x = m_airplane.GetBodyXAxisVector();
        vec3 vForward = clf_vector3D(x.x, x.z, x.y);
        vec3 vUp = clf_vector3D(z.x, z.z, z.y);
        x = m_airplane.Airplane.vPosition * 0.3f; // convert ft to m
        vec3 vLocation = clf_vector3D(x.x, x.z, x.y);
        main_view.get_camera()->setup_d(vLocation, vForward, vUp);  

        // calculate map view
        x = m_airplane.Airplane.vPosition * 0.3f; // convert ft to m
        vLocation = clf_vector3D(x.x, 5000, x.y);
        vForward = clf_vector3D(0, -1, 0);
        x = m_airplane.GetBodyXAxisVector();
        vUp = clf_vector3D(x.x, 0, x.y);
        m_map.get_camera()->setup_d(vLocation, vForward, vUp);
    }
};

flight_sim my_app;       // default