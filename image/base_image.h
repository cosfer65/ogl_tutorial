#ifndef __base_image_h__
#define __base_image_h__

namespace atlas {
    class base_image {
    public:
        unsigned char* m_buf;
        int m_width;
        int m_height;

        base_image() :m_buf(NULL) {}
        virtual ~base_image() {}

        virtual bool load(const char* filename) = 0;
        virtual void release() = 0;
        virtual int format() = 0;
        virtual void rotate180() = 0;

        virtual int width() {
            return m_width;
        }
        virtual int height() {
            return m_height;
        }
        virtual unsigned char* image() {
            return m_buf;
        }
    };
}

#endif // __base_image_h__
