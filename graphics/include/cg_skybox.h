#ifndef __skybox_h__
#define __skybox_h__

#include <vector>
#include <string>

namespace atlas {
    class cg_shader;
    class cg_viewport;
    class cg_camera;

    class cg_skybox {
        // cg_skybox VAO
        unsigned int skyboxVAO;
        unsigned int skyboxVBO;
        unsigned int textureID;
        cg_shader* default_shader;
    public:
        cg_skybox();
        ~cg_skybox();
        void load(const std::vector<std::string>& faces);
        void render(cg_viewport* vp, cg_camera* cam);
        unsigned int texture() {
            return textureID;
        }
    };
}

#endif // __skybox_h__
