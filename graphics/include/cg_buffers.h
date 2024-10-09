#ifndef __cg_buffers__
#define __cg_buffers__

namespace atlas {
    class cg_frame_buffer {
    protected:
        unsigned int m_buffer;
        unsigned int m_texture;
    public:
        cg_frame_buffer() {
            m_buffer = 0;
            m_texture = 0;
        }
        virtual ~cg_frame_buffer() {
        }
        virtual unsigned int create(int w, int h) = 0;

        void bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
        }
        void release() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        unsigned int texture() {
            return m_texture;
        }
    };

    class cg_texture_buffer : public cg_frame_buffer {
    public:
        cg_texture_buffer() {}
        virtual ~cg_texture_buffer() {}
        virtual unsigned int create(int w, int h);
    };

    class cg_depth_buffer : public cg_frame_buffer {
    public:
        cg_depth_buffer() {
        }
        virtual ~cg_depth_buffer() {
        }
        virtual unsigned int create(int w, int h);
    };
}

#endif // __cg_buffers__
