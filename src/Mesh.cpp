#include "Mesh.h"

#include "ObjLoader.h"
#include "Util.h"
#include "Logger.h"
#include <cstring>

Mesh::Mesh(GLenum primitive,
           const std::vector<GLfloat> &vertices,
           const std::vector<GLfloat> &normals,
           const std::vector<GLfloat> &texcoords,
           const std::vector<GLushort> &indices)
    : m_primitive{primitive}
{
    m_numberOfVertices = vertices.size();
    m_numberOfElements = indices.size();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(NUM_BUFFERS, m_buffers);

    LOG_TRACE << "Loaded Mesh: " << "\t vaoId: " << m_vao << '\n'
              << "  Vertices: " << vertices.size()/3 << "\t id: " << m_buffers[POSITIONS] << '\n'
              << "  Normals: " << normals.size()/3 << "\t id: " << m_buffers[NORMALS] << '\n'
              << "  TexCoords: " << texcoords.size()/2 << "\t id: " << m_buffers[TEXCOORDS] << '\n'
              << "  Indices: " << indices.size() << "\t id: " << m_buffers[INDICES] << '\n'
              << "  Primitive: " << primitive;

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    if (texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    LOG_INFO << "Released: " << m_vao;
}

void Mesh::draw() const
{
    if (m_numberOfElements == 0) {
        draw(0, m_numberOfVertices);
    } else {
        draw(0, m_numberOfElements);
    }
}

void Mesh::draw(int start, int count) const
{
    glBindVertexArray(m_vao);

    // Draw
    if (m_numberOfElements == 0) {
        glDrawArrays(m_primitive, start, count);
    } else {
        glDrawElements(m_primitive, count, GL_UNSIGNED_SHORT, 0);
    }
}

Mesh *Mesh::create(const std::string& objfileName)
{
    ObjLoader objLoader;
    objLoader.load(objfileName);

    std::string name = extractFilename(objfileName);

    LOG_INFO << "Loading... " << name;

    return new Mesh{GLenum(objLoader.primitive()),
            objLoader.vertices(),
            objLoader.normals(),
            objLoader.texCoords(),
            objLoader.indices()};
}

