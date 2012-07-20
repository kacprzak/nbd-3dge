#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "loader.h"
#include <string>
#include <vector>
#include "vector.h"

class ObjLoader : public Loader
{
    typedef Vector<float> Vectorf;
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

        Vectorf normal(const std::vector<Vectorf>& vertices) const
        {
            Vectorf v1 = vertices[vertexIndices[0]];
            Vectorf v2 = vertices[vertexIndices[1]];
            Vectorf v3 = vertices[vertexIndices[2]];

            // Normal
            Vectorf e12 = v2 - v1;
            Vectorf e13 = v3 - v1;
            Vectorf n = e12.cross(e13);
            n.normalize();

            return n;
        }
    };

public:
    ObjLoader()
        : m_precompiledNormals(false)
        , m_shading(Smooth)
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
    void computeNormals();
    std::vector<float> expandVertices() const;
    std::vector<float> expandTexCoords() const;

    std::vector<Vectorf> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Vectorf> m_normals;
    std::vector<TexCoord> m_texCoords;

    bool m_precompiledNormals;
    ShadeModel m_shading;
};


#endif // OBJLOADER_H
