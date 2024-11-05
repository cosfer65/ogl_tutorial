#ifndef __cg_camera__
#define __cg_camera__

#include "cg_math.h"
#include "cg_matrix.h"

namespace atlas {
    class cg_camera {
        void update_camera() {
            vForward.normalize();
            vUp.normalize();
        }
    public:
        vec3 vLocation;
        vec3 vForward;
        vec3 vUp;

        cg_camera() {
            setup(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
        }
        cg_camera(const vec3& _l, const vec3 _f, const vec3& _u) {
            setup(_l, _f, _u);
        }
        cg_camera(cg_camera& cp) {
            setup(cp.vLocation, cp.vForward, cp.vUp);
        }
        ~cg_camera() {
        }
        mat4 perspective() {
            return lookAt(vLocation, vLocation + vForward, vUp);
        }

        void moveby(float fwdx, float fwdy, float fwdz) {
            vLocation.x += fwdx;
            vLocation.y += fwdy;
            vLocation.z += fwdz;
            setup(vLocation, vForward, vUp);
        }

        void moveto(float x, float y, float z) {
            vLocation.x = x;
            vLocation.y = y;
            vLocation.z = z;
            setup(vLocation, vForward, vUp);
        }

        void rotate(float x, float y, float z) {
            // x - right
            // y - up
            // z - forward
            rotate_locX(x);
            rotate_locY(y);
            rotate_locZ(z);
        }

        //void rotate_vec(const vec3& v, float x, float y, float z);
        void rotate_locX(float angle);
        void rotate_locY(float angle);
        void rotate_locZ(float angle);
        void setup(const vec3& pos, const vec3& lookat, const vec3& up);
        void setup_d(const vec3& pos, const vec3& forw, const vec3& up);
    };

    class cg_viewport {
        int width, height;  // window width, height in float
        float near_plane, far_plane;
        float zoom_angle;
        float aspect;

    public:
        cg_viewport(float _pa = 45, int _w = 800, int _h = 600, float _n = 0.1f, float _f = 1000.f) :
            zoom_angle(_pa), near_plane(_n), far_plane(_f), width(_w), height(_h), aspect((float)_w / (float)_h) {
        }
        mat4 perspective() {
            return perspective_matrix(zoom_angle, aspect, near_plane, far_plane);
        }
        mat4 ortho() {
            return ortho_matrix(zoom_angle, aspect, near_plane, far_plane);
        }
        void set_window_aspect(int _w, int _h) {
            width = _w;
            height = _h;
            aspect = (float)width / (float)height;
        }
        void set_fov(float _z) {
            zoom_angle = _z;
        }
        void set_view_field(float _n, float _f) {
            near_plane = _n;
            far_plane = _f;
        }
        void set_viewport() {
            glViewport(0, 0, width, height);
        }
    };
}
#endif // __cg_camera__
