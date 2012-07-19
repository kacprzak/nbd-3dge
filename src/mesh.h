#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>

class Mesh
{
    enum Buffers {
        VERTICES,
        INDICES,
        NORMALS,
        TEXCOORDS,
        NUM_BUFFERS
    };

public:
    ~Mesh();

    void draw() const;

    static Mesh *create(const std::string& objfileName, GLenum shadeModel = GL_SMOOTH);

private:
    Mesh(const std::string& objfileName);
    GLuint m_buffers[NUM_BUFFERS];

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;
    const std::string m_objfileName;
    GLenum m_shadeModel;
};

#endif // MESH_H
