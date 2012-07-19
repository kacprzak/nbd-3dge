#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <string>
#include <vector>

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "vector.h"
#include "util.h"

template <typename V, typename I>
class ObjLoader
{
    typedef Vector<V> Vertex;
    typedef Vector<V> Normal;
    typedef struct {
        V s;
        V t;
    } TexCoord;


    enum ShadeModel {
        Flat,
        Smooth
    };

    struct Material {
        std::string name;
        std::string texture;
    };

    struct Face {
        I vertexIndices[3];
        I texIndices[3];

        Normal normal(const std::vector<Vertex>& vertices) const
        {
            Vertex v1 = vertices[vertexIndices[0]];
            Vertex v2 = vertices[vertexIndices[1]];
            Vertex v3 = vertices[vertexIndices[2]];

            // Normal
            Vertex e12 = v2 - v1;
            Vertex e13 = v3 - v1;
            Normal n = e12.cross(e13);
            n.normalize();

            return n;
        }
    };

public:
    ObjLoader()
        : m_precompiledNormals(false)
        , m_shading(Smooth)
    {}

    void load(const std::string& filename);

    void setFlatShadingModel()
    {
        m_shading = Flat;
    }

    const std::vector<V> vertices()
    {
        if (m_shading == Flat)
            expandVertices();

        std::vector<V> vertices_a;
        for (const Vertex& v : m_vertices) {
            vertices_a.push_back(v.x);
            vertices_a.push_back(v.y);
            vertices_a.push_back(v.z);
        }
        return vertices_a;
    }

    const std::vector<V> normals()
    {
        std::vector<V> normals_a;
        for (const Normal& n : m_normals) {
            normals_a.push_back(n.x);
            normals_a.push_back(n.y);
            normals_a.push_back(n.z);
        }
        return normals_a;
    }

    const std::vector<I> indices()
    {
        std::vector<I> indices_a;

        if (m_shading == Flat)
            return indices_a;

        for (const Face& f : m_faces) {
            indices_a.push_back(f.vertexIndices[0]);
            indices_a.push_back(f.vertexIndices[1]);
            indices_a.push_back(f.vertexIndices[2]);
        }
        return indices_a;
    }

    const std::vector<V> texCoords()
    {
        std::vector<V> texcoords_a;

        if (m_texCoords.size() == 0)
            return texcoords_a;

        if (m_shading == Flat)
            expandTexCoords();

        for (const TexCoord& t : m_texCoords) {
            texcoords_a.push_back(t.s);
            texcoords_a.push_back(t.t);
        }
        return texcoords_a;
    }

private:
    void loadMtlLib(const std::string& filename);

    void parseObjLine(const std::string& line);
    void parseMtlLine(const std::string& line);
    void computeNormals();
    void expandVertices();
    void expandTexCoords();

    std::vector<Vertex> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Normal> m_normals;
    std::vector<TexCoord> m_texCoords;

    std::vector<Material> m_materials;

    std::string m_folder;
    bool m_precompiledNormals;
    ShadeModel m_shading;
};

template <typename V, typename I>
void ObjLoader<V, I>::load(const std::string& filename)
{
    using namespace std;
    m_folder = extractDirectory(filename);

    string tmp;
    ifstream f(filename.c_str());

    if (f.is_open() == true) {
        while (f.good()) {
            getline(f, tmp);
            boost::algorithm::trim(tmp);
            parseObjLine(tmp);
        }
        f.close();
    } else {
        cerr << "Error: unable to open " << filename << endl;
    }

    computeNormals();
}

template <typename V, typename I>
void ObjLoader<V, I>::parseObjLine(const std::string& line)
{
    using namespace boost;

    // Ignoruj
    if (line.empty() || algorithm::starts_with(line, "#"))
        return;

    typedef tokenizer<char_separator<char> >
            tokenizer;

    char_separator<char> sep(" ");
    tokenizer tokens(line, sep);

    tokenizer::iterator it = tokens.begin();
    const std::string command = *it;
    ++it; // skip command;

    if (command == "mtllib") {
        loadMtlLib(*it);
    } else if (command == "v") {
        Vertex v;
        v.x = atof(it->c_str()); ++it;
        v.y = atof(it->c_str()); ++it;
        v.z = atof(it->c_str()); ++it;
        m_vertices.push_back(v);

    } else if (command == "vn") {
        m_precompiledNormals = true;
        Normal n;
        n.x = atof(it->c_str()); ++it;
        n.y = atof(it->c_str()); ++it;
        n.z = atof(it->c_str()); ++it;
        m_normals.push_back(n);

    } else if (command == "vt") {
        TexCoord tc;
        tc.s = atof(it->c_str()); ++it;
        tc.t = atof(it->c_str()); ++it;
        m_texCoords.push_back(tc);

    } else if (command == "f") {
        Face f;
        std::vector<std::string> elems;

        for (int i = 0; i < 3; ++i) {
            elems.clear();
            split(*it, '/', elems);
            f.vertexIndices[i] = atoi(elems[0].c_str()) - 1;
            if (elems.size() > 1)
                f.texIndices[i] = atoi(elems[1].c_str()) - 1;
            ++it;
        }
        m_faces.push_back(f);

        // if Quad add Triangle
        if (it != tokens.end()) {
            Face f2;
            elems.clear();
            split(*it, '/', elems);

            f2.vertexIndices[0] = f.vertexIndices[2];
            f2.vertexIndices[1] = atoi(elems[0].c_str()) - 1;
            f2.vertexIndices[2] = f.vertexIndices[0];

            if (elems.size() == 2) {
                f2.texIndices[0] = f.texIndices[2];
                f2.texIndices[1] = atoi(elems[1].c_str()) - 1;
                f2.texIndices[2] = f.texIndices[0];
            }
            m_faces.push_back(f2);
        }
    }
}

template <typename V, typename I>
void ObjLoader<V, I>::loadMtlLib(const std::string& filename)
{
    using namespace std;

    string fullpath = m_folder + filename;

    string tmp;
    ifstream f(fullpath.c_str());

    if (f.is_open() == true) {
        while (f.good()) {
            getline(f, tmp);
            boost::algorithm::trim(tmp);
            parseMtlLine(tmp);
        }
        f.close();
    } else {
        cerr << "Error: unable to open " << filename << endl;
    }
}

template <typename V, typename I>
void ObjLoader<V, I>::parseMtlLine(const std::string& line)
{
    using namespace boost;

    // Ignoruj
    if (line.empty() || algorithm::starts_with(line, "#"))
        return;

    typedef tokenizer<char_separator<char> >
            tokenizer;

    char_separator<char> sep(" ");
    tokenizer tokens(line, sep);

    tokenizer::iterator it = tokens.begin();
    const std::string command = *it;
    ++it; // skip command;

    if (command == "newmtl") {
        Material mtl;
        mtl.name = *it;
        m_materials.push_back(mtl);
    } else if (command == "map_Kd") {
        Material *mtl = &m_materials.back();
        mtl->texture = *it;
    }
}

template <typename V, typename I>
void ObjLoader<V, I>::expandVertices()
{
    std::vector<Vertex> expanded;

    for (const Face& f : m_faces) {
        expanded.push_back(m_vertices[f.vertexIndices[0]]);
        expanded.push_back(m_vertices[f.vertexIndices[1]]);
        expanded.push_back(m_vertices[f.vertexIndices[2]]);
    }
    m_vertices = expanded;
}

template <typename V, typename I>
void ObjLoader<V, I>::expandTexCoords()
{
    std::vector<TexCoord> expanded;

    for (const Face& f : m_faces) {
        expanded.push_back(m_texCoords[f.texIndices[0]]);
        expanded.push_back(m_texCoords[f.texIndices[1]]);
        expanded.push_back(m_texCoords[f.texIndices[2]]);
    }
    m_texCoords = expanded;
}

template <typename V, typename I>
void ObjLoader<V, I>::computeNormals()
{
    if (m_precompiledNormals)
        return;

    if (m_shading == Flat) {
        for (Face& f : m_faces) {
            Normal n = f.normal(m_vertices);
            m_normals.push_back(n);
            m_normals.push_back(n);
            m_normals.push_back(n);
        }
    } else {
        for (unsigned int i = 0; i < m_vertices.size(); ++i) {
            std::vector<Face> intersectFaces;
            for (const Face& f : m_faces) {
                if (f.vertexIndices[0] == i
                        || f.vertexIndices[1] == i
                        || f.vertexIndices[2] == i)
                {
                    intersectFaces.push_back(f);
                }
            }

            Normal n;
            for (const Face& f : intersectFaces) {
                n = n + f.normal(m_vertices);
            }
            n.normalize();

            m_normals.push_back(n);
        }
    }
}

#endif // OBJLOADER_H
