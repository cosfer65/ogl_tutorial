#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include "common.h"

#include "shaders.h"

GLuint compile_shader(shader_vec& shv)
{
#ifdef _DEBUG
    int success;
    char infoLog[512];
#endif
    std::vector<GLuint> shaders;
    GLuint program = glCreateProgram();
    for (auto i = shv.begin(); i != shv.end(); ++i)
    {
        GLuint _vs = glCreateShader(i->type);
        glShaderSource(_vs, 1, &(i->source), NULL);
        glCompileShader(_vs);
#ifdef _DEBUG
        glGetShaderiv(_vs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(_vs, 512, NULL, infoLog);
            OutputDebugString(infoLog);
        }
#endif
        glAttachShader(program, _vs);
        shaders.push_back(_vs);
    }
    glLinkProgram(program);

    // delete shaders
    std::for_each(shaders.begin(), shaders.end(), [](GLuint _vs) {glDeleteShader(_vs); });

    return program;
}