#include "ObjLoader.h"

#include "Util.h"
#include <boost/lexical_cast.hpp>
#include <algorithm>

static inline float to_float(const std::string& s) {
    return boost::lexical_cast<float>(s);
}

static inline int to_int(const std::string& s) {
    return boost::lexical_cast<int>(s);
}

std::vector<float> ObjLoader::vertices() const
{
    std::vector<float> vertices_a;

    if (!m_oglFaces.empty()) {
        vertices_a.reserve(m_oglVertices.size() * 3);

        for (const auto& v : m_oglVertices) {
            vertices_a.push_back(v.p.x);
            vertices_a.push_back(v.p.y);
            vertices_a.push_back(v.p.z);
        }
        return vertices_a;
    }
    
    for (const auto& v : m_vertices) {
        vertices_a.push_back(v.x);
        vertices_a.push_back(v.y);
        vertices_a.push_back(v.z);
    }
    return vertices_a;
}

std::vector<float> ObjLoader::normals() const
{
    std::vector<float> normals_a;

    if (!m_oglFaces.empty()) {
        normals_a.reserve(m_oglVertices.size() * 3);

        for(const auto& v : m_oglVertices) {
            normals_a.push_back(v.n.x);
            normals_a.push_back(v.n.y);
            normals_a.push_back(v.n.z);
        }
        return normals_a;
    }

    for (const auto& n : m_normals) {
        normals_a.push_back(n.x);
        normals_a.push_back(n.y);
        normals_a.push_back(n.z);
    }
    return normals_a;
}

std::vector<unsigned short> ObjLoader::indices() const
{
    std::vector<unsigned short> indices_a;

    if (m_oglFaces.empty())
        return indices_a;
    
    indices_a.reserve(m_oglFaces.size() * 3);
    
    for (const auto& f : m_oglFaces) {
        indices_a.push_back(f.indices[0]);
        indices_a.push_back(f.indices[1]);
        indices_a.push_back(f.indices[2]);
    }
    return indices_a;
}

std::vector<float> ObjLoader::texCoords() const
{
    std::vector<float> texcoords_a;

    if (!m_oglFaces.empty()) {
        texcoords_a.reserve(m_oglVertices.size() * 2);
        
        for (const auto& v : m_oglVertices) {
            texcoords_a.push_back(v.t.s);
            texcoords_a.push_back(v.t.t);
        }
        return texcoords_a;
    }
    
    for (const auto& t : m_texCoords) {
        texcoords_a.push_back(t.s);
        texcoords_a.push_back(t.t);
    }
    return texcoords_a;
}


void ObjLoader::command(const std::string& cmd, const std::vector<std::string>& args)
{
    if (cmd == "mtllib") {
        //loadMtlLib(args[0]);
    } else if (cmd == "v") {
        // v -42.209999 19.670004 38.799995
        glm::vec3 v;
        v.x = to_float(args[0]);
        v.y = to_float(args[1]);
        v.z = to_float(args[2]);
        m_vertices.push_back(v);

    } else if (cmd == "vn") {
        // vn -0.002913 -0.974373 -0.224919
        glm::vec3 n;
        n.x = to_float(args[0]);
        n.y = to_float(args[1]);
        n.z = to_float(args[2]);
        m_normals.push_back(n);

    } else if (cmd == "vt") {
        // vt 0.622800 0.226700
        glm::vec2 tc;
        tc.s = to_float(args[0]);
        tc.t = to_float(args[1]);
        m_texCoords.push_back(tc);

    } else if (cmd == "f") {
        //    v/vt/vn
        // f 44/61/61 56/62/62 62/63/63 
        Face f;
        std::vector<std::string> elems;

        for (int i = 0; i < 3; ++i) {
            elems.clear();
            split(args[i], '/', elems);
            f.vertexIndices[i] = to_int(elems[0]) - 1;

            if (elems.size() > 1 && !elems[1].empty())
                f.texIndices[i] = to_int(elems[1]) - 1;

            if (elems.size() > 2 && !elems[2].empty())
                f.normIndices[i] = to_int(elems[2]) - 1;
        }
        m_faces.push_back(f);

        // Quads are not supported, so one more triangle needs to be created
        if (args.size() > 3) {
            Face f2;
            elems.clear();
            split(args[3], '/', elems);

            f2.vertexIndices[0] = f.vertexIndices[2];
            f2.vertexIndices[1] = to_int(elems[0]) - 1;
            f2.vertexIndices[2] = f.vertexIndices[0];

            if (elems.size() > 1 && !elems[1].empty()) {
                f2.texIndices[0] = f.texIndices[2];
                f2.texIndices[1] = to_int(elems[1]) - 1;
                f2.texIndices[2] = f.texIndices[0];
            }

            if (elems.size() > 2 && !elems[2].empty()) {
                f2.normIndices[0] = f.normIndices[2];
                f2.normIndices[1] = to_int(elems[2]) - 1;
                f2.normIndices[2] = f.normIndices[0];
            }

            m_faces.push_back(f2);
        }
    }
}

void ObjLoader::fileLoaded()
{
    if (!m_faces.empty()) {
        m_oglVertices.reserve(m_faces.size()/3);
        m_oglFaces.reserve(m_faces.size());

        for(const Face& f : m_faces) {
            OpenGlFace face;
            for (int i = 0; i < 3; ++i) {        
                OpenGlVertex vert;
                vert.p = m_vertices[f.vertexIndices[i]];
                vert.n = !m_normals.empty() ? m_normals[f.normIndices[i]] : glm::vec3{0.0f, 0.0f, 0.0f};
                vert.t = !m_texCoords.empty() ? m_texCoords[f.texIndices[i]] : glm::vec2{0.0f, 0.0f};

                const auto& it = std::find(std::cbegin(m_oglVertices), std::cend(m_oglVertices), vert);
                if (it != std::cend(m_oglVertices)) {
                    face.indices[i] = std::distance(cbegin(m_oglVertices), it);
                } else {
                    m_oglVertices.push_back(vert);
                    face.indices[i] = m_oglVertices.size() - 1;
                }
            }
            m_oglFaces.push_back(face);
        }

        m_faces.clear();
        m_vertices.clear();
        m_normals.clear();
        m_texCoords.clear();
    }
}
