#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <memory>
#include <string>
#include <vector>

class Mesh
{
    enum Buffers { POSITIONS, NORMALS, TEXCOORDS, INDICES, NUM_BUFFERS };

  public:
    Mesh(GLenum primitive, const std::vector<GLfloat>& vertices,
         const std::vector<GLfloat>& normals, const std::vector<GLfloat>& texcoords,
         const std::vector<GLushort>& indices);

    ~Mesh();

    void draw() const;
    void draw(int start, int count) const;

    std::vector<float> positions() const;
    
    static Mesh* fromWavefrontObj(const std::string& objfileName);
    static Mesh* fromHeightmap(const std::vector<float>& heights, int w, int h,
                               float textureStrech);

  private:
    GLuint m_buffers[NUM_BUFFERS];
    GLuint m_vao;

    size_t m_bufferSizes[NUM_BUFFERS];

    GLenum m_primitive;

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;
};

#endif // MESH_H
