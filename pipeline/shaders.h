#pragma once

class shader_input {
public:
    // shader code
    const char* source;
    // shader type
    // GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER,
    // GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER,
    GLuint type;

    shader_input(const char* s = NULL, GLuint t = 0) :source(s), type(t) {}
};
typedef std::vector< shader_input> shader_vec;

GLuint compile_shader(shader_vec& shv);

inline void set_mat4(GLuint program, const char* name, const float* mat) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, mat);
}
