#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>

class Mesh
{
    enum Buffers {
        POSITIONS,
        NORMALS,
        TEXCOORDS,
        INDICES,
        NUM_BUFFERS
    };

 public:
    Mesh(GLenum primitive,
         const std::vector<GLfloat> &vertices,
         const std::vector<GLfloat> &normals,
         const std::vector<GLfloat> &texcoords,
         const std::vector<GLushort> &indices);

    ~Mesh();

    void draw() const;
    void draw(int start, int count) const;

    static Mesh* fromWavefrontObj(const std::string& objfileName);
    static Mesh* fromHeightmap(const std::vector<unsigned char>& heights, int w, int h,
                               float amplitude, float textureStrech);

 private:
    GLuint m_buffers[NUM_BUFFERS];
    GLuint m_vao;

    GLenum m_primitive;

    unsigned int m_numberOfElements;
    unsigned int m_numberOfVertices;
};

#endif // MESH_H
