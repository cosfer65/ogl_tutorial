#ifndef __cg_mesh_h__
#define __cg_mesh_h__

#include <vector>

#include "cg_vector.h"

namespace atlas {
    struct iface {
        int i1, i2, i3;
        iface() { i1 = i2 = i3 = 0; }
        iface(int _i1, int _i2, int _i3) :
            i1(_i1), i2(_i2), i3(_i3) {}
    };
    struct cg_mesh_data {
        std::vector<float> vertices;
        std::vector<float> normals;  // vertex normals
        std::vector<float> texCoords;
        std::vector<unsigned short> indices;
    };
    struct cg_mesh_sizes {
        size_t num_vertices;
        size_t num_normals;
        size_t num_texCoords;
        size_t num_indices;
        void set_from_mesh(const cg_mesh_data& mesh) {
            num_vertices = mesh.vertices.size();
            num_normals = mesh.normals.size();
            num_texCoords = mesh.texCoords.size();
            num_indices = mesh.indices.size();
        }
    };
    class cg_mesh {
    public:
        float minx;
        float maxx;
        float miny;
        float maxy;
        float minz;
        float maxz;

        cg_mesh();
        virtual ~cg_mesh();

        int num_vertices;
        int num_faces;
        cg_mesh_data m_data;
        std::vector<vec3> face_normals; // needed for collision detection

        virtual void addVertex(float x, float y, float z);
        virtual void addNormal(float x, float y, float z);
        virtual void addTexCoord(float s, float t);
        virtual void addIndices(unsigned short i1, unsigned short i2, unsigned short i3);

        virtual void create(float dx, float dy, float dz) {};
        bool load(const char* fname);

        vec3 vertex(int idx) {
            int i = idx * 3; // point to vertex array index
            return vec3(m_data.vertices[i], m_data.vertices[i + 1], m_data.vertices[i + 2]);
        }
        iface face(int idx) {
            int i = idx * 3; // point to index array index
            return iface(m_data.indices[i], m_data.indices[i + 1], m_data.indices[i + 2]);
        }
        int faces() {
            return num_faces;
        }
        int n_vertices() {
            return num_vertices;
        }
        virtual void move(float x, float y, float z);
    };
    class cg_hexa_mesh : public cg_mesh {
    public:
        cg_hexa_mesh();
        virtual ~cg_hexa_mesh();
        virtual void create(float x, float y, float z);
    };
    class cg_oblique_mesh : public cg_mesh {
    public:
        cg_oblique_mesh();
        virtual ~cg_oblique_mesh();
        virtual void create(float x, float y, float z);
    };
    class cg_cylindrical_mesh : public cg_mesh {
    public:
        cg_cylindrical_mesh();
        virtual ~cg_cylindrical_mesh();
        virtual void create(float x, float y, float z);
    };
    class cg_plane_mesh : public cg_mesh {
    public:
        cg_plane_mesh() : cg_mesh() {
        }
        virtual ~cg_plane_mesh() {}
        virtual void create(float x, float y, float z);
    };

    cg_mesh* load_model(const char* fname);
}
#endif // __cg_mesh_h__
