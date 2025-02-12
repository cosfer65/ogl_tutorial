#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "mesh.h"
#include "amath.h"
#include "geom.h"
#include "quaternion.h"
// #include "3ds_model.h"
#include "utils.h"

namespace atlas {
    class ply_loader {
        enum read_state { IDLE, VERTEX_READ, FACE_READ };
        read_state state;
        unsigned int v_count, f_count;

        c_mesh* m_mesh;
        cg_parser parser;

        void add_vertex(str_array& star) {
            float x = (float)atof(star[0].c_str());
            float y = (float)atof(star[1].c_str());
            float z = (float)atof(star[2].c_str());
            m_mesh->addVertex(x, y, z);
            x = (float)atof(star[3].c_str());
            y = (float)atof(star[4].c_str());
            z = (float)atof(star[5].c_str());
            m_mesh->addNormal(x, y, z);
            x = (float)atof(star[6].c_str());
            y = (float)atof(star[7].c_str());
            m_mesh->addTexCoord(x, y);
        }
        void add_face(str_array& star) {
            unsigned int x = atoi(star[1].c_str());
            unsigned int y = atoi(star[2].c_str());
            unsigned int z = atoi(star[3].c_str());
            m_mesh->addIndices(x, y, z);
        }
        int parse_ply(const std::string& line) {
            str_array star;
            parser.tokenize(line, " ", star);

            if (star[0] == "ply" || star[0] == "format" || star[0] == "comment")
                return 1;

            if (star[0] == "element")
            {
                if (star[1] == "vertex")
                    v_count = atoi(star[2].c_str());
                if (star[1] == "face")
                    f_count = atoi(star[2].c_str());
            }
            if (star[0] == "end_header")
            {
                state = VERTEX_READ;
                return 1;
            }
            if (star[0] == "property")
            {
                return 1;
            }
            if (state == VERTEX_READ)
            {
                add_vertex(star);
                v_count--;
                if (v_count == 0)
                {
                    state = FACE_READ;
                    return 1;
                }
            }
            if (state == FACE_READ)
            {
                add_face(star);
                f_count--;
                if (f_count == 0)
                {
                    state = IDLE;
                    return 1;
                }
            }
            return 1;
        }
        ////////////////////////////////////////////////////////////////////////////
    public:
        ply_loader(c_mesh* pmesh) {
            m_mesh = pmesh;
        }
        ~ply_loader() {
        }

        bool load_ply(const std::string& fname) {
            state = IDLE;
            std::string line;
            std::ifstream mdl(fname);
            if (mdl.is_open())
            {
                while (std::getline(mdl, line))
                {
                    parse_ply(line);
                }
                mdl.close();
            }
            else
            {
                std::cout << "Unable to open file";
                return false;
            }

            return true;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    c_mesh::c_mesh() : num_vertices(0), num_faces(0)
    {
    }
    c_mesh::~c_mesh()
    {
    }
    void c_mesh::addVertex(float x, float y, float z) {
        m_data.vertices.push_back(x);
        m_data.vertices.push_back(y);
        m_data.vertices.push_back(z);
        ++num_vertices;
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y > maxy) maxy = y;
        if (y < miny) miny = y;
        if (z > maxz) maxz = z;
        if (z < minz) minz = z;
    }
    void c_mesh::addVertex(const vec3& v) {
        addVertex(v.x, v.y, v.z);
    }
    void c_mesh::addNormal(float x, float y, float z) {
        m_data.normals.push_back(x);
        m_data.normals.push_back(y);
        m_data.normals.push_back(z);
    }
    void c_mesh::addColor(float r, float g, float b) {
        m_data.colors.push_back(r);
        m_data.colors.push_back(g);
        m_data.colors.push_back(b);
    }
    void c_mesh::addColor(const vec3& n) {
        m_data.colors.push_back(n.x);
        m_data.colors.push_back(n.y);
        m_data.colors.push_back(n.z);
    }
    void c_mesh::addNormal(const vec3& n) {
        addNormal(n.x, n.y, n.z);
    }
    void c_mesh::addTexCoord(float s, float t) {
        m_data.texCoords.push_back(s);
        m_data.texCoords.push_back(t);
    }
    void c_mesh::addIndices(unsigned int i1, unsigned int i2, unsigned int i3) {
        m_data.indices.push_back(i1);
        m_data.indices.push_back(i2);
        m_data.indices.push_back(i3);
        vec3 p1 = vertex(i1);
        vec3 p2 = vertex(i2);
        vec3 p3 = vertex(i3);
        face_normals.push_back(calc_normal(p1, p2, p3));
        ++num_faces;
    }
    void c_mesh::move(float x, float y, float z)
    {
        for (auto i = m_data.vertices.begin(); i != m_data.vertices.end(); i += 3) {
            *i += x;
            *(i + 1) += y;
            *(i + 2) += z;
        }
    }
    void c_mesh::rotate(quaternion& q) {
        for (auto i = m_data.vertices.begin(); i != m_data.vertices.end(); i += 3) {
            vec3 p = qv_rotate(q, vec3(*i, *(i + 1), *(i + 2)));
            *i = p.x;
            *(i + 1) = p.y;
            *(i + 2) = p.z;
        }
    }
    bool c_mesh::load(const std::string& fname)
    {
        if (file_extension(fname) == "ply") {
            ply_loader mld(this);
            return mld.load_ply(fname);
        }
        // if (file_extension(fname) == "3ds") {
        //     sModel* model = load3dsmodel(fname);
        //     if (!model)
        //         return false;
        //
        //     // extract geometry
        //
        //     delete model;
        // }

        return true;
    }
    c_mesh* create_cube(float x, float y, float z) {
        c_mesh* ms = new c_mesh;

        float xh = x / 2.f;
        float yh = y / 2.f;
        float zh = z / 2.f;
        float vertex_positions[] =
        {
             xh, -yh,  zh,  xh,  yh,  zh, -xh, -yh,  zh, -xh,  yh,  zh, // front
            -xh, -yh,  zh, -xh,  yh,  zh, -xh, -yh, -zh, -xh,  yh, -zh, // left
            -xh, -yh, -zh, -xh,  yh, -zh,  xh, -yh, -zh,  xh,  yh, -zh, // back
             xh, -yh, -zh,  xh,  yh, -zh,  xh, -yh,  zh,  xh,  yh,  zh,  // right
             xh,  yh,  zh,  xh,  yh, -zh, -xh,  yh,  zh, -xh,  yh, -zh, // top
             xh, -yh, -zh,  xh, -yh,  zh, -xh, -yh, -zh, -xh, -yh,  zh, // bottom
        };
        unsigned int num_points = sizeof(vertex_positions) / sizeof(float);
        int vertex_indices[] =
        {
            0, 1, 2, 2, 1, 3,    // front
            4, 5, 6, 6, 5, 7,    // left
            8, 9, 10, 10, 9, 11, // back
            12,13,14,14,13,15,   // right
            16,17,18, 18,17,19,  // top
            20,21,22,22,21,23,   // bottom
        };
        unsigned int num_indices = sizeof(vertex_indices) / sizeof(int);

        float vertex_normals[] =
        {
            0,0,1, 0,0,1 ,0,0,1, 0,0,1,// front
            -1,0,0,-1,0,0,-1,0,0,-1,0,0,// left
            0,0,-1,0,0,-1,0,0,-1,0,0,-1,// back
            1,0,0,1,0,0,1,0,0,1,0,0,// right
            0,1,0,0,1,0,0,1,0,0,1,0,// top
            0,-1,0,0,-1,0,0,-1,0,0,-1,0,// bottom
        };
        float tex_positions[] =
        {
            1,0, 1,1, 0,0, 0,1,
            1,0, 1,1, 0,0, 0,1,
            1,0, 1,1, 0,0, 0,1,
            1,0, 1,1, 0,0, 0,1,
            1,0, 1,1, 0,0, 0,1,
            1,0, 1,1, 0,0, 0,1,
        };
        unsigned int tex_points = sizeof(tex_positions) / sizeof(float);

        for (unsigned int i = 0, j = 0; i < num_points; i += 3, j += 2) {
            ms->addVertex(vertex_positions[i], vertex_positions[i + 1], vertex_positions[i + 2]);
            ms->addNormal(vertex_normals[i], vertex_normals[i + 1], vertex_normals[i + 2]);

            ms->addTexCoord(tex_positions[j], tex_positions[j + 1]);
        }
        for (unsigned int i = 0; i < num_indices; i += 3) {
            ms->addIndices(vertex_indices[i], vertex_indices[i + 1], vertex_indices[i + 2]);
        }

        return ms;
    }
    c_mesh* create_sphere(float rx, float ry, float rz) {
        c_mesh* ms = new c_mesh;

        float radius = (rx + ry + rz) / 3.f; // 0.5f;
        int sectorCount = 36;
        int stackCount = 36;

        float x, y, z, xz;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
        float s, t;                                     // texCoord
        float sectorStep = 2 * pi / sectorCount;
        float stackStep = pi / stackCount;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = -(pi / 2 - i * stackStep);        // starting from -pi/2 to pi/2
            xz = radius * cosf(stackAngle);
            y = radius * sinf(stackAngle);

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position
                x = xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
                z = xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
                ms->addVertex(x, y, z);

                // normalized vertex normal
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                ms->addNormal(nx, ny, nz);

                // vertex tex coord between [0, 1]
                s = 1 - (float)j / sectorCount;
                t = (float)i / stackCount;
                ms->addTexCoord(s, t);
            }
        }

        // indices
        //  k1--k1+1
        //  |  / |
        //  | /  |
        //  k2--k2+1
        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding 1st and last stacks
                if (i != 0)
                    ms->addIndices(k1, k2, k1 + 1);   // k1---k2---k1+1

                if (i != (stackCount - 1))
                    ms->addIndices(k1 + 1, k2, k2 + 1); // k1+1---k2---k2+1
            }
        }

        return ms;
    }
    c_mesh* create_cylinder(float rx, float ry, float rz) {
        c_mesh* ms = new c_mesh;

        float len = ry;
        int sectorCount = 18;
        float texture_step = 1.f / (float)sectorCount;
        float angle_step = 2 * pi / sectorCount;
        float y = -len / 2;
        for (int j = 0; j < 2; ++j)
        {
            for (int i = 0; i < sectorCount + 1; ++i)// create a closing set of points
            {
                float x, z;
                float a = (float)i * angle_step;
                x = (float)cos(a) * rx;
                z = (float)sin(a) * rz;
                ms->addVertex(x, y, z);
                ms->addNormal(x, 0, z);
                ms->addTexCoord(texture_step * (float)i, (float)j);
            }
            y += len;
        }

        // indices
        //  k2--k2+1
        //  |  / |
        //  | /  |
        //  k1--k1+1
        int k1, k2;
        k1 = 0;     // beginning of bottom circle
        k2 = sectorCount + 1;      // beginning of top circle
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) // number of faces
        {
            // 2 triangles per sector
            ms->addIndices(k2, k2 + 1, k1);
            ms->addIndices(k2 + 1, k1 + 1, k1);
        }

        return ms;
    }
    c_mesh* create_plane(float x, float y, float z) {
        c_mesh* ms = new c_mesh;
        float minx = -x / 2;
        float maxx = x / 2;
        float minz = -z / 2;
        float maxz = z / 2;
        ms->addVertex(minx, 0, maxz);
        ms->addVertex(minx, 0, minz);
        ms->addVertex(maxx, 0, minz);
        ms->addVertex(maxx, 0, maxz);

        ms->addNormal(0, 1, 0);
        ms->addNormal(0, 1, 0);
        ms->addNormal(0, 1, 0);
        ms->addNormal(0, 1, 0);

        ms->addTexCoord(0, 0);
        ms->addTexCoord(0, 1);
        ms->addTexCoord(1, 1);
        ms->addTexCoord(1, 0);

        //  indices
        //  1----2
        //  |  / |
        //  | /  |
        //  0----3
        ms->addIndices(0, 2, 1);
        ms->addIndices(0, 3, 2);
        return ms;
    }
}