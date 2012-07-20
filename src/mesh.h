#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>
#include <vector>

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

    static Mesh *create(const std::string& name,
                        const std::vector<GLfloat>& vertices,
                        const std::vector<GLfloat>& normals,
                        const std::vector<GLfloat>& texcoords,
                        const std::vector<GLushort>& indices,
                        GLenum shadeModel = GL_SMOOTH);

private:
    Mesh(const std::string& name);
    GLuint m_buffers[NUM_BUFFERS];

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;

    bool m_hasNormals;
    bool m_hasTexCoords;

    const std::string m_name;
    GLenum m_shadeModel;
};

#endif // MESH_H
