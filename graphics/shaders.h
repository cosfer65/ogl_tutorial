#ifndef __shader_h__
#define __shader_h__

#include <vector>
#include <string>

#include "amath.h"
#include "matrix.h"

namespace atlas {
    struct shader_file {
        // Must be one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER,
        // GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER
        shader_file(GLenum _t, const std::string& _s) :shaderType(_t), shader_fname(_s) {}
        GLenum shaderType;
        std::string shader_fname;
    };
    struct shader_source {
        // Must be one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER,
        // GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER
        shader_source(GLenum _t, const std::string& _s) :shaderType(_t), shader_src(_s) {}
        GLenum shaderType;
        std::string shader_src;
    };

    class gl_shader {
        GLuint program;
        GLint old_program;
        std::vector<shader_file> sh_file_list;
    public:
        gl_shader() :program(0) {
        }
        ~gl_shader()
        {
            if (program)
                glDeleteProgram(program);
        }

        size_t add_file(GLenum _t, const std::string& _s) {
            sh_file_list.push_back(shader_file(_t, _s));
            return sh_file_list.size();
        }

        GLuint load();
        GLuint compile(const std::vector<shader_source*>& code);
        GLuint load(const char* vertex_file, const char* fragment_file, const char* geometry_file = nullptr);
        GLuint compile(const char* vertex_source, const char* fragment_source, const char* geom_source = nullptr);

        void use() {
            glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);
            glUseProgram(program);
        }
        void end() {
            glUseProgram(old_program);
        }
        void set_int(const std::string& name, int value) const {
            glUniform1i(glGetUniformLocation(program, name.c_str()), value);
        }
        void set_float(const std::string& name, float value) const {
            glUniform1f(glGetUniformLocation(program, name.c_str()), value);
        }
        void set_mat4(const std::string& name, const float* mat) const {
            glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, mat);
        }
        void set_mat4(const std::string& name, mat4& mat) const {
            glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, (float*)mat);
        }
        void set_vec4(const std::string& name, vec4& value) const {
            glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, (float*)value);
        }
        void set_vec4(const std::string& name, const float* value) const {
            glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, value);
        }
        void set_vec3(const std::string& name, vec3& value) const {
            glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, (float*)value);
        }
        void set_vec3(const std::string& name, const float* value) const {
            glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, value);
        }
    };
}

#endif // __shader_h__
