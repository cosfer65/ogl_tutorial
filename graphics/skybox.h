#ifndef __skybox_h__
#define __skybox_h__

#include <vector>
#include <string>

namespace atlas {
    class c_shader;
    class gl_viewport;
    class gl_camera;

    class c_skybox {
        // c_skybox VAO
        unsigned int skyboxVAO;
        unsigned int skyboxVBO;
        unsigned int textureID;
        c_shader* default_shader;
    public:
        c_skybox();
        ~c_skybox();
        void load(const std::vector<std::string>& faces);
        void render(gl_viewport* vp, gl_camera* cam);
        unsigned int texture() {
            return textureID;
        }
    };
}

#endif // __skybox_h__
