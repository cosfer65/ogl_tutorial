#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <memory.h>

#include <vector>
#include <algorithm>

#define GL_DATASIZE GL_FLOAT

// OpenGL
// use wrangler headers because gl headers are not updated in windows
#define GLEW_STATIC
#include "glew.h"
#include "wglew.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
