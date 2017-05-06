#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Loader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

class ObjLoader : public Loader
{
    struct Face
    {
        unsigned short vertexIndices[3];
        unsigned short normIndices[3];
        unsigned short texIndices[3];
    };

    struct OpenGlVertex
    {
        glm::vec3 p, n;
        glm::vec2 t;

        bool operator==(const OpenGlVertex& other) const
        {
            return p == other.p && n == other.n && t == other.t;
        }
    };

    struct OpenGlFace
    {
        unsigned indices[3];
    };

  public:
    ObjLoader() {}

    std::vector<float> vertices() const;
    std::vector<float> normals() const;
    std::vector<unsigned short> indices() const;
    std::vector<float> texCoords() const;
    int primitive() const { return m_primitive; }

  protected:
    void command(const std::string& cmd, const std::vector<std::string>& args) override;
    void fileLoaded() override;

  private:
    GLenum m_primitive = GL_TRIANGLES; //< triangles by deafult

    std::vector<glm::vec3> m_vertices;
    std::vector<Face> m_faces;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;

    std::vector<OpenGlVertex> m_oglVertices;
    std::vector<OpenGlFace> m_oglFaces;
};

#endif // OBJLOADER_H
