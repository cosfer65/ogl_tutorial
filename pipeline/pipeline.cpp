// this is a simple example to show the basics of the OpenGL pipeline
#include "common.h"
#include "pipeline.h"

#include "math.h"
#include "shaders.h"

// SIMPLE
// vertex shader, sets the final position for the vertex
// and passes the color to the fragment shader
const GLchar* vert_sh = { "#version 410 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aCol;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"out vec4 vs_color;\n"
"void main(){\n"
"    gl_Position = view * model * vec4(aPos, 1.0);\n"
"    vs_color = vec4(aCol, 1.0);\n"
"}\n" };
// fragment shader, sets the color for the fragment
const GLchar* frag_sh = { "#version 410 core\n"
"out vec4 color;\n"
"in vec4 vs_color;\n"
"void main(){\n"
"    color = vs_color;\n"
"}\n" };

/////////////////////////////////////////////////////////////////////////

// TESSELATION
// vertex shader reads the position only
const GLchar* vert_sh_t = { "#version 410 core\n"
"layout(location = 0) in vec3 aPos;\n"
"void main(){\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"}\n" };

// sets output color passed by evaluation shader
const GLchar* frag_sh_t = { "#version 410 core\n"
"out vec4 color;\n"
"in vec4 es_color;\n"
"void main(void)\n"
"{\n"
"    color = es_color;\n"
"}\n" };

/* The first of the three tessellation phases is the tessellation control shader
(TCS; sometimes known as simply the control shader). This shader takes its input from the
vertex shader and is primarily responsible for two things: the determination of the level
of tessellation that will be sent to the tessellation engine, and the generation of data that
will be sent to the tessellation evaluation shader that is run after tessellation has occurred. */
const GLchar* ctrl_sh = { "#version 410 core\n"
"layout(vertices = 3) out;\n"
"void main(void)\n"
"{\n"
"    if (gl_InvocationID == 0)\n"
"    {\n"
"        gl_TessLevelInner[0] = 5.0;\n"
// number of tesselations on each of the outer edges
"        gl_TessLevelOuter[0] = 5.0;\n"
"        gl_TessLevelOuter[1] = 5.0;\n"
"        gl_TessLevelOuter[2] = 5.0;\n"
"    }\n"
"    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n"
"}\n" };

/* Once the fixed-function tessellation engine has run, it produces a number of output
vertices representing the primitives it has generated. These are passed to the tessellation
evaluation shader. The tessellation evaluation shader (TES; also called simply the
evaluation shader) runs an invocation for each vertex produced by the tessellator. When
the tessellation levels are high, the tessellation evaluation shader could run an extremely
large number of times. For this reason, you should be careful with complex evaluation
shaders and high tessellation levels. */
const GLchar* eval_sh = { "#version 410 core\n"
"layout(triangles, equal_spacing, cw) in;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"out vec4 es_color;\n"
"void main(void)\n"
"{\n"
"    vec4 pos = (gl_TessCoord.x * gl_in[0].gl_Position) +\n"
"        (gl_TessCoord.y * gl_in[1].gl_Position) +\n"
"        (gl_TessCoord.z * gl_in[2].gl_Position);\n"
"    es_color = vec4(gl_TessCoord,1);\n"
"    gl_Position = view*model*pos;\n"
"}\n" };

// keep these variables global for now
// the OpenGL shader
GLuint shaderID = 0;
GLuint shaderID_t = 0;

// our geometry
GLuint vertex_array;
GLuint vertex_buffer;
GLuint colors_buffer;

void init_application()
{
    // load and compile the shader code
    shader_vec sv = {
        shader_input(vert_sh, GL_VERTEX_SHADER),
        shader_input(frag_sh, GL_FRAGMENT_SHADER)
    };
    shaderID = compile_shader(sv);

    shader_vec sv_t = {
        shader_input(vert_sh_t, GL_VERTEX_SHADER),
        shader_input(ctrl_sh, GL_TESS_CONTROL_SHADER),
        shader_input(eval_sh, GL_TESS_EVALUATION_SHADER),
        shader_input(frag_sh_t, GL_FRAGMENT_SHADER)
    };
    shaderID_t = compile_shader(sv_t);

    // the vertices of the triangle
    static float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };
    // the color at each vertex
    static float colors[] = {
    1.f, 0.f, 0.f,
    0.f, 1.f, 0.f,
    0.f, 0.f, 1.f
    };

    // create the main storage
    glGenVertexArrays(1, &vertex_array);
    // bind and use it
    glBindVertexArray(vertex_array);

    // create a buffer for the vertices
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // create a buffer for the colors
    glGenBuffers(1, &colors_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // done
    glBindVertexArray(0);
}

void frame_render()
{
    // set the viewport to the whole window
    glViewport(0, 0, g_window.vwidth, g_window.vheight);

    // set background color
    glClearColor(0.35f, 0.35f, 0.35f, 1.f);
    // clear screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = perspective_matrix(pi / 4.f, (float)(g_window.vwidth) / (float)(g_window.vheight), 1.f, 1000.f);
    mat4 model = translation_matrix(0.5, 0, -2);     // simple model to the right
    mat4 model_t = translation_matrix(-0.5, 0, -2);  // tesselated model to the left

    // select geometry to draw
    glBindVertexArray(vertex_array);

    // use tesselation shader
    glUseProgram(shaderID_t);
    // select position transformations
    set_mat4(shaderID, "model", (float*)model_t);
    set_mat4(shaderID, "view", (float*)view);
    // draw the triangle (vertext coordinates are in the shader)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_PATCHES, 0, 3);

    // use simple shader
    glUseProgram(shaderID);
    // select position transformations
    set_mat4(shaderID, "model", (float*)model);
    set_mat4(shaderID, "view", (float*)view);
    // draw the triangle (vertext coordinates are in the shader)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}

void terminate_application()
{
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &colors_buffer);

    glDeleteProgram(shaderID);
    glDeleteProgram(shaderID_t);
}