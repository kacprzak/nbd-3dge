#ifndef MESH_H
#define MESH_H

#include "Aabb.h"
#include "Material.h"
#include "MeshData.h"
#include "ShaderProgram.h"

#include <GL/glew.h>

class Mesh final
{
    enum Buffers { POSITIONS, NORMALS, TANGENTS, TEXCOORDS, INDICES, NUM_BUFFERS };

  public:
    Mesh(const MeshData& meshData);
    Mesh(Mesh&& other);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    ~Mesh();

    void draw(ShaderProgram* shaderProgram) const;

    std::vector<float> positions() const;
    Aabb aabb() const { return m_aabb; }

    void setMaterial(const std::shared_ptr<Material>& material);

  protected:
    const Material* getMaterial() const;

  private:
    GLuint m_buffers[NUM_BUFFERS];
    GLuint m_vao;

    size_t m_bufferSizes[NUM_BUFFERS] = {0};

    GLenum m_primitive = GL_TRIANGLES;
    GLenum m_typeOfElement = GL_UNSIGNED_SHORT;
    unsigned int m_numberOfElements = 0u;
    unsigned int m_numberOfVertices = 0u;

    Aabb m_aabb;

    std::shared_ptr<Material> m_material;
};

#endif // MESH_H
