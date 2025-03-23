#ifndef __targa_h__
#define __targa_h__

#include "base_image.h"

namespace atlas {
    enum TGATypes
    {
        TGA_NODATA = 0,
        TGA_INDEXED = 1,
        TGA_RGB = 2,
        TGA_GRAYSCALE = 3,
        TGA_INDEXED_RLE = 9,
        TGA_RGB_RLE = 10,
        TGA_GRAYSCALE_RLE = 11
    };

    // Image data types
#define	IMAGE_DATA_UNSIGNED_BYTE GL_UNSIGNED_BYTE

// Pixel data transfer from file to screen:
// These masks are AND'd with the imageDesc in the TGA header,
// bit 4 is left-to-right ordering
// bit 5 is top-to-bottom
#define BOTTOM_LEFT  0x00	// first pixel is bottom left corner
#define BOTTOM_RIGHT 0x10	// first pixel is bottom right corner
#define TOP_LEFT     0x20	// first pixel is top left corner
#define TOP_RIGHT    0x30	// first pixel is top right corner

// TGA header
    struct tgaheader_t
    {
        unsigned char  idLength;
        unsigned char  colorMapType;
        unsigned char  imageTypeCode;
        unsigned char  colorMapSpec[5];
        unsigned short xOrigin;
        unsigned short yOrigin;
        unsigned short width;
        unsigned short height;
        unsigned char  bpp;
        unsigned char  imageDesc;
    };

    struct rgba_t
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    struct rgb_t
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    // tga image class
    class targa_image : public base_image
    {
    private:
        unsigned int  m_colorDepth;
        unsigned int  m_imageDataFormat;
        unsigned long  m_imageSize;

        // swap the red and blue components in the image data
        void swap_red_blue();
        // converts RGB format to RGBA format and vice versa
        bool convert_RGBA_to_RGB();
        bool convert_RGB_to_RGBA(unsigned char alphaValue);
        // flips image vertically
        bool flip_vertical();

    public:
        targa_image();
        virtual ~targa_image();

        // loading and unloading
        virtual bool load(const char* filename);
        virtual void release();
        virtual void rotate180();

        virtual int format() { return m_imageDataFormat; }
    };
}

#endif // __targa_h__
