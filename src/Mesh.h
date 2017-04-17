#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>

class Mesh
{
    enum Buffers {
        VERTICES,
        TEXCOORDS,
        NORMALS,
        INDICES,
        NUM_BUFFERS
    };

public:
    ~Mesh();

    void draw() const;
    void draw(int start, int count) const;

    static Mesh *create(const std::string& objfileName);

    static Mesh *create(const std::string& name,
                        const std::vector<GLfloat>& vertices,
                        const std::vector<GLfloat>& normals,
                        const std::vector<GLfloat>& texcoords,
                        const std::vector<GLushort>& indices);

private:
    Mesh(const std::string& name);

    const std::string m_name;

    GLuint m_buffers[NUM_BUFFERS];
    GLuint m_vao;

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;
};

typedef std::shared_ptr<Mesh> MeshPtr;

#endif // MESH_H
