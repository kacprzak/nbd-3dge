#include "Mesh.h"

#include "ObjLoader.h"
#include "Util.h"
#include <iostream>
#include <cstring>

Mesh::Mesh(const std::string& name)
    : m_name(name)
{
    memset(m_buffers, 0, sizeof(m_buffers));
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    std::cout << "Released: " << m_name << std::endl;
}

void Mesh::draw(GLenum mode) const
{
    if (m_numberOfElements == 0) {
        draw(0, m_numberOfVertices, mode);
    } else {
        draw(0, m_numberOfElements, mode);
    }
}

void Mesh::draw(int start, int count, GLenum mode) const
{
    glBindVertexArray(m_vao);

    // Draw
    if (m_numberOfElements == 0) {
        glDrawArrays(mode, start, count);
    } else {
        glDrawElements(mode, count, GL_UNSIGNED_SHORT, 0);
    }
}

Mesh *Mesh::create(const std::string& objfileName)
{
    ObjLoader objLoader;
    objLoader.load(objfileName);

    std::string name = extractFilename(objfileName);

    std::cout << "Loading... " << name << '\n';

    return create(name,
                  objLoader.vertices(),
                  objLoader.normals(),
                  objLoader.texCoords(),
                  objLoader.indices());
}

Mesh *Mesh::create(const std::string& name,
                   const std::vector<GLfloat> &vertices,
                   const std::vector<GLfloat> &normals,
                   const std::vector<GLfloat> &texcoords,
                   const std::vector<GLushort> &indices)
{
    Mesh *mesh = new Mesh(name);

    mesh->m_numberOfVertices = vertices.size();
    mesh->m_numberOfElements = indices.size();

    glGenVertexArrays(1, &mesh->m_vao);
    glGenBuffers(NUM_BUFFERS, mesh->m_buffers);

    std::cout << "Loaded: " << mesh->m_name << "\t vaoId: " << mesh->m_vao << "\n";
    std::cout << "  Vertices: " << vertices.size()/3 << "\t id: " << mesh->m_buffers[VERTICES] << "\n";
    std::cout << "  TexCoords: " << texcoords.size()/2 << "\t id: " << mesh->m_buffers[TEXCOORDS] << "\n";
    std::cout << "  Normals: " << normals.size()/3 << "\t id: " << mesh->m_buffers[NORMALS] << "\n";
    std::cout << "  Faces: " << indices.size()/3 << "\t id: " << mesh->m_buffers[INDICES] << std::endl;

    glBindVertexArray(mesh->m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffers[VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffers[TEXCOORDS]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffers[NORMALS]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_buffers[INDICES]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
       
    return mesh;
}
