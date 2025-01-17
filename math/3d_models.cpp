#include <iostream>
#include <fstream>
#include <regex>

#include "3d_models.h"

namespace atlas {
    //typedef basevec3<int> ivec3;
    template<typename T>
    basevec3<T> parse_vector3(const std::string& str, const std::string& delim) {
        basevec3<T> v;
        str_array star;
        cg_parser parser;

        parser.tokenize(str, delim, star);
        v.x = (T)atof(star[1].c_str());
        v.y = (T)atof(star[0].c_str());
        v.z = (T)atof(star[2].c_str());
    }

    obj_model::vertex obj_model::parse_vertex(const std::string& token) {
        obj_model::vertex v;
        str_array star;
        cg_parser parser;

        parser.tokenize(token, "/", star);
        v.x = atoi(star[0].c_str());
        v.y = atoi(star[1].c_str());
        v.z = atoi(star[2].c_str());

        return v;
    }

    void obj_model::parse_material_tokens(const str_array& tokens) {
        if (tokens[0] == "newmtl") {
            mat_lib = new mtl;
            mat_list[tokens[1]] = mat_lib;
        }
        else if (tokens[0] == "Ns") {
            mat_lib->ns = (float)atof(tokens[1].c_str());
        }
        else if (tokens[0] == "d") {
            mat_lib->d = (float)atof(tokens[1].c_str());
        }
        else if (tokens[0] == "Tr") {
            mat_lib->tr = (float)atof(tokens[1].c_str());
        }
        else if (tokens[0] == "Tf") {
            mat_lib->tf = ivec3(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()));
        }
        else if (tokens[0] == "illum") {
            mat_lib->illum = atoi(tokens[1].c_str());
        }
        else if (tokens[0] == "Ka") {
            mat_lib->ka = vec3((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
        }
        else if (tokens[0] == "Kd") {
            mat_lib->kd = vec3((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
        }
        else if (tokens[0] == "Ks") {
            mat_lib->ks = vec3((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
        }
    }

    bool obj_model::import_material_lib(const std::string& fnm) {
        std::string matlib_name = change_filename(current_file, fnm);
        cg_parser parser;
        std::string line;
        std::ifstream mdl(matlib_name);
        current = nullptr;
        if (mdl.is_open())
        {
            while (std::getline(mdl, line))
            {
                line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
                str_array star;
                parser.tokenize(line, " ", star);
                parse_material_tokens(star);
            }
            mdl.close();
        }
        else
        {
            return false;
        }

        return true;
    }

    void obj_model::parse_tokens(const str_array& tokens) {
        if (tokens[0] == "mtllib") {
            import_material_lib(tokens[1]);
        }
        else if (tokens[0] == "flip") {
            flip.x = tokens[1][0] - 'w';
            flip.y = tokens[2][0] - 'w';
        }
        else if (tokens[0] == "negate") {
            negate = tokens[1][0] - 'w';
        }
        else if (tokens[0] == "ccw") {
            ccw = atoi(tokens[1].c_str());
        }
        else if (tokens[0] == "o") {
            // new object
            current = new object;
            m_objects.push_back(current);
        }
        else if (tokens[0] == "v") {
            // add vertex
            add_vertex((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), -(float)atof(tokens[3].c_str()));
        }
        else if (tokens[0] == "vn") {
            // add normal
            add_normal((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
        }
        else if (tokens[0] == "vt") {
            // add texture coord
            add_tex_coord((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()));
        }
        else if (tokens[0] == "f") {
            if (!current) {
                current = new object;
                m_objects.push_back(current);
            }
            // add face
            obj_model::vertex v1 = parse_vertex(tokens[1]);
            obj_model::vertex v2 = parse_vertex(tokens[2]);
            obj_model::vertex v3 = parse_vertex(tokens[3]);
            // convert to CCW
            add_face(v1, v2, v3);
        }
        else if (tokens[0] == "usemtl") {
            current->m_material = mat_list[tokens[1]];
        }
    }

    bool obj_model::load(const std::string& fnm) {
        current_file = fnm;
        cg_parser parser;
        std::string line;
        std::ifstream mdl(fnm);
        current = nullptr;
        if (mdl.is_open())
        {
            while (std::getline(mdl, line))
            {
                str_array star;
                parser.tokenize(line, " ", star);
                parse_tokens(star);
            }
            mdl.close();
            fix_model();
        }
        else
        {
            return false;
        }

        return true;
    }

    void obj_model::fix_model() {
        if (flip.x != 0 && flip.y != 0) {
            for (auto& v : m_vertices) {
                float t = v[flip.x - 1];
                v[flip.x - 1] = v[flip.y - 1];
                v[flip.y - 1] = t;
            }
        }
        if (negate != 0) {
            negate--;
            for (auto& v : m_vertices) {
                v[negate] *= -1;
            }
            // for (auto& v : m_normals) {
            //    // v[negate] *= -1;
            // }
        }
        if (!ccw) {
            for (auto o : m_objects) {
                for (auto& f : *o) {
                    vertex t = f[0];
                    f[0] = f[2];
                    f[2] = t;
                }
            }
        }
    }

    void base_3d_model::invert_coordinates(const ivec3& ivt) {
        for (auto& v : m_vertices) {
            v.x *= ivt.x;
            v.y *= ivt.y;
            v.z *= ivt.z;
        }
        for (auto& v : m_normals) {
            v.x *= ivt.x;
            v.y *= ivt.y;
            v.z *= ivt.z;
        }
    }
}