#include "pch.h"
#include "cg_image.h"
#include "targa.h"
#include "jpeg.h"
#include <string>

namespace atlas {
    struct image_internals {
        base_image* timage;
        image_internals() {
            timage = NULL;
        }
        ~image_internals() {
            release();
        }
        void init(image_type type) {
            if (type == IMAGE_TGA)
                timage = new targa_image;
            else if (type == IMAGE_JPG)
                timage = new jpeg_image;
            else
                timage = NULL;
        }
        void release() {
            if (timage)
            {
                timage->release();
                delete timage;
            }
            timage = NULL;
        }
    };

    cg_image::cg_image() {
        internals = new image_internals;
    };

    cg_image::~cg_image() {
        delete internals;
    };

    bool strings_equal(const std::string& str1, const std::string& str2)
    {
        return ((str1.size() == str2.size()) && std::equal(str1.begin(), str1.end(), str2.begin(), [](char c1, char c2) {
            return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
            }));
    }

    static image_type img_type(const std::string& filename) {
        size_t pos = filename.find_last_of(".");
        if (std::string::npos == pos)
            return IMAGE_UNKNOWN;
        std::string ext = filename.substr(pos);
        if (strings_equal(ext, ".jpg"))
            return IMAGE_JPG;
        if (strings_equal(ext, ".jpeg"))
            return IMAGE_JPG;
        if (strings_equal(ext, ".tga"))
            return IMAGE_TGA;
        if (strings_equal(ext, ".targa"))
            return IMAGE_TGA;

        return IMAGE_UNKNOWN;
    }

    bool cg_image::load(const char* filename) {
        image_type type = img_type(filename);
        internals->init(type);
        return internals->timage->load(filename);
    }

    void cg_image::release() {
        internals->timage->release();
    }

    int cg_image::width() {
        return internals->timage->width();
    }
    int cg_image::height() {
        return internals->timage->height();
    }
    int cg_image::format() {
        return internals->timage->format();
    }

    unsigned char* cg_image::image() {
        return internals->timage->image();
    }

    void cg_image::rotate180() {
        internals->timage->rotate180();
        {
        };
    }
}