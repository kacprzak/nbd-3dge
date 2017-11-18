#include "Mesh.h"

#include "Logger.h"
#include "ObjLoader.h"
#include "Util.h"

#include <array>
#include <cstring>
#include <limits>

Mesh::Mesh(const MeshData& md)
    : m_primitive{md.primitive}
{
    std::fill(std::begin(m_bufferSizes), std::end(m_bufferSizes), 0);

    m_numberOfVertices = md.positions.size();
    m_numberOfElements = md.indices.size();

    const auto& tangents = MeshData::calculateTangents(md);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(NUM_BUFFERS, m_buffers);

    auto aabb = MeshData::calculateAABB(md.positions);
    LOG_INFO << "Loaded Mesh: " << m_vao << " | " << md.name;
    LOG_TRACE << "  Positions: " << md.positions.size() << "\t id: " << m_buffers[POSITIONS] << '\n'
              << "  Normals: " << md.normals.size() << "\t id: " << m_buffers[NORMALS] << '\n'
              << "  Tangents: " << tangents.size() << "\t id: " << m_buffers[TANGENTS] << '\n'
              << "  TexCoords: " << md.texcoords.size() << "\t id: " << m_buffers[TEXCOORDS] << '\n'
              << "  Indices: " << md.indices.size() << "\t id: " << m_buffers[INDICES] << '\n'
              << "  Primitive: " << md.primitive << '\n'
              << "  Dimensions: (" << aabb[1] - aabb[0] << " x " << aabb[3] - aabb[2] << " x "
              << aabb[5] - aabb[4] << ")";

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    m_bufferSizes[POSITIONS] = sizeof(md.positions[0]) * md.positions.size();
    glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[POSITIONS], &md.positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (md.normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
        m_bufferSizes[NORMALS] = sizeof(md.normals[0]) * md.normals.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[NORMALS], &md.normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (tangents.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TANGENTS]);
        m_bufferSizes[TANGENTS] = sizeof(tangents[0]) * tangents.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[TANGENTS], &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (md.texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
        m_bufferSizes[TEXCOORDS] = sizeof(md.texcoords[0]) * md.texcoords.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[TEXCOORDS], &md.texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (md.indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);
        m_bufferSizes[INDICES] = sizeof(GLushort) * md.indices.size();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_bufferSizes[INDICES], &md.indices[0],
                     GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    LOG_INFO << "Released Mesh: " << m_vao;
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

    if (m_numberOfElements == 0) {
        glDrawArrays(m_primitive, start, count);
    } else {
        glDrawElements(m_primitive, count, GL_UNSIGNED_SHORT, 0);
    }

    glBindVertexArray(0);
}

std::vector<float> Mesh::positions() const
{
    std::vector<float> retVal;
    retVal.resize(m_bufferSizes[POSITIONS] / sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    void* buff = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

    std::memcpy(retVal.data(), buff, m_bufferSizes[POSITIONS]);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return retVal;
}
