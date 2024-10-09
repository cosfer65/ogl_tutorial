#ifndef __cg_app__
#define __cg_app__

namespace atlas {
    class cg_shader;

    void debug_out(const char* prompt, float var);

    struct cg_window {
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

    class cg_app {
        std::string szWindowClass;

        HINSTANCE hInst;

    protected:
        cg_shader* base_shader;
    public:
        cg_window m_window;
        bool bLooping;
        bool keyDown[256];

        cg_app();
        virtual ~cg_app();

        HWND create_GL_window(int wid, int hei, int bitsPerPixel, const char* title, HINSTANCE hInstance, const char* classname, int stencilBuffer);
        bool destroy_GL_window();

        virtual int init(HINSTANCE hInstance);
        virtual int run();
        virtual void terminate();
        virtual void resize_window(int width, int height);

        virtual int init_game();
        virtual void frame_move(float fElapsed);
        virtual void frame_render();
        virtual void exit_game();
    };
}

#endif //__cg_app__
