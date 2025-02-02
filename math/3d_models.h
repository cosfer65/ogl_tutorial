#ifndef __3d_models_h__
#define __3d_models_h__

#include "amath.h"

#include <vector>
#include <string>
#include <map>
#include "utils.h"

namespace atlas {
    typedef ivec3 vertex;

    class base_3d_model {
    public:
        static int inv_x;
        static int inv_y;
        static int inv_z;
        static int flip_xy;
        static int flip_xz;
        static int flip_yz;

        base_3d_model() {}

        virtual bool load(const std::string& fnm) = 0;
        virtual bool save(const std::string& fnm) { return true; }

        std::vector<vec3> m_vertices;
        std::vector<vec3> m_normals;
        std::vector<vec3> m_colors;
        std::vector<vec2> m_textures;

        typedef std::vector<vertex> face;
        struct material {
            float ns;
            float d;
            float tr;
            ivec3 tf;
            int illum;
            vec3 ka;
            vec3 kd;
            vec3 ks;
        };
        class object : public std::vector<face> {
        public:
            object() {}
            material* m_material;
        };

        std::vector<object*> m_objects;
        object* current;
        std::map<std::string, material*> mat_list;
        material* mat_lib;

        virtual ~base_3d_model() {
            for (auto o : m_objects) delete o;
            for (auto m : mat_list) delete m.second;
        }

        size_t add_vertex(float x, float y, float z) {
            m_vertices.push_back(vec3(x, y, z));
            return m_vertices.size(); // index of last insert!
        };
        size_t add_vertex(const vec3& v) {
            m_vertices.push_back(v);
            return m_vertices.size(); // index of last insert!
        };
        size_t add_normal(float x, float y, float z) {
            m_normals.push_back(vec3(x, y, z));
            return m_normals.size(); // index of last insert!
        };
        size_t add_normal(const vec3& n) {
            m_normals.push_back(n);
            return m_normals.size(); // index of last insert!
        };
        size_t add_color(float r, float g, float b) {
            m_colors.push_back(vec3(r, g, b));
            return m_colors.size(); // index of last insert!
        };
        size_t add_color(const vec3& n) {
            m_colors.push_back(n);
            return m_colors.size(); // index of last insert!
        };
        size_t add_tex_coord(float x, float y) {
            m_textures.push_back(vec2(x, y));
            return m_textures.size(); // index of last insert!
        };
        size_t add_face(const vertex& v1, const vertex& v2, const vertex& v3) {
            current->push_back(face({ v1,v2,v3 })); // vertex, texture, normal indices per vertex
            return current->size(); // index of last insert!
        }
        void invert_coordinates(const ivec3& ivt);
    };

    class obj_model : public base_3d_model {
        void parse_tokens(const str_array& tokens);
        vertex parse_vertex(const std::string& token);
        bool import_material_lib(const std::string& fname);
        void parse_material_tokens(const str_array& tokens);
        std::string current_file;
        ivec2 flip = ivec2(0, 0);// 1=x, 2=y, 3=z --> 1,2=flip x,y etc (0,0) = do nothing
        int negate = 0;// ivec3(0, 0, 0);
        int ccw = 1;
        void fix_model();
    public:
        obj_model() :base_3d_model() {
        }
        virtual bool load(const std::string& fnm);
    };

    class facet;
    class stl_model : public base_3d_model {
        void parse_tokens(const str_array& tokens);
        std::vector<facet*> m_facets;
        void build_internals();
    public:
        stl_model() :base_3d_model() {
        }
        virtual ~stl_model();
        virtual bool load(const std::string& fnm);
        virtual bool save(const std::string& fnm);
    };

    class ifacet;
    class off_model : public base_3d_model {
        std::vector<ifacet*> m_facets;
        void build_internals();
    public:
        off_model() :base_3d_model() {
        }
        virtual ~off_model();
        virtual bool load(const std::string& fnm);
        // virtual bool save(const std::string& fnm);
    };
}

#endif // __3d_models_h__
