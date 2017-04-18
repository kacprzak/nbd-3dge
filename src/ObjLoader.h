#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Loader.h"
#include <string>
#include <vector>

class ObjLoader : public Loader
{
    struct Vectorf {
        float x;
        float y;
        float z;

        bool operator==(const Vectorf& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    struct TexCoord {
        float s;
        float t;

        bool operator==(const TexCoord& other) const
        {
            return s == other.s && t == other.t;
        }
    };

    struct Face {
        unsigned short vertexIndices[3];
        unsigned short texIndices[3];
        unsigned short normIndices[3];
    };

    struct OpenGlVertex
    {
        Vectorf p, n;
        TexCoord t;

        bool operator==(const OpenGlVertex& other) const
        {
            return p == other.p && n == other.n && t == other.t;
        }
    };
    
    struct OpenGlFace {
        unsigned indices[3];
    };

 public:
    ObjLoader() {}

    std::vector<float> vertices() const;
    std::vector<float> normals() const;
    std::vector<unsigned short> indices() const;
    std::vector<float> texCoords() const;
    int vertPerFace() const { return m_vertPerFace; }

 protected:
    void command(const std::string& cmd, const std::vector<std::string>& args);
    void fileLoaded();

 private:
    int m_vertPerFace = 3; //< triangles by deafult
    
    std::vector<Vectorf> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Vectorf> m_normals;
    std::vector<TexCoord> m_texCoords;

    std::vector<OpenGlVertex> m_oglVertices;
    std::vector<OpenGlFace> m_oglFaces;
};


#endif // OBJLOADER_H
