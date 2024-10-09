#include "pch.h"
#include "jpeg.h"
#include "glew.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "..\jpeg\jpeglib.h"
#ifdef __cplusplus
}
#endif // __cplusplus

#include <setjmp.h>

struct my_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr* my_error_ptr;
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message) (cinfo, buffer);
    longjmp(myerr->setjmp_buffer, 1);
}

static void j_putRGBScanline(unsigned char* jpegline, int widthPix, unsigned char* outBuf, int row);
static void j_putGrayScanlineToRGB(unsigned char* jpegline, int widthPix, unsigned char* outBuf, int row);
static unsigned char* JpegFileToRGB(const char* fileName, int* width, int* height);
static bool VertFlipBuf(unsigned char* inbuf, int widthBytes, int height);
static bool BGRFromRGB(unsigned char* buf, int widthPix, int height);

void j_putRGBScanline(unsigned char* jpegline, int widthPix, unsigned char* outBuf, int row)
{
    int offset = row * widthPix * 3;
    int count;
    for (count = 0; count < widthPix; count++) {
        unsigned char iRed, iBlu, iGrn;
        unsigned char* oRed, * oBlu, * oGrn;

        iRed = *(jpegline + count * 3 + 0);
        iGrn = *(jpegline + count * 3 + 1);
        iBlu = *(jpegline + count * 3 + 2);

        oRed = outBuf + offset + count * 3 + 0;
        oGrn = outBuf + offset + count * 3 + 1;
        oBlu = outBuf + offset + count * 3 + 2;

        *oRed = iRed;
        *oGrn = iGrn;
        *oBlu = iBlu;
    }
}

void j_putGrayScanlineToRGB(unsigned char* jpegline, int widthPix, unsigned char* outBuf, int row)
{
    int offset = row * widthPix * 3;
    int count;
    for (count = 0; count < widthPix; count++) {
        unsigned char iGray;
        unsigned char* oRed, * oBlu, * oGrn;

        // get our grayscale value
        iGray = *(jpegline + count);

        oRed = outBuf + offset + count * 3;
        oGrn = outBuf + offset + count * 3 + 1;
        oBlu = outBuf + offset + count * 3 + 2;

        *oRed = iGray;
        *oGrn = iGray;
        *oBlu = iGray;
    }
}

unsigned char* JpegFileToRGB(const char* fileName, int* width, int* height)
{
    *width = 0;
    *height = 0;
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    FILE* infile = NULL;		/* source file */
    JSAMPARRAY buffer;		/* Output row buffer */
    int row_stride;		/* physical row width in output buffer */

    if ((infile = fopen(fileName, "rb")) == NULL) {
        return NULL;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        if (infile != NULL)
            fclose(infile);
        return NULL;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    unsigned char* dataBuf;
    dataBuf = (unsigned char*)new unsigned char[cinfo.output_width * 3 * cinfo.output_height];
    if (dataBuf == NULL) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return NULL;
    }

    *width = cinfo.output_width;
    *height = cinfo.output_height;
    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        if (cinfo.out_color_components == 3) {
            j_putRGBScanline(buffer[0], *width, dataBuf, cinfo.output_scanline - 1);
        }
        else if (cinfo.out_color_components == 1) {
            j_putGrayScanlineToRGB(buffer[0], *width, dataBuf, cinfo.output_scanline - 1);
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return dataBuf;
}

bool VertFlipBuf(unsigned char* inbuf, int widthBytes, int height)
{
    unsigned char* tb1;
    unsigned char* tb2;

    if (inbuf == NULL)
        return FALSE;

    unsigned int bufsize;

    bufsize = widthBytes;

    tb1 = (unsigned char*)new unsigned char[bufsize];
    if (tb1 == NULL) {
        return FALSE;
    }

    tb2 = (unsigned char*)new unsigned char[bufsize];
    if (tb2 == NULL) {
        delete[] tb1;
        return FALSE;
    }

    int row_cnt;
    ULONG off1 = 0;
    ULONG off2 = 0;

    for (row_cnt = 0; row_cnt < (height + 1) / 2; row_cnt++) {
        off1 = row_cnt * bufsize;
        off2 = ((height - 1) - row_cnt) * bufsize;

        memcpy(tb1, inbuf + off1, bufsize);
        memcpy(tb2, inbuf + off2, bufsize);
        memcpy(inbuf + off1, tb2, bufsize);
        memcpy(inbuf + off2, tb1, bufsize);
    }

    delete[] tb1;
    delete[] tb2;

    return TRUE;
}

bool BGRFromRGB(unsigned char* buf, int widthPix, int height)
{
    if (buf == NULL)
        return FALSE;

    int col, row;
    for (row = 0; row < height; row++) {
        for (col = 0; col < widthPix; col++) {
            unsigned char* pRed, /* *pGrn,*/* pBlu;
            pRed = buf + row * widthPix * 3 + col * 3;
            //pGrn = buf + row * widthPix * 3 + col * 3 + 1;
            pBlu = buf + row * widthPix * 3 + col * 3 + 2;

            // swap red and blue
            unsigned char tmp;
            tmp = *pRed;
            *pRed = *pBlu;
            *pBlu = tmp;
        }
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
namespace atlas {
    jpeg_image::jpeg_image()
    {
        m_buf = NULL;
    }

    jpeg_image::~jpeg_image()
    {
        if (m_buf != NULL) {
            delete[] m_buf;
            m_buf = NULL;
        }
    }

    bool jpeg_image::load(const char* filename)
    {
        if (m_buf != NULL) {
            delete[] m_buf;
            m_buf = NULL;
        }

        m_buf = JpegFileToRGB(filename, &m_width, &m_height);
        VertFlipBuf(m_buf, m_width * 3, m_height);

        return true;
    }

    void jpeg_image::release() {
        if (m_buf != NULL) {
            delete[] m_buf;
            m_buf = NULL;
        }
    };

    int jpeg_image::format() {
        return GL_RGB;
    }
}