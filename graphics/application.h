#ifndef __application_h__
#define __application_h__

#include <Windows.h>
#include <string>

extern "C" void __cdecl odprintf(const char* format, ...);

namespace atlas {
    class gl_shader;

    void debug_out(const char* prompt, float var);

    struct gl_window {
        std::string szTitle;
        int prefered_width;
        int prefered_height;
        int current_width;
        int current_height;
        HWND  hWnd;
        HDC   hDC;
        HGLRC hRC;
        bool isMinimized;
    };

    class gl_application {
        std::string szWindowClass;

        HINSTANCE hInst;
        bool bLooping;
        HWND create_GL_window(int wid, int hei, int bitsPerPixel, const char* title, HINSTANCE hInstance, const char* classname, int stencilBuffer);
        bool destroy_GL_window();

    protected:
        gl_shader* base_shader;
        bool b_paused = false;
    public:
        bool keyDown[256];
        gl_window m_window;

        gl_application();
        virtual ~gl_application();

        int init(HINSTANCE hInstance);
        int run();
        virtual void terminate();
        virtual void resize_window(int width, int height);

        virtual void onKeyDown(int keycode);
        virtual void onKeyUp(int keycode);
        virtual void windowMinimized(bool);
        virtual void windowMaximized();

        virtual int init_application();
        virtual void step_simulation(float fElapsed);
        virtual void pause_simulation(float fElapsed);
        virtual void resume_simulation(float fElapsed);
        virtual void restart_simulation();
        virtual void render();
        virtual void exit_application();
    };
}

#endif // __application_h__
