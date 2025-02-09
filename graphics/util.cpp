#include "graphics.h"
#include "util.h"
#include "image.h"

namespace atlas {
    GLuint load_texture(const std::string& fname)
    {
        // default return is failure
        GLuint tex = -1;

        // try to load the TGA image
        cg_image img;
        if (!img.load(fname.c_str()))
        {
            // return failure (invalid OpenGL id)
            return tex;
        }

        // how bytes are aligned in memory
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // enable textures for the following commands
        glEnable(GL_TEXTURE_2D);

        // we will generate a texture with mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        // generate texture
        glGenTextures(1, &tex);
        // and make it the current processing object
        glBindTexture(GL_TEXTURE_2D, tex);
        if (img.format() == GL_RGBA)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image());

        // how texture values (colors) are interpreted
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        // when texture area is small, bilinear filter the closest mipmap
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        // when texture area is large, bilinear filter the original
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // the texture wraps over at the edges (repeat)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // now build the mipmaps
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, img.width(), img.height(), img.format(), GL_UNSIGNED_BYTE, img.image());
        glBindTexture(GL_TEXTURE_2D, 0);
        img.release();// release the TGA image

        return tex;
    }
}