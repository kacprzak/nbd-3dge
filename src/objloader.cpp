/* -*- c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include "objloader.h"

#include "util.h"
#include <boost/lexical_cast.hpp>

static inline float to_float(const std::string& s) {
    return boost::lexical_cast<float>(s);
}

static inline int to_int(const std::string& s) {
    return boost::lexical_cast<int>(s);
}

const std::vector<float> ObjLoader::vertices() const
{
    if (m_shading == Flat && m_faces.size() > 0)
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

    if (m_normals.size() == 0)
        return normals_a;

    if (m_shading == Flat && m_faces.size() > 0)
        return expandNormals();

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

    if (m_shading == Flat && m_faces.size() > 0)
        return expandTexCoords();

    for (const TexCoord& t : m_texCoords) {
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
        Vectorf v;
        v.x = to_float(args[0]);
        v.y = to_float(args[1]);
        v.z = to_float(args[2]);
        m_vertices.push_back(v);

    } else if (cmd == "vn") {
        // vn -0.002913 -0.974373 -0.224919
        Vectorf n;
        n.x = to_float(args[0]);
        n.y = to_float(args[1]);
        n.z = to_float(args[2]);
        m_normals.push_back(n);

    } else if (cmd == "vt") {
        // vt 0.622800 0.226700
        TexCoord tc;
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
    //computeNormals();
}

const std::vector<float> ObjLoader::expandVertices() const
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

const std::vector<float> ObjLoader::expandNormals() const
{
    std::vector<float> expanded;

    for (const Face& f : m_faces) {
        for (int i = 0; i < 3; ++i) {
            const Vectorf& v = m_vertices[f.normIndices[i]];
            expanded.push_back(v.x);
            expanded.push_back(v.y);
            expanded.push_back(v.z);
        }
    }
    return expanded;
}

const std::vector<float> ObjLoader::expandTexCoords() const
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
