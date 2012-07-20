#include "mesh.h"

#include "objloader.h"
#include "util.h"
#include <iostream>

Mesh::Mesh(const std::string &name)
    : m_name(name)
    , m_hasNormals(false)
    , m_hasTexCoords(false)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    std::cout << "Released: " << m_name << std::endl;
}

void Mesh::draw() const
{
    // Bind Buffers

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTICES]);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    if (m_hasNormals) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
        glNormalPointer(GL_FLOAT, 0, 0);
    }

    if (m_hasTexCoords) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);

    // Draw
    if (m_shadeModel == GL_FLAT) {
        glShadeModel(GL_FLAT);
        glDrawArrays(GL_TRIANGLES, 0, m_numberOfVertices);
        glShadeModel(GL_SMOOTH);
    } else {
        glDrawElements(GL_TRIANGLES, m_numberOfElements, GL_UNSIGNED_SHORT, 0);
    }

    if (m_hasNormals) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    if (m_hasTexCoords) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

Mesh *Mesh::create(const std::string& objfileName, GLenum shadeModel)
{
    ObjLoader objLoader;

    if (shadeModel == GL_FLAT)
        objLoader.setFlatShadingModel();
    objLoader.load(objfileName);

    std::string name = extractFilename(objfileName);

    return create(name,
                  objLoader.vertices(),
                  objLoader.normals(),
                  objLoader.texCoords(),
                  objLoader.indices(),
                  shadeModel);
}

Mesh *Mesh::create(const std::string& name,
                   const std::vector<GLfloat> &vertices,
                   const std::vector<GLfloat> &normals,
                   const std::vector<GLfloat> &texcoords,
                   const std::vector<GLushort> &indices,
                   GLenum shadeModel)
{
    Mesh *mesh = new Mesh(name);
    mesh->m_shadeModel = shadeModel;

    mesh->m_numberOfVertices = vertices.size();
    mesh->m_numberOfElements = indices.size();

    mesh->m_hasNormals = !normals.empty();
    mesh->m_hasTexCoords = !texcoords.empty();

    glGenBuffers(NUM_BUFFERS, mesh->m_buffers);

    std::cout << "Loaded: " << mesh->m_name << "\n";
    std::cout << "  Vertices: " << vertices.size()/3 << "\t id: " << mesh->m_buffers[VERTICES] << "\n";
    std::cout << "  Normals: " << normals.size()/3 << "\t id: " << mesh->m_buffers[NORMALS] << "\n";
    std::cout << "  TexCoords: " << texcoords.size()/2 << "\t id: " << mesh->m_buffers[TEXCOORDS] << "\n";
    std::cout << "  Faces: " << indices.size()/3 << "\t id: " << mesh->m_buffers[INDICES] << std::endl;


    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffers[VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffers[NORMALS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(),
                 &normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffers[TEXCOORDS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcoords.size(),
                 &texcoords[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_buffers[INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(),
                 &indices[0], GL_STATIC_DRAW);

    return mesh;
}
