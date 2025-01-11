#ifndef __3d_models_h__
#define __3d_models_h__

#include "amath.h"

#include <vector>
#include <string>
#include <map>
#include "utils.h"

namespace atlas {
    class base_3d_model {

    public:
        base_3d_model() {}

        virtual bool load(const std::string& fnm) { return NULL; }

        std::vector<vec3> m_vertices;
        std::vector<vec3> m_normals;
        std::vector<vec2> m_textures;

        typedef ivec3 vertex;
        typedef std::vector<vertex> face;
        struct mtl {
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
            mtl* m_material;
        };

        std::vector<object*> m_objects;
        object* current;
        std::map<std::string, mtl*> mat_list;
        mtl* mat_lib;

        ~base_3d_model() {
            for (auto o : m_objects) delete o;
            for (auto m : mat_list) delete m.second;
        }

        void add_vertex(float x, float y, float z) {
            m_vertices.push_back(vec3(x, y, z));
        };
        void add_normal(float x, float y, float z) {
            m_normals.push_back(vec3(x, y, z));
        };
        void add_tex_coord(float x, float y) {
            m_textures.push_back(vec2(x, y));
        };
        void add_face(const vertex& v1, const vertex& v2, const vertex& v3) {
            current->push_back(face({v1,v2,v3}));
        }
        void invert_coordinates(const ivec3& ivt);
    };

    class obj_model : public base_3d_model {
        void parse_tokens(const str_array& tokens);
        vertex parse_vertex(const std::string& token);
        bool import_material_lib(const std::string& fname);
        void parse_material_tokens(const str_array& tokens);
        std::string current_file;
        ivec2 flip=ivec2(0,0);// 1=x, 2=y, 3=z --> 1,2=flip x,y etc (0,0) = do nothing
        int negate = 0;// ivec3(0, 0, 0);
        int ccw = 1;
        void fix_model();
    public:
        obj_model() :base_3d_model() {

        }
        virtual bool load(const std::string& fnm);
    };

}

#endif // __3d_models_h__
