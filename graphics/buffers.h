#ifndef __buffers__
#define __buffers__

namespace atlas {
    class gl_frame_buffer {
    protected:
        unsigned int m_buffer;
        unsigned int m_texture;
    public:
        gl_frame_buffer() {
            m_buffer = 0;
            m_texture = 0;
        }
        virtual ~gl_frame_buffer() {
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

    class gl_texture_buffer : public gl_frame_buffer {
    public:
        gl_texture_buffer() {}
        virtual ~gl_texture_buffer() {}
        virtual unsigned int create(int w, int h);
    };

    class gl_depth_buffer : public gl_frame_buffer {
    public:
        gl_depth_buffer() {
        }
        virtual ~gl_depth_buffer() {
        }
        virtual unsigned int create(int w, int h);
    };
}

#endif // __buffers__
