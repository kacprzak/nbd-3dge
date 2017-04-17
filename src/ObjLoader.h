#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Loader.h"
#include <string>
#include <vector>

class ObjLoader : public Loader
{
    typedef struct {
        float x;
        float y;
        float z;
    } Vectorf;

    typedef struct {
        float s;
        float t;
    } TexCoord;

    struct Face {
        unsigned short vertexIndices[3];
        unsigned short texIndices[3];
        unsigned short normIndices[3];
    };

 public:
    ObjLoader()
        {}

    std::vector<float> vertices() const;
    std::vector<float> normals() const;
    std::vector<unsigned short> indices() const;
    std::vector<float> texCoords() const;

 protected:
    void command(const std::string& cmd, const std::vector<std::string>& args);
    void fileLoaded();

 private:
    std::vector<float> expandVertices() const;
    std::vector<float> expandTexCoords() const;
    std::vector<float> expandNormals() const;

    std::vector<Vectorf> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Vectorf> m_normals;
    std::vector<TexCoord> m_texCoords;
};


#endif // OBJLOADER_H
