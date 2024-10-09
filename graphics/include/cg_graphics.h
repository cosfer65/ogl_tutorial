#ifndef __goggl_h__
#define __goggl_h__

#include "cg_config.h"

// OpenGL
// use wrangler headers because gl headers are not updated in windows
#define GLEW_STATIC
#include "glew.h"
#include "wglew.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

#pragma comment( lib, "cg_graphics.lib" )

#endif //__goggl_h__