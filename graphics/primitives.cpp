#include "3d_models.h"
#include "graphics.h"
#include "primitives.h"

namespace atlas {
    static gl_prim* create_prim(const std::vector<atlas::vec3>& verts, const std::vector<atlas::vec3>& norms, const std::vector<atlas::vec3>* colors, const std::vector<std::vector<atlas::ivec3>>& fcs);

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

        if (m_mesh_sizes.num_colors > 0)
        {
            GLuint color_buffer;
            glGenBuffers(1, &color_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
            glBufferData(GL_ARRAY_BUFFER, m_mesh_sizes.num_colors * sizeof(float), &mesh->m_data.colors[0], GL_STATIC_DRAW);
            ++idx;
            glVertexAttribPointer(idx, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(idx);
            use_vertex_color = 1;
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh_sizes.num_indices * sizeof(unsigned int), &mesh->m_data.indices[0], GL_STATIC_DRAW);
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

    gl_prim* create_cross(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* tmesh = create_cross_mesh(); // 1, 1, 1);
        p->set_draw_type(GL_LINES);
        p->create_from_mesh(tmesh, drmode, dr_el);
        delete tmesh;
        return p;
    }

    gl_prim* create_diamond(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* tmesh = create_diamond_mesh(); // 1, 1, 1);
        p->create_from_mesh(tmesh, drmode, dr_el);
        delete tmesh;
        return p;
    }

    gl_prim* create_hbar(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        gl_prim* p = new gl_prim;
        c_mesh* tmesh = create_hbar_mesh(); // 1, 1, 1);
        p->set_draw_type(GL_LINES);
        p->create_from_mesh(tmesh, drmode, dr_el);
        delete tmesh;
        return p;
    }

    inline float random_color() {
        int c = rand() % 256;
        float col = ((float)c) / 255.f;
        return col;
    }

    static gl_prim* create_prim(const std::vector<atlas::vec3>& verts, const std::vector<atlas::vec3>& norms, const std::vector<atlas::vec3>* colors, const std::vector<std::vector<atlas::ivec3>>& fcs) {
        c_mesh* m_mesh = new c_mesh;

        unsigned int count = 0;
        for (auto& f : fcs) {
            for (unsigned int i = 0; i < 3; ++i) {
                unsigned int v = f[i].x;
                m_mesh->addVertex(verts[v]);

                if (colors) {
                    // float r = random_color();
                    // float g = random_color();
                    // float b = random_color();
                    float r = (*colors)[v].x;
                    float g = (*colors)[v].y;
                    float b = (*colors)[v].z;
                    m_mesh->addColor(r, g, b);
                }

                v = f[i].z;
                m_mesh->addNormal(norms[v]);
            }
            m_mesh->addIndices(count, count + 1, count + 2);
            count += 3;
        }
        gl_prim* prim = new gl_prim;
        prim->create_from_mesh(m_mesh, GL_FILL);
        delete m_mesh;
        return prim;
    }

    gl_model::gl_model(const std::string& model_file) {
        load(model_file);
    }
    void gl_model::clean_up() {
        for (auto o : m_objects)
            delete o;
        m_objects.clear();
        if (loaded_model)
            delete loaded_model;
    }

    void gl_model::load(const std::string& model_file) {
        clean_up();
        if (file_extension(model_file) == "obj") {
            obj_model* l_model = new obj_model;
            l_model->load(model_file);
            create(l_model);
        }
        else if (file_extension(model_file) == "stl") {
            stl_model* l_model = new stl_model;
            l_model->load(model_file);
            create(l_model);
        }
        else if (file_extension(model_file) == "off") {
            off_model* l_model = new off_model;
            l_model->load(model_file);
            create(l_model);
        }
    }
    void gl_model::save(const std::string& model_file) {
        if (file_extension(model_file) == "obj") {
        }
        else if (file_extension(model_file) == "stl") {
            if (loaded_model)
                loaded_model->save(model_file);
        }
    }

    void gl_model::create(base_3d_model* model) {
        loaded_model = model;

        for (auto o : model->m_objects) {
            std::vector<atlas::vec3>* colors = NULL;
            if (model->m_colors.size() > 0)
                colors = &model->m_colors;
            gl_prim* p = create_prim(model->m_vertices, model->m_normals, colors, *o);

            base_3d_model::material* mm = o->m_material;
            if (mm) {
                cg_material* m_mat1 = new cg_material;
                m_mat1->set_ambient(mm->ka);
                m_mat1->set_diffuse(mm->kd);
                m_mat1->set_specular(mm->ks);
                m_mat1->set_shine(mm->ns);// 0.4f * 128);
                p->set_material(m_mat1);
            }
            m_objects.push_back(p);
        }
    }

    void gl_stencil::create(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        draw_elements = true;
        draw_mode = GL_FILL;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;

            tmesh->addVertex(-.2f, -.25f, 0);
            tmesh->addVertex(.2f, -.25f, 0);
            tmesh->addVertex(.2f, .25f, 0);
            tmesh->addVertex(-.2f, .25f, 0);

            tmesh->addIndices(0, 1, 2);
            tmesh->addIndices(0, 2, 3);

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
    void gl_stencil::create_elliptic(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        draw_elements = true;
        draw_mode = GL_FILL;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;
            int c = 0;
            for (float a = 0; a <= 360; a += 10) {
                float da = dtr(a);
                float x = (float)(cos(da));
                float y = (float)(sin(da));
                tmesh->addVertex(x, y, 0);
                c++;
            }
            tmesh->addVertex(0, 0, 0);//37

            for (int i = 0; i <= 36; ++i) {
                tmesh->addIndices(37, i, i + 1);
            }

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
    void gl_stencil::r_create(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        draw_elements = true;
        draw_mode = GL_FILL;

        if (vao == 0)
        {
            c_mesh* tmesh = new c_mesh;
            int c = 0;
            for (float a = 0; a <= 360; a += 10) {
                float da = dtr(a);
                float x = (float)(0.25 * cos(da));
                float y = (float)(0.25 * sin(da));
                tmesh->addVertex(x, y, 0);
                c++;
            }
            tmesh->addVertex(.25f, .25f, 0);
            tmesh->addVertex(-.25f, .25f, 0);
            tmesh->addVertex(-.25f, -.25f, 0);
            tmesh->addVertex(.25f, -.25f, 0);

            tmesh->addVertex(1, 1, 0); //37
            tmesh->addVertex(-1, 1, 0);//38
            tmesh->addVertex(-1, -1, 0);//39
            tmesh->addVertex(1, -1, 0);//40

            for (int q = 0; q < 4; ++q) {
                int j = 9 * q;
                for (int i = 0; i <= 9; ++i) {
                    tmesh->addIndices(i + j, 37 + q, i + j + 1);
                }
            }

            create_from_mesh(tmesh, GL_FILL, true);
            delete tmesh;
        }
    }
}