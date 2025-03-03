#ifndef __skybox_h__
#define __skybox_h__

#include <vector>
#include <string>

namespace atlas {
    class gl_shader;
    class gl_viewport;
    class gl_camera;
    class gl_effect;

    class gl_skybox {
        // gl_skybox VAO
        unsigned int skyboxVAO;
        unsigned int skyboxVBO;
        unsigned int textureID;
        gl_shader* default_shader;
    public:
        gl_skybox();
        ~gl_skybox();
        void load_shader(const std::string& vs, const std::string& fs);
        void load(const std::vector<std::string>& faces);
        void render(gl_viewport* vp, gl_camera* cam);
        unsigned int texture() {
            return textureID;
        }
        void render_with_effects(gl_effect* effect, gl_viewport* vp, gl_camera* cam);
    };
}

#endif // __skybox_h__
