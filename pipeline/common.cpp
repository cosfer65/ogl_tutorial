#include "common.h"
#include "timer.h"
#include "pipeline.h"
#include "resource.h"

application_window g_window;

////////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    // if initialization fails, abort
    if (!initialize_window(hInstance, nCmdShow))
        return 0;

    // initialize wrangler library
    glewInit();

    // now we can initialize our game
    // we can load any resources we want
    // and prepare everything
    init_application();

    // start the timer before we enter the main loop
    get_global_timer()->start();
    get_global_timer()->get_elapsed_time();

    bool bLooping = true;
    MSG msg;
    while (bLooping)
    {
        // check for windows message and precess them
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
        {
            if (msg.message == WM_QUIT)  // destroy window was invoked (escape or Alt-F4)
                bLooping = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else  // no messages, just loop for next frame
        {
            if (g_window.isMinimized)		// if window is minimized
            {
                WaitMessage();		// yeld back to system, do not waste processing power
            }
            else
            {
                float fElapsed = (float)get_global_timer()->get_elapsed_time();

                // render scene
                render();
                // Swap Buffers (Double Buffering)
                SwapBuffers(g_window.hDC);

                // frame counting mechanism
                static int m_nFrames = 0;				// frame Counter
                static float tot = 0;					// time couner
                tot += fElapsed;						// increment counters
                m_nFrames++;
                if (tot >= 1.f)							// one second reached
                {
                    char txt[200];
                    sprintf_s(txt, "GusOnGames (pipeline), fps:%d", m_nFrames);
                    SetWindowText(g_window.hWnd, txt);
                    tot = 0;							// reset counters
                    m_nFrames = 0;
                }
            }
        }
    }

    // here we can clean up before exiting the game
    terminate_application();

    // destroy the window returning any locked resources to the system
    destroy_GL_window();

    return (int)msg.wParam;
}

bool initialize_window(HINSTANCE hInstance, int nCmdShow)
{
    // Initialize strings
    // The title bar text
    char szTitle[100];
    // the main window class name
    char szWindowClass[100];
    LoadString(hInstance, IDS_APP_TITLE, szTitle, 100);
    LoadString(hInstance, IDC_PIPELINE, szWindowClass, 100);

    // the window class (not to be confused with C++ class)
    // this is a description of our window
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    // window style
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // callback function
    // this is called from the 'DispatchMessage'
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    // application icon, muse cursor, background color, menu
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIPELINE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    // let the system store our options
    RegisterClassEx(&wcex);

    // now ask the system to create our window
    HWND hWnd = create_GL_window(800, 600, 32, szTitle, hInstance, szWindowClass, 1);

    // on error just abort
    if (!hWnd)
    {
        return false;
    }
    // show the window
    ShowWindow(hWnd, nCmdShow);

    // return sucess
    return true;
}

// the callback windows calls when needed via DispatchMessage
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
            g_window.isMinimized = true;							// set isMinimized to true
            return 0;

        case SIZE_MAXIMIZED:										// maximized?
            g_window.isMinimized = false;							// set isMinimized to false
            resize_window(LOWORD(lParam), HIWORD(lParam));		    // recalc view - LoWord=Width, HiWord=Height
            return 0;

        case SIZE_RESTORED:											// restored?
            g_window.isMinimized = false;							// set isMinimized to false
            resize_window(LOWORD(lParam), HIWORD(lParam));		    // recalc view - LoWord=Width, HiWord=Height
            return 0;
        }
        break;
    case WM_KEYDOWN:												// Update Keyboard Buffers For keyDown Pressed
        if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
        {
            if (wParam == VK_ESCAPE)                                // escape key = exit
                DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_KEYUP:
        break;

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

// This function creates our OpenGL Window
HWND create_GL_window(int wid, int hei, int bitsPerPixel, const char* title, HINSTANCE hInstance, const char* classname, int stencilBuffer)
{
    DWORD windowStyle = WS_OVERLAPPEDWINDOW;				// define our window style
    DWORD windowExtendedStyle = WS_EX_APPWINDOW;			// define the window's extended style

    int width = wid;
    int height = hei;

    RECT windowRect = { 0, 0, width, height };				// define our window coordinates

    // adjust window, account for window borders
    AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
#if 0
    // create the opengl window
    g_window.hWnd = CreateWindowEx(windowExtendedStyle,	// extended style
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
#endif
    g_window.hWnd = CreateWindow(classname, title, WS_OVERLAPPEDWINDOW,
        0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

    if (g_window.hWnd == 0)								// was window creation a success?
    {
        return g_window.hWnd;							// if not return false
    }

    g_window.hDC = GetDC(g_window.hWnd);							// grab a device context for this window
    if (g_window.hDC == 0)								// did we get a device context?
    {
        // Failed
        DestroyWindow(g_window.hWnd);					// destroy the window
        g_window.hWnd = 0;								// zero the window handle
        return g_window.hWnd;							// return false
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

    GLuint PixelFormat = ChoosePixelFormat(g_window.hDC, &pfd);	// find a compatible pixel format
    if (PixelFormat == 0)								        // did we find a compatible format?
    {
        // Failed
        ReleaseDC(g_window.hWnd, g_window.hDC);					// release our device context
        g_window.hDC = 0;										// zero the device context
        DestroyWindow(g_window.hWnd);							// destroy the window
        g_window.hWnd = 0;										// zero the window handle
        return g_window.hWnd;									// return false
    }

    if (SetPixelFormat(g_window.hDC, PixelFormat, &pfd) == false)// try to set the pixel format
    {
        // Failed
        ReleaseDC(g_window.hWnd, g_window.hDC);					// release our device context
        g_window.hDC = 0;										// zero the device context
        DestroyWindow(g_window.hWnd);							// destroy the window
        g_window.hWnd = 0;										// zero the window handle
        return g_window.hWnd;									// return false
    }

    g_window.hRC = wglCreateContext(g_window.hDC);				// try to get a rendering context
    if (g_window.hRC == 0)										// did we get a rendering context?
    {
        // Failed
        ReleaseDC(g_window.hWnd, g_window.hDC);					// release our device context
        g_window.hDC = 0;										// zero the device context
        DestroyWindow(g_window.hWnd);							// destroy the window
        g_window.hWnd = 0;										// zero the window handle
        return g_window.hWnd;									// return false
    }

    // make the rendering context our current rendering context
    if (wglMakeCurrent(g_window.hDC, g_window.hRC) == false)	// failed
    {
        wglDeleteContext(g_window.hRC);							// delete the rendering context
        g_window.hRC = 0;										// zero the rendering context
        ReleaseDC(g_window.hWnd, g_window.hDC);					// release our device context
        g_window.hDC = 0;										// zero the device context
        DestroyWindow(g_window.hWnd);							// destroy the window
        g_window.hWnd = 0;										// zero the window handle
        return g_window.hWnd;									// return false
    }
    //glfwSwapInterval(0);

    ShowWindow(g_window.hWnd, SW_NORMAL);						// make the window visible
    g_window.isMinimized = false;								// set isMinimized to false

    resize_window(width, height);
    // UpdateWindow(g_window.hWnd);

    return g_window.hWnd;										// window creating was a success
}

bool destroy_GL_window()								// destroy the opengl window & release resources
{
    if (g_window.hWnd != 0)								// does the window have a handle?
    {
        if (g_window.hDC != 0)							// does the window have a device context?
        {
            wglMakeCurrent(g_window.hDC, 0);			// set the current active rendering context to zero
            if (g_window.hRC != 0)						// does the window have a rendering context?
            {
                wglDeleteContext(g_window.hRC);			// release the rendering context
                g_window.hRC = 0;						// zero the rendering context
            }
            ReleaseDC(g_window.hWnd, g_window.hDC);		// release the device context
            g_window.hDC = 0;							// zero the device context
        }
        DestroyWindow(g_window.hWnd);					// destroy the window
        g_window.hWnd = 0;								// zero the window handle
    }

    return true;										// return true
}

void resize_window(int width, int height)
{
    g_window.vwidth = width;
    g_window.vheight = height;
}