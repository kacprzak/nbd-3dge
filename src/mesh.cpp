#include "mesh.h"

#include "objloader.h"
#include "util.h"

Mesh::Mesh(const std::string &objfileName)
    : m_objfileName(objfileName)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    std::cout << "Released: " << m_objfileName << std::endl;
}

void Mesh::draw() const
{
    // Bind Buffers

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTICES]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);

    // Draw
    if (m_shadeModel == GL_FLAT) {
        glShadeModel(GL_FLAT);
        glDrawArrays(GL_TRIANGLES, 0, m_numberOfVertices);
        glShadeModel(GL_SMOOTH);
    } else {
        glDrawElements(GL_TRIANGLES, m_numberOfElements, GL_UNSIGNED_SHORT, 0);
    }
}

Mesh *Mesh::create(const std::string& objfileName, GLenum shadeModel)
{
    Mesh *mesh = new Mesh(extractFilename(objfileName));
    mesh->m_shadeModel = shadeModel;

    ObjLoader<GLfloat, GLushort> objLoader;
    if (shadeModel == GL_FLAT)
        objLoader.setFlatShadingModel();

    objLoader.load(objfileName);
    std::vector<GLfloat> vertices = objLoader.vertices();
    std::vector<GLfloat> normals = objLoader.normals();
    std::vector<GLfloat> texcoords = objLoader.texCoords();
    std::vector<GLushort> indices = objLoader.indices();

    mesh->m_numberOfVertices = vertices.size();
    mesh->m_numberOfElements = indices.size();

    glGenBuffers(NUM_BUFFERS, mesh->m_buffers);

    std::cout << "Loaded: " << mesh->m_objfileName << "\n";
    std::cout << "  Vertices: " << vertices.size()/3 << "\t" << mesh->m_buffers[VERTICES] << "\n";
    std::cout << "  Normals: " << normals.size()/3 << "\t" << mesh->m_buffers[NORMALS] << "\n";
    std::cout << "  TexCoords: " << texcoords.size()/2 << "\t" << mesh->m_buffers[TEXCOORDS] << "\n";
    std::cout << "  Faces: " << indices.size()/3 << "\t" << mesh->m_buffers[INDICES] << std::endl;


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
