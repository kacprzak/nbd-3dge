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


    enum ShadeModel {
        Flat,
        Smooth
    };

    struct Face {
        unsigned short vertexIndices[3];
        unsigned short texIndices[3];
        unsigned short normIndices[3];
    };

public:
    ObjLoader()
        : m_shading(Smooth)
    {}

    void setFlatShadingModel() {
        m_shading = Flat;
    }

    const std::vector<float> vertices() const;
    const std::vector<float> normals() const;
    const std::vector<unsigned short> indices() const;
    const std::vector<float> texCoords() const;

protected:
    void command(const std::string& cmd, const std::vector<std::string>& args);
    void fileLoaded();

private:
    const std::vector<float> expandVertices() const;
    const std::vector<float> expandTexCoords() const;
    const std::vector<float> expandNormals() const;

    std::vector<Vectorf> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Vectorf> m_normals;
    std::vector<TexCoord> m_texCoords;

    ShadeModel m_shading;
};


#endif // OBJLOADER_H
