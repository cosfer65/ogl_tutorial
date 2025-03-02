#include <iostream>
#include <fstream>
#include <regex>

#include "3d_models.h"
#include "geom.h"

namespace atlas {
    bool base_3d_model::inv_trias = false;
    bool base_3d_model::inv_norms = false;

    void base_3d_model::recenter() {
        float mnx, mxx;
        float mny, mxy;
        float mnz, mxz;
        mnx = mxx = m_vertices[0].x;
        mny = mxy = m_vertices[0].y;
        mnz = mxz = m_vertices[0].z;
        for (auto& v : m_vertices) {
            if (v.x < mnx) mnx = v.x;
            if (v.y < mny) mny = v.y;
            if (v.z < mnz) mnz = v.z;
            if (v.x > mxx) mxx = v.x;
            if (v.y > mxy) mxy = v.y;
            if (v.z > mxz) mxz = v.z;
        }

        float cx = (mxx + mnx) / 2;
        float cy = (mxy + mny) / 2;
        float cz = (mxz + mnz) / 2;

        bbox_size = vec3(mxx - mnx, mxy - mny, mxz - mnz);

        for (auto& v : m_vertices) {
            v.x -= cx;
            v.y -= cy;
            v.z -= cz;
        }
    }

    void base_3d_model::fix_model() {
        if (base_3d_model::inv_trias)
        {
            for (auto o : m_objects) {
                for (auto& f : *o) {
                    vertex t = f[0]; // vertex contains vertex, texture, normal indices
                    f[0] = f[2];
                    f[2] = t;
                }
            }
        }
        if (base_3d_model::inv_norms)
        {
            for (auto& n : m_normals) {
                n = n * -1;
            }
        }
    }

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

    vertex obj_model::parse_vertex(const std::string& token) {
        vertex v;
        str_array star;
        cg_parser parser;

        parser.tokenize(token, "/", star);
        v.x = atoi(star[0].c_str()) - 1; // convert to 0 base
        v.y = atoi(star[1].c_str()) - 1;
        v.z = atoi(star[2].c_str()) - 1;

        return v;
    }

    void obj_model::parse_material_tokens(const str_array& tokens) {
        if (tokens[0] == "newmtl") {
            mat_lib = new material;

            mat_lib->ns = 250.f;
            mat_lib->ka = vec3(0.45f, 0.45f, 0.65f);
            mat_lib->kd = vec3(0.45f, 0.45f, 0.65f);
            mat_lib->ks = vec3(0.45f, 0.45f, 0.65f);
            mat_lib->ke = vec3(0.45f, 0.45f, 0.65f);

            mat_lib->ni = 1.8f;
            mat_lib->d = 0.9f;
            mat_lib->illum = 2;
            mat_lib->tr = 1.f;
            mat_lib->tf = ivec3(0);

            mat_list[tokens[1]] = mat_lib;
        }
        else if (tokens[0] == "Ns") {
            mat_lib->ns = (float)atof(tokens[1].c_str());
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
        else if (tokens[0] == "Ke") {
            mat_lib->ke = vec3((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
        }
        else if (tokens[0] == "Ni") {
            mat_lib->ni = (float)atof(tokens[1].c_str());
        }
        else if (tokens[0] == "d") {
            mat_lib->d = (float)atof(tokens[1].c_str());
        }
        else if (tokens[0] == "illum") {
            mat_lib->illum = atoi(tokens[1].c_str());
        }
        else if (tokens[0] == "Tr") {
            mat_lib->tr = (float)atof(tokens[1].c_str());
        }
        else if (tokens[0] == "Tf") {
            mat_lib->tf = ivec3(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()));
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
            vertex v1 = parse_vertex(tokens[1]);
            vertex v2 = parse_vertex(tokens[2]);
            vertex v3 = parse_vertex(tokens[3]);
            add_face(v1, v2, v3);
        }
        else if (tokens[0] == "usemtl") {
            current->m_material = mat_list[tokens[1]];
        }
    }

    void obj_model::recalc_normals() {
        m_normals.clear();
        for (auto o : m_objects) {
            for (auto& f : *o) { // vertex contains vertex, texture, normal indices
                vec3 p1 = m_vertices[f[0].x];
                vec3 p2 = m_vertices[f[1].x];
                vec3 p3 = m_vertices[f[2].x];
                vec3 norm = calc_normal(p1, p2, p3);
                m_normals.push_back(norm);
                f[0].z = f[1].z = f[2].z = (int)m_normals.size() - 1;
            }
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
            recenter();
            fix_model();
            recalc_normals();
        }
        else
        {
            return false;
        }

        return true;
    }

    class facet {
    public:
        facet() {
        }
        ~facet() {
        }

        std::vector<vec3> vertices;
        vec3 normal;
    };

    stl_model::~stl_model() {
        for (auto f : m_facets)
            delete f;
    }

    void stl_model::parse_tokens(const str_array& tokens) {
        static facet* face = nullptr;
        if (tokens[0] == "facet") {
            face = new facet;
            m_facets.push_back(face);
        }
        if (!face) return;
        for (int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "normal") {
                face->normal = vec3((float)atof(tokens[i + 1].c_str()), (float)atof(tokens[i + 2].c_str()), (float)atof(tokens[i + 3].c_str()));
                break;
            }
            if (tokens[i] == "vertex") {
                face->vertices.push_back(vec3((float)atof(tokens[i + 1].c_str()), (float)atof(tokens[i + 2].c_str()), (float)atof(tokens[i + 3].c_str())));
                break;
            }
        }
    }

    void stl_model::build_internals() {
        current = new object;
        current->m_material = new material;

        current->m_material->ns = 250.f;
        current->m_material->ka = vec3(0.45f, 0.45f, 0.65f);
        current->m_material->kd = vec3(0.45f, 0.45f, 0.65f);
        current->m_material->ks = vec3(0.45f, 0.45f, 0.65f);
        current->m_material->ke = vec3(0.45f, 0.45f, 0.65f);

        current->m_material->ni = 1.8f;
        current->m_material->d = 0.9f;
        current->m_material->illum = 2;
        current->m_material->tr = 1.f;
        current->m_material->tf = ivec3(0);

        m_objects.push_back(current);

        for (auto& f : m_facets) {
            int v1 = (int)add_vertex(f->vertices[0]) - 1;//convert to 0 base
            int n = (int)add_normal(f->normal) - 1;
            int v2 = (int)add_vertex(f->vertices[1]) - 1;
            int v3 = (int)add_vertex(f->vertices[2]) - 1;
            add_face(vertex(v1, -1, n), vertex(v2, -1, n), vertex(v3, -1, n));
        }
    }

    bool stl_model::load(const std::string& fnm) {
        cg_parser parser;
        std::string line;
        std::ifstream mdl(fnm);
        if (mdl.is_open()) {
            while (std::getline(mdl, line))
            {
                str_array star;
                parser.tokenize(line, " ", star);
                parse_tokens(star);
            }
            mdl.close();
        }
        else {
            return false;
        }
        build_internals();
        recenter();
        fix_model();
        return true;
    }

    bool stl_model::save(const std::string& fnm) {
        std::ofstream mdl(fnm);
        mdl << "solid Object01\n";
        if (mdl.is_open()) {
            for (auto& f : m_facets) {
                mdl << "  facet normal " << f->normal.x << " " << f->normal.y << " " << f->normal.z << "\n";
                mdl << "    outer loop\n";
                for (int i = 0; i < 3; ++i)
                    mdl << "      vertex " << f->vertices[i].x << " " << f->vertices[i].y << " " << f->vertices[i].z << "\n";
                mdl << "    endloop\n";
                mdl << "  endfacet\n";
            }
            mdl << "endsolid Object01\n";
            mdl.close();
        }
        else {
            return false;
        }
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    class ifacet {
    public:
        ifacet() {
        }
        ~ifacet() {
        }

        ivec3 vertices;
        vec3 normal;
    };

    off_model::~off_model() {
    }

    bool off_model::load(const std::string& fnm) {
        cg_parser parser;
        std::string line;
        std::ifstream mdl(fnm);
        if (mdl.is_open()) {
            int max_points = -1;
            int max_trias = -1;
            int pt_count = 0;
            int tria_count = 0;
            while (std::getline(mdl, line))
            {
                str_array star;
                parser.tokenize(line, " ", star);

                if (star[0] == "OFF" || star[0] == "COFF") continue;

                if (star.size() < 3) continue; // error?
                if (max_points == -1) {
                    max_points = atoi(star[0].c_str());
                    max_trias = atoi(star[1].c_str());
                    continue;
                }
                if (pt_count < max_points) {
                    float x = (float)atof(star[0].c_str());
                    float y = (float)atof(star[1].c_str());
                    float z = (float)atof(star[2].c_str());
                    m_vertices.push_back(vec3(x, y, z));
                    if (star.size() > 5) {
                        float r = (float)atof(star[3].c_str());
                        float g = (float)atof(star[4].c_str());
                        float b = (float)atof(star[5].c_str());
                        m_colors.push_back(vec3(r, g, b));
                    }
                    pt_count++;
                    continue;
                }
                if (tria_count < max_trias) {
                    int i = atoi(star[1].c_str());
                    int j = atoi(star[2].c_str());
                    int k = atoi(star[3].c_str());
                    ifacet* face = new ifacet;
                    face->vertices = ivec3(i, j, k);
                    m_facets.push_back(face);
                    pt_count++;
                    continue;
                }
            }
            mdl.close();
            build_internals();
            recenter();
            fix_model();
        }
        else {
            return false;
        }
        return true;
    }

    void off_model::build_internals() {
        current = new object;
        current->m_material = new material;

        current->m_material->ns = 250.f;
        current->m_material->ka = vec3(0.45f, 0.45f, 0.65f);
        current->m_material->kd = vec3(0.45f, 0.45f, 0.65f);
        current->m_material->ks = vec3(0.45f, 0.45f, 0.65f);
        current->m_material->ke = vec3(0.45f, 0.45f, 0.65f);

        current->m_material->ni = 1.8f;
        current->m_material->d = 0.9f;
        current->m_material->illum = 2;
        current->m_material->tr = 1.f;
        current->m_material->tf = ivec3(0);

        m_objects.push_back(current);

        for (auto& f : m_facets) {
            int i = f->vertices.x;
            int j = f->vertices.y;
            int k = f->vertices.z;
            vec3 n = calc_normal(m_vertices[i], m_vertices[j], m_vertices[k]);
            int ni = (int)add_normal(n) - 1;
            add_face(vertex(f->vertices.x, -1, ni), vertex(f->vertices.y, -1, ni), vertex(f->vertices.z, -1, ni));
        }
    }
}