#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class Mesh
{
    enum Buffers { POSITIONS, NORMALS, TANGENTS, TEXCOORDS, INDICES, NUM_BUFFERS };

  public:
    Mesh(GLenum primitive, const std::vector<glm::vec3>& vertices,
         const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texcoords,
         const std::vector<GLushort>& indices);

    ~Mesh();

    void draw() const;
    void draw(int start, int count) const;

    std::vector<float> positions() const;

    static std::unique_ptr<Mesh> fromWavefrontObj(const std::string& objfileName);
    static std::unique_ptr<Mesh> fromHeightmap(const std::vector<float>& heights, int w, int h,
                                               float textureStrech);

  private:
    static std::array<float, 6> calculateAABB(const std::vector<glm::vec3>& positions);

    static std::vector<glm::vec3> calculateTangents(const std::vector<glm::vec3>& vertices,
                                                    const std::vector<glm::vec3>& normals,
                                                    const std::vector<glm::vec2>& texcoords,
                                                    const std::vector<GLushort>& indices);

    GLuint m_buffers[NUM_BUFFERS];
    GLuint m_vao;

    size_t m_bufferSizes[NUM_BUFFERS];

    GLenum m_primitive;

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;
};

#endif // MESH_H
