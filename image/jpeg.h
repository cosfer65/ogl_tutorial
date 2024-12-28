#ifndef __jpeg_h__
#define __jpeg_h__

#include "base_image.h"

namespace atlas {
    class jpeg_image :public base_image
    {
    public:
        jpeg_image();
        virtual ~jpeg_image();

        virtual bool load(const char* filename);
        virtual void release();
        virtual int format();
        virtual void rotate180() {}
    };
}

#endif // __jpeg_h__
