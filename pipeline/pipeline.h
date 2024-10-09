#pragma once

struct application_window {
    HWND  hWnd;
    HDC   hDC;
    HGLRC hRC;
    int   vwidth;
    int   vheight;
    bool isMinimized;
};
extern application_window g_window;

bool initialize_window(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND create_GL_window(int wid, int hei, int bitsPerPixel, const char* title, HINSTANCE hInstance, const char* classname, int stencilBuffer);
bool destroy_GL_window();

void resize_window(int width, int height);

void init_application();
void frame_render();
void terminate_application();
