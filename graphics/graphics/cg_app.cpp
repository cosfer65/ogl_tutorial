#include "pch.h"
#include "cg_timer.h"
#include "cg_shader.h"
#include "cg_app.h"

atlas::cg_app* theApp = NULL;

void debug_out(const char* prompt, float var)
{
    char txt[200];
    sprintf(txt, "%s:%f", prompt, var);
    OutputDebugString(txt);
}

void __cdecl odprintf(const char* format, ...)
{
    char    buf[4096], * p = buf;
    va_list args;
    int     n;

    va_start(args, format);
    n = _vsnprintf(p, sizeof buf - 3, format, args); // buf-3 is room for CR/LF/NUL
    va_end(args);

    p += (n < 0) ? sizeof buf - 3 : n;

    while (p > buf && isspace(p[-1]))
        *--p = '\0';

    *p++ = '\r';
    *p++ = '\n';
    *p = '\0';

    OutputDebugString(buf);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (theApp == NULL)
        return 0;

    if (theApp->init(hInstance))
    {
        theApp->run();
        theApp->terminate();
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_SIZE:													// window size change
        switch (wParam)												// evaluate
        {
        case SIZE_MINIMIZED:										// minimized?
            theApp->m_window.isMinimized = true;					// set isMinimized to true
            return 0;

        case SIZE_MAXIMIZED:										// maximized?
            theApp->m_window.isMinimized = false;					// set isMinimized to false
            theApp->resize_window(LOWORD(lParam), HIWORD(lParam));	// recalc view - LoWord=Width, HiWord=Height
            return 0;

        case SIZE_RESTORED:											// restored?
            theApp->m_window.isMinimized = false;					// set isMinimized to false
            theApp->resize_window(LOWORD(lParam), HIWORD(lParam));	// recalc view - LoWord=Width, HiWord=Height
            return 0;
        }
        break;
    case WM_KEYDOWN:												// Update Keyboard Buffers For keyDown Pressed
        if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
        {
            theApp->keyDown[wParam] = true;							// Set The Selected Key (wParam) To True
            if (theApp->keyDown[VK_ESCAPE])
                theApp->bLooping = false;
            return 0;												// Return
        }
        break;														// Break

    case WM_KEYUP:													// Update Keyboard Buffers For keyDown Released
        if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
        {
            theApp->keyDown[wParam] = false;						// Set The Selected Key (wParam) To False
            return 0;												// Return
        }
        break;														// Break

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
namespace atlas {
    cg_app::cg_app()
    {
        theApp = this;

        m_window.szTitle = "GusOnGames";
        m_window.prefered_width = 800;
        m_window.prefered_height = 600;
        m_window.current_width = 0;
        m_window.current_height = 0;

        szWindowClass = "GusOnGames";

        hInst = 0;
        m_window.hWnd = 0;
        m_window.hDC = 0;
        m_window.hRC = 0;
        m_window.isMinimized = false;
        bLooping = false;
    }

    cg_app::~cg_app()
    {
    }

    int cg_app::run()
    {
        // zero the keystatus buffers
        memset(keyDown, 0, sizeof(keyDown));

        // initialize wrangler library
        glewInit();
        // initialize game
        init_game();
        // start the timer before we enter the main loop
        get_global_timer()->start();
        get_global_timer()->get_elapsed_time();
        // Main message loop:
        bLooping = true;
        MSG msg;
        while (bLooping)
        {
            // check for windows message and precess them
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
            {
                if (msg.message == WM_QUIT)
                    bLooping = false;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else  // no messages, just loop for next frame
            {
                if (m_window.isMinimized)		// if window is minimized
                {
                    WaitMessage();		// yeld back to system, do not waste processing power
                }
                else
                {
                    float fElapsed = (float)get_global_timer()->get_elapsed_time();

                    // update the scene based on the elapsed time since last update
                    frame_move(fElapsed);
                    // render scene
                    frame_render();
                    // Swap Buffers (Double Buffering)
                    SwapBuffers(m_window.hDC);

                    // frame counting mechanism
                    // the higher the frame rate the faster the system
                    // if frame rate drops below 30 we are in deep trouble
                    // we should either optimize the program or buy a new computer
                    static int m_nFrames = 0;				// frame Counter
                    static float tot = 0;				// time couner
                    tot += fElapsed;						// increment counters
                    m_nFrames++;
                    if (tot >= 1.f)							// one second reached
                    {
                        char txt[200];
                        sprintf_s(txt, "%s, fps:%d", m_window.szTitle.c_str(), m_nFrames);
                        SetWindowText(m_window.hWnd, txt);
                        tot = 0;							// reset counters
                        m_nFrames = 0;
                    }
                }
            }
        }
        exit_game();
        destroy_GL_window();
        // stop the clock
        get_global_timer()->stop();

        return (int)msg.wParam;
    }

    int cg_app::init(HINSTANCE hInstance)
    {
        // Perform standard Windows application initialization
        hInst = hInstance; // Store instance handle in our global variable

        // create the app window, starting with the class
        WNDCLASSEX wcex;
        ZeroMemory(&wcex, sizeof(WNDCLASSEX));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = szWindowClass.c_str();
        wcex.hIconSm = NULL;
        if (RegisterClassEx(&wcex) == 0)
        {
            MessageBox(HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
            return false;
        }
        // create the window
        m_window.hWnd = create_GL_window(m_window.prefered_width, m_window.prefered_height, 32, m_window.szTitle.c_str(), hInstance, szWindowClass.c_str(), 1);
        // if anything goes wrong abort
        if (!m_window.hWnd)
            return FALSE;
        return 1;
    }

    void cg_app::terminate()
    {
    }

    void cg_app::resize_window(int width, int height)
    {
        m_window.current_width = width;
        m_window.current_height = height;
    }

    // This function Creates Our OpenGL Window
    HWND cg_app::create_GL_window(int wid, int hei, int bitsPerPixel, const char* title, HINSTANCE hInstance, const char* classname, int stencilBuffer)
    {
        DWORD windowStyle = WS_OVERLAPPEDWINDOW;				// define our window style
        DWORD windowExtendedStyle = WS_EX_APPWINDOW;			// define the window's extended style

        int width = wid;
        int height = hei;

        RECT windowRect = { 0, 0, width, height };				// define our window coordinates

        // adjust window, account for window borders
        AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);

        // create the opengl window
        m_window.hWnd = CreateWindowEx(windowExtendedStyle,	// extended style
            classname,								// class name
            title,									// window title
            windowStyle,							// window style
            0, 0,									// window x,y position
            windowRect.right - windowRect.left,		// window width
            windowRect.bottom - windowRect.top,		// window height
            HWND_DESKTOP,							// desktop is window's parent
            0,										// no menu
            hInstance,								// pass the window instance
            0);

        if (m_window.hWnd == 0)						// was window creation a success?
        {
            return m_window.hWnd;					// if not return false
        }

        m_window.hDC = GetDC(m_window.hWnd);		// grab a device context for this window
        if (m_window.hDC == 0)						// did we get a device context?
        {
            // Failed
            DestroyWindow(m_window.hWnd);			// destroy the window
            m_window.hWnd = 0;						// zero the window handle
            return m_window.hWnd;					// return false
        }

        PIXELFORMATDESCRIPTOR pfd =								// pfd tells windows how we want things to be
        {
            sizeof(PIXELFORMATDESCRIPTOR),						// size of this pixel format descriptor
            1,													// version number
            PFD_DRAW_TO_WINDOW |								// format must support window
            PFD_SUPPORT_OPENGL |								// format must support opengl
            PFD_DOUBLEBUFFER,									// must support double buffering
            PFD_TYPE_RGBA,										// request an rgba format
            (BYTE)bitsPerPixel,									// select our color depth
            0, 0, 0, 0, 0, 0,									// color bits ignored
            0,													// no alpha buffer
            0,													// shift bit ignored
            0,													// no accumulation buffer
            0, 0, 0, 0,											// accumulation bits ignored
            16,													// 16bit z-buffer (depth buffer)
            (BYTE)stencilBuffer,								// stencil buffer
            0,													// no auxiliary buffer
            PFD_MAIN_PLANE,										// main drawing layer
            0,													// reserved
            0, 0, 0												// layer masks ignored
        };

        GLuint PixelFormat = ChoosePixelFormat(m_window.hDC, &pfd);	// find a compatible pixel format
        if (PixelFormat == 0)						    		// did we find a compatible format?
        {
            // Failed
            ReleaseDC(m_window.hWnd, m_window.hDC);				// release our device context
            m_window.hDC = 0;							    	// zero the device context
            DestroyWindow(m_window.hWnd);						// destroy the window
            m_window.hWnd = 0;									// zero the window handle
            return m_window.hWnd;								// return false
        }

        if (SetPixelFormat(m_window.hDC, PixelFormat, &pfd) == false)// try to set the pixel format
        {
            // Failed
            ReleaseDC(m_window.hWnd, m_window.hDC);				// release our device context
            m_window.hDC = 0;									// zero the device context
            DestroyWindow(m_window.hWnd);						// destroy the window
            m_window.hWnd = 0;									// zero the window handle
            return m_window.hWnd;								// return false
        }

        m_window.hRC = wglCreateContext(m_window.hDC);			// try to get a rendering context
        if (m_window.hRC == 0)									// did we get a rendering context?
        {
            // Failed
            ReleaseDC(m_window.hWnd, m_window.hDC);			    // release our device context
            m_window.hDC = 0;									// zero the device context
            DestroyWindow(m_window.hWnd);						// destroy the window
            m_window.hWnd = 0;									// zero the window handle
            return m_window.hWnd;								// return false
        }

        // make the rendering context our current rendering context
        if (wglMakeCurrent(m_window.hDC, m_window.hRC) == false)	// failed
        {
            wglDeleteContext(m_window.hRC);							// delete the rendering context
            m_window.hRC = 0;										// zero the rendering context
            ReleaseDC(m_window.hWnd, m_window.hDC);					// release our device context
            m_window.hDC = 0;										// zero the device context
            DestroyWindow(m_window.hWnd);							// destroy the window
            m_window.hWnd = 0;										// zero the window handle
            return m_window.hWnd;									// return false
        }
        //glfwSwapInterval(0);

        ShowWindow(m_window.hWnd, SW_NORMAL);						// make the window visible
        m_window.isMinimized = false;								// set isMinimized to false

        resize_window(width, height);
        UpdateWindow(m_window.hWnd);

        return m_window.hWnd;										// window creating was a success
    }

    bool cg_app::destroy_GL_window()								// destroy the opengl window & release resources
    {
        if (m_window.hWnd != 0)										// does the window have a handle?
        {
            if (m_window.hDC != 0)									// does the window have a device context?
            {
                wglMakeCurrent(m_window.hDC, 0);					// set the current active rendering context to zero
                if (m_window.hRC != 0)								// does the window have a rendering context?
                {
                    wglDeleteContext(m_window.hRC);					// release the rendering context
                    m_window.hRC = 0;								// zero the rendering context
                }
                ReleaseDC(m_window.hWnd, m_window.hDC);				// release the device context
                m_window.hDC = 0;									// zero the device context
            }
            DestroyWindow(m_window.hWnd);							// destroy the window
            m_window.hWnd = 0;										// zero the window handle
        }

        return true;										// return true
    }

    int cg_app::init_game()
    {
        // geometry and color data are stored in the shader
        static const GLchar* vertex_source =
        {
        "#version 330 core"
        "out vec4 vs_color;"
        "void main(void)"
        "{"
            "const vec4 colors[3] = vec4[3](vec4(1, 0, 0, 1.0),"
            "vec4(0, 1, 0, 1.0),"
            "vec4(0, 0, 1, 1.0));"
            // Declare a hard-coded array of positions
            "const vec4 vertices[3] = vec4[3](vec4(-0.5, -0.5, 0, 1.0),"
            "vec4(0, 0.5, 0, 1.0),"
            "vec4(.5, -0.5, 0, 1.0));"
            // Index into our array using gl_VertexID
            "gl_Position = vertices[gl_VertexID];"
            "vs_color = colors[gl_VertexID];"
        "}"
        };
        // Source code for fragment shader
        static const GLchar* fragment_source =
        {
        "#version 330 core"
        "in vec4 vs_color;"
        "out vec4 color;"
        "void main(void)"
        "{"
        " color = vs_color;"
        "}"
        };
        base_shader = new cg_shader;
        base_shader->compile(vertex_source, fragment_source);
        return 1;
    }
    void cg_app::frame_move(float fElapsed)
    {
    }

    void cg_app::frame_render()
    {
        glViewport(0, 0, m_window.current_width, m_window.current_height);
        // clear the screen
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use shader
        base_shader->use();
        // draw the triangle (vertext coordinates are in the shader)
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void cg_app::exit_game()
    {
        delete base_shader;
    }
}