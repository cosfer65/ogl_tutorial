#include "pch.h"
#include "cg_skybox.h"
#include "cg_util.h"
#include "cg_image.h"
#include "cg_camera.h"
#include "cg_shader.h"

namespace atlas {
    static float skyboxVertices[] = {
        // left  GL_TEXTURE_CUBE_MAP_POSITIVE_X
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,

        // right  GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // up/top  GL_TEXTURE_CUBE_MAP_POSITIVE_Y
         1.0f, 1.0f,  1.0f,
         1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
         1.0f, 1.0f,  1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f,  1.0f,

        // down/bottom  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,

        // front  GL_TEXTURE_CUBE_MAP_POSITIVE_Z
         1.0f, -1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        // back  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
    };

    static const GLchar* vertex_source =
    {
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec2 aTexCoord;\n"
        "out vec3 TexCoords;\n"
        "uniform mat4 view;\n"
        "void main() {\n"
        "   TexCoords = aPos;\n"
        "	vec4 pos = view * vec4(aPos, 1.0);\n"
        "	gl_Position = pos.xyww;\n"
        "}\n"
    };
    // Source code for fragment shader
    static const GLchar* fragment_source =
    {
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 TexCoords;\n"
        "uniform samplerCube cg_skybox;\n"
        "void main()\n"
        "{\n"
        "	FragColor = texture(cg_skybox, TexCoords);\n"
        "}\n"
    };

    cg_skybox::cg_skybox() {
    }

    cg_skybox::~cg_skybox() {
    }

    void cg_skybox::load(const std::vector<std::string>& faces)
    {
        if (skyboxVAO == 0)
        {
            // cg_skybox VAO
            glGenVertexArrays(1, &skyboxVAO);
            glGenBuffers(1, &skyboxVBO);
            glBindVertexArray(skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        }

        if (default_shader == NULL)
        {
            default_shader = new cg_shader;
            default_shader->compile(vertex_source, fragment_source);
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        for (auto i = 0; i < 6; i++)
        {
            cg_image* img = new cg_image;
            img->load(faces[i].c_str());
            // the coordinate system for cubemaps is left-handed
            // and the coordinate system of our engine is right-handed
            // so we rotate the images by 180 degrees
            img->rotate180();
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, img->format(),
                img->width(), img->height(), 0, img->format(), GL_UNSIGNED_BYTE, img->image());
            delete img;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    void cg_skybox::render(cg_viewport* vp, cg_camera* cam)
    {
        glDepthFunc(GL_LEQUAL);
        default_shader->use();
        mat4 view = vp->perspective();
        mat4 camera = cam->perspective();
        remove_translation(camera); // remove translation from the camera matrix
        mat4 cam_matrix = view * camera;
        default_shader->set_mat4("view", cam_matrix);

        // GL_CW inside the box!
        glFrontFace(GL_CW);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        default_shader->set_int("cg_skybox", GL_TEXTURE0);
        glEnable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }
}