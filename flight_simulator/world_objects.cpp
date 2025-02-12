#include "graphics.h"
#include "util.h"

#include "world_objects.h"

resource_manager* resource_manager::m_manager = nullptr;
std::map<std::string, GLuint> resource_manager::m_textures;

GLuint resource_manager::load_texture(const std::string & fname) {
    std::map<std::string, GLuint>::iterator t = resource_manager::m_textures.find(fname);
    if (t != resource_manager::m_textures.end())
        return t->second;
    resource_manager::m_textures[fname] = atlas::load_texture(fname);
    return resource_manager::m_textures[fname];
}
