#ifndef __mesh_h__
#define __mesh_h__

#include <vector>
#include <string>

#include "vector.h"

namespace atlas {
    class quaternion;

    struct iface {
        int i1, i2, i3;
        iface() { i1 = i2 = i3 = 0; }
        iface(int _i1, int _i2, int _i3) :
            i1(_i1), i2(_i2), i3(_i3) {
        }
    };
    struct iedge {
        vec3 s;
        vec3 t;
        iedge(vec3 _s = vec3(0), const vec3 _t = vec3(0)) :s(_s), t(_t) {}
    };
    struct mesh_data {
        std::vector<float> vertices;
        std::vector<float> normals;  // vertex normals
        std::vector<float> colors;   // per vertex color
        std::vector<float> texCoords;
        std::vector<unsigned short> indices;
    };
    struct mesh_sizes {
        size_t num_vertices;
        size_t num_normals;
        size_t num_colors;
        size_t num_texCoords;
        size_t num_indices;
        void set_from_mesh(const mesh_data& mesh) {
            num_vertices = mesh.vertices.size();
            num_normals = mesh.normals.size();
            num_texCoords = mesh.texCoords.size();
            num_indices = mesh.indices.size();
            num_colors = mesh.colors.size();
        }
    };
    class c_mesh {
    public:
        float minx;
        float maxx;
        float miny;
        float maxy;
        float minz;
        float maxz;

        c_mesh();
        virtual ~c_mesh();

        int num_vertices;
        int num_faces;
        mesh_data m_data;
        std::vector<vec3> face_normals; // needed for collision detection

        virtual void addVertex(float x, float y, float z);
        virtual void addVertex(const vec3& v);
        virtual void addNormal(float x, float y, float z);
        virtual void addColor(float r, float g, float b);
        virtual void addColor(const vec3& n);
        virtual void addNormal(const vec3& n);
        virtual void addTexCoord(float s, float t);
        virtual void addIndices(unsigned short i1, unsigned short i2, unsigned short i3);

        virtual void create(float dx, float dy, float dz) {};
        bool load(const std::string& fname);

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
        int n_colors() {
            return (int)m_data.colors.size();
        }
        virtual void move(float x, float y, float z);
        virtual void rotate(quaternion& q);
    };

    c_mesh* create_cube(float x, float y, float z);
    c_mesh* create_sphere(float x, float y, float z);
    c_mesh* create_cylinder(float x, float y, float z);
    // create a rectangular surface at the x-z plane and y=0
    // it extends -x/2 -> x/2  -z/2->z/2 y=0-parameter ignored
    c_mesh* create_plane(float x, float y, float z);
}
#endif // __mesh_h__
