#ifndef __mesh_h__
#define __mesh_h__

#include <vector>
#include <string>

#include "vector.h"
#include "matrix.h"

namespace atlas {
    class quaternion;

    struct iface {
        unsigned int i1, i2, i3;
        iface() { i1 = i2 = i3 = 0; }
        iface(unsigned int _i1, unsigned int _i2, unsigned int _i3) :
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
        std::vector<unsigned int> indices;
    };
    struct mesh_sizes {
        unsigned int num_vertices;
        unsigned int num_normals;
        unsigned int num_colors;
        unsigned int num_texCoords;
        unsigned int num_indices;
        void set_from_mesh(const mesh_data& mesh) {
            num_vertices = (unsigned int)mesh.vertices.size();
            num_normals = (unsigned int)mesh.normals.size();
            num_texCoords = (unsigned int)mesh.texCoords.size();
            num_indices = (unsigned int)mesh.indices.size();
            num_colors = (unsigned int)mesh.colors.size();
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

        unsigned int num_vertices;
        unsigned int num_faces;
        mesh_data m_data;
        // std::vector<vec3> face_normals; // needed for collision detection

        virtual void addVertex(float x, float y, float z);
        virtual void addVertex(const vec3& v);
        virtual void addNormal(float x, float y, float z);
        virtual void addColor(float r, float g, float b);
        virtual void addColor(const vec3& n);
        virtual void addNormal(const vec3& n);
        virtual void addTexCoord(float s, float t);
        virtual void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
        virtual void addIndices_n(unsigned int i1, unsigned int i2, unsigned int i3);
        virtual void addIndices(unsigned int i1, unsigned int i2);

        virtual void create(float dx, float dy, float dz) {};
        bool load(const std::string& fname);

        vec3 vertex(unsigned int idx) {
            unsigned int i = idx * 3; // point to vertex array index
            return vec3(m_data.vertices[i], m_data.vertices[i + 1], m_data.vertices[i + 2]);
        }
        iface face(unsigned int idx) {
            unsigned int i = idx * 3; // point to index array index
            return iface(m_data.indices[i], m_data.indices[i + 1], m_data.indices[i + 2]);
        }
        unsigned int faces() {
            return num_faces;
        }
        unsigned int n_vertices() {
            return num_vertices;
        }
        unsigned int n_colors() {
            return (unsigned int)m_data.colors.size();
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
    c_mesh* create_cross_mesh();
    c_mesh* create_diamond_mesh();
    c_mesh* create_hbar_mesh();
    c_mesh* create_UCS_mesh();
    c_mesh* create_heightmap_mesh(const matrix<float>& hmap, float length_scale);
}
#endif // __mesh_h__
