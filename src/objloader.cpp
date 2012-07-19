#include "objloader.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "util.h"

const std::vector<float> ObjLoader::vertices() const
{
    if (m_shading == Flat)
        return expandVertices();

    std::vector<float> vertices_a;
    for (const Vectorf& v : m_vertices) {
        vertices_a.push_back(v.x);
        vertices_a.push_back(v.y);
        vertices_a.push_back(v.z);
    }
    return vertices_a;
}

const std::vector<float> ObjLoader::normals() const
{
    std::vector<float> normals_a;
    for (const Vectorf& n : m_normals) {
        normals_a.push_back(n.x);
        normals_a.push_back(n.y);
        normals_a.push_back(n.z);
    }
    return normals_a;
}

const std::vector<unsigned short> ObjLoader::indices() const
{
    std::vector<unsigned short> indices_a;

    if (m_shading == Flat)
        return indices_a;

    for (const Face& f : m_faces) {
        indices_a.push_back(f.vertexIndices[0]);
        indices_a.push_back(f.vertexIndices[1]);
        indices_a.push_back(f.vertexIndices[2]);
    }
    return indices_a;
}

const std::vector<float> ObjLoader::texCoords() const
{
    std::vector<float> texcoords_a;

    if (m_texCoords.size() == 0)
        return texcoords_a;

    if (m_shading == Flat)
        return expandTexCoords();

    for (const TexCoord& t : m_texCoords) {
        texcoords_a.push_back(t.s);
        texcoords_a.push_back(t.t);
    }
    return texcoords_a;
}

void ObjLoader::loadObj(const std::string& filename)
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


void ObjLoader::parseObjLine(const std::string& line)
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
        Vectorf v;
        v.x = atof(it->c_str()); ++it;
        v.y = atof(it->c_str()); ++it;
        v.z = atof(it->c_str()); ++it;
        m_vertices.push_back(v);

    } else if (command == "vn") {
        m_precompiledNormals = true;
        Vectorf n;
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

            if (elems.size() > 1) {
                f2.texIndices[0] = f.texIndices[2];
                f2.texIndices[1] = atoi(elems[1].c_str()) - 1;
                f2.texIndices[2] = f.texIndices[0];
            }
            m_faces.push_back(f2);
        }
    }
}


void ObjLoader::loadMtlLib(const std::string& filename)
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


void ObjLoader::parseMtlLine(const std::string& line)
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


std::vector<float> ObjLoader::expandVertices() const
{
    std::vector<float> expanded;

    for (const Face& f : m_faces) {
        for (int i = 0; i < 3; ++i) {
            const Vectorf& v = m_vertices[f.vertexIndices[i]];
            expanded.push_back(v.x);
            expanded.push_back(v.y);
            expanded.push_back(v.z);
        }
    }
    return expanded;
}


std::vector<float> ObjLoader::expandTexCoords() const
{
    std::vector<float> expanded;

    for (const Face& f : m_faces) {
        for (int i = 0; i < 3; ++i) {
            const TexCoord& tc = m_texCoords[f.texIndices[i]];
            expanded.push_back(tc.s);
            expanded.push_back(tc.t);
        }
    }
    return expanded;
}


void ObjLoader::computeNormals()
{
    if (m_precompiledNormals)
        return;

    if (m_shading == Flat) {
        for (Face& f : m_faces) {
            Vectorf n = f.normal(m_vertices);
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

            Vectorf n;
            for (const Face& f : intersectFaces) {
                n = n + f.normal(m_vertices);
            }
            n.normalize();

            m_normals.push_back(n);
        }
    }
}
