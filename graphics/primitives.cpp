#include "graphics.h"
#include "primitives.h"

namespace atlas {
    static GLuint create_vbo(std::vector<float>& vertices, std::vector<unsigned short>& indices) {
        // the identifier of the buffer we will create
        GLuint vao;

        // generate the buffer
        glGenVertexArrays(1, &vao);
        // and make it current so that any subsequent calls will operate on it
        glBindVertexArray(vao);

        // we start with the vertex coordinates
        GLuint position_buffer;
        // generate buffer
        glGenBuffers(1, &position_buffer);
        // make it current
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
        // copy the data into it
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
        // set the data organization parameters
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        // enable the position data at 'location=0' (see shader!)
        glEnableVertexAttribArray(0);

        // now set the index buffer
        GLuint index_buffer;
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);

        // return the generated buffer ID
        return vao;
    }

    void gl_prim::create_from_mesh(c_mesh* mesh, GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        m_mesh_sizes.set_from_mesh(mesh->m_data);

        draw_elements = dr_el;
        draw_mode = drmode;

        int idx = 0;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint position_buffer;
        glGenBuffers(1, &position_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
        glBufferData(GL_ARRAY_BUFFER, m_mesh_sizes.num_vertices * sizeof(float), &mesh->m_data.vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(idx);

        if (m_mesh_sizes.num_normals > 0)
        {
            GLuint normals_buffer;
            glGenBuffers(1, &normals_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
            glBufferData(GL_ARRAY_BUFFER, m_mesh_sizes.num_normals * sizeof(float), &mesh->m_data.normals[0], GL_STATIC_DRAW);
            ++idx;
            glVertexAttribPointer(idx, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(idx);
        }

        if (m_mesh_sizes.num_texCoords > 0)
        {
            GLuint texture_buffer;
            glGenBuffers(1, &texture_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, texture_buffer);
            glBufferData(GL_ARRAY_BUFFER, m_mesh_sizes.num_texCoords * sizeof(float), &mesh->m_data.texCoords[0], GL_STATIC_DRAW);
            ++idx;
            glVertexAttribPointer(idx, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(idx);
        }

        GLuint index_buffer;
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh_sizes.num_indices * sizeof(unsigned short), &mesh->m_data.indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    gl_prim* create_cube(GLenum drmode/*=GL_LINE*/, bool dr_el/*=true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* ms = create_cube(1, 1, 1);
        p->create_from_mesh(ms, drmode, dr_el);
        delete ms;
        return p;
    }

    gl_prim* create_sphere(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* om = create_sphere(0.5f, 0.5f, 0.5f);
        p->create_from_mesh(om, drmode, dr_el);
        delete om;
        return p;
    }

    gl_prim* create_cylinder(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* cm = create_cylinder(0.5f, 1, 0.5f);
        p->create_from_mesh(cm, drmode, dr_el);
        delete cm;
        return p;
    }

    gl_prim* create_plane(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* tmesh = create_plane(1, 1, 1);
        p->create_from_mesh(tmesh, drmode, dr_el);
        delete tmesh;
        return p;
    }
}