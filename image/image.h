#ifndef __cg_image_h__
#define __cg_image_h__

namespace atlas {
    enum image_type { IMAGE_UNKNOWN, IMAGE_TGA, IMAGE_JPG };
    struct image_internals;
    class cg_image {
        image_internals* internals;
    public:
        cg_image();
        ~cg_image();

        bool load(const char* filename);
        void release();

        int width();
        int height();
        int format();
        unsigned char* image();
        void rotate180();
        int bytes_per_pixel();
    };
}

#pragma comment( lib, "as_image.lib" )

#endif // __cg_image_h__
