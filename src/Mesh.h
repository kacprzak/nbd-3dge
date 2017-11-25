#ifndef MESH_H
#define MESH_H

#include "Aabb.h"
#include "MeshData.h"

#include <GL/glew.h>
#include <string>
#include <vector>

class Mesh final
{
    enum Buffers { POSITIONS, NORMALS, TANGENTS, TEXCOORDS, INDICES, NUM_BUFFERS };

  public:
    Mesh(const MeshData& meshData);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    ~Mesh();

    void draw() const;
    void draw(int start, int count) const;

    std::vector<float> positions() const;
    Aabb aabb() const { return m_aabb; }

  private:
    GLuint m_buffers[NUM_BUFFERS];
    GLuint m_vao;

    size_t m_bufferSizes[NUM_BUFFERS];

    GLenum m_primitive;

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;

    Aabb m_aabb;
};

#endif // MESH_H
