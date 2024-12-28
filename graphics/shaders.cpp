#include "graphics.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "shaders.h"

namespace atlas {
    GLuint c_shader::load()
    {
        std::vector<shader_source*> code;

        for (auto it = sh_file_list.begin(); it != sh_file_list.end(); ++it)
        {
            std::ifstream shaderFile;
            shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
                shaderFile.open((*it).shader_fname);
                std::stringstream shaderStream;
                shaderStream << shaderFile.rdbuf();
                shaderFile.close();
                shader_source* src = new shader_source((*it).shaderType, shaderStream.str());
                code.push_back(src);
            }
            catch (...)
            {
#ifdef WIN32
                OutputDebugString("error");
                OutputDebugString((*it).shader_fname.c_str());
#endif
            }
        }
        program = compile(code);
        for (auto i = code.begin(); i != code.end(); ++i)
        {
            delete* i;
        }

        return program;
    }

    GLuint c_shader::compile(const std::vector<shader_source*>& code)
    {
        std::vector<GLuint> ishaders;
        for (auto it = code.begin(); it != code.end(); ++it)
        {
            GLuint _shader = glCreateShader((*it)->shaderType);
            const char* src = (*it)->shader_src.c_str();
            glShaderSource(_shader, 1, &src, NULL);
            glCompileShader(_shader);
            ishaders.push_back(_shader);
        }
        program = glCreateProgram();
        for (auto it = ishaders.begin(); it != ishaders.end(); ++it)
            glAttachShader(program, *it);
        glLinkProgram(program);

        return program;
    }

    GLuint c_shader::load(const char* vertex_file, const char* fragment_file, const char* geometry_file /*= nullptr*/)
    {
        add_file(GL_VERTEX_SHADER, vertex_file);
        add_file(GL_FRAGMENT_SHADER, fragment_file);
        if (geometry_file)
            add_file(GL_GEOMETRY_SHADER, geometry_file);
        program = load();
        return program;
    }

    GLuint c_shader::compile(const char* vertex_source, const char* fragment_source, const char* geom_source/* = NULL*/)
    {
        std::vector<shader_source*> code;
        if (vertex_source)
            code.push_back(new shader_source(GL_VERTEX_SHADER, vertex_source));
        code.push_back(new shader_source(GL_FRAGMENT_SHADER, fragment_source));
        if (geom_source)
            code.push_back(new shader_source(GL_GEOMETRY_SHADER, geom_source));
        program = compile(code);
        for (auto i = code.begin(); i != code.end(); ++i)
        {
            delete* i;
        }
        return program;
    }
}