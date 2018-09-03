#include "Mesh.h"

#include "Logger.h"
#include "ObjLoader.h"
#include "Util.h"

#include <array>
#include <cstring>
#include <limits>

Mesh::Mesh(std::array<Accessor, Accessor::Attribute::Size> attributes, Accessor indices,
           GLenum primitive)
    : m_primitive{primitive}
{
    m_positionsAcc = attributes[Accessor::Attribute::Position];

    if (m_positionsAcc.count == 0) {
        auto msg = "Mesh must have positions buffer";
        LOG_ERROR(msg);
        throw std::runtime_error{msg};
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if (indices.count > 0) {
        m_typeOfElement    = indices.type;
        m_numberOfElements = indices.count;
        indices.buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
    }

    for (int i = 0; i < Accessor::Attribute::Size; ++i) {
        const Accessor& acc = attributes[i];
        if (acc.count > 0) {
            m_numberOfVertices = acc.count;
            acc.buffer->bind(GL_ARRAY_BUFFER);
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, acc.size, acc.type, acc.normalized, acc.buffer->m_byteStride,
                                  (const void*)acc.byteOffset);
        }
    }

    auto minPos = m_positionsAcc.min;
    auto maxPos = m_positionsAcc.max;
    m_aabb      = Aabb{{minPos[0], minPos[1], minPos[2]}, {maxPos[0], maxPos[1], maxPos[2]}};

    LOG_INFO("Loaded Mesh: {}", m_vao);

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other)
{
    std::swap(m_vao, other.m_vao);
    std::swap(m_primitive, other.m_primitive);
    std::swap(m_typeOfElement, other.m_typeOfElement);
    std::swap(m_numberOfElements, other.m_numberOfElements);
    std::swap(m_numberOfVertices, other.m_numberOfVertices);
    std::swap(m_aabb, other.m_aabb);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);

    LOG_INFO("Released Mesh: {}", m_vao);
}

void Mesh::draw(ShaderProgram* shaderProgram) const
{
    if (shaderProgram) {
        shaderProgram->use();
        m_material.applyTo(shaderProgram);
    }

    glBindVertexArray(m_vao);

    if (m_numberOfElements == 0) {
        glDrawArrays(m_primitive, 0, m_numberOfVertices);
    } else {
        glDrawElements(m_primitive, m_numberOfElements, m_typeOfElement, 0);
    }

    glBindVertexArray(0);
}

std::vector<float> Mesh::positions() const
{
    std::vector<float> retVal;
    /*
    retVal.resize(m_bufferSizes[POSITIONS] / sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    void* buff = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

    std::memcpy(retVal.data(), buff, m_bufferSizes[POSITIONS]);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
*/
    // return m_positionsAcc.buffer->getData();

    return retVal;
}

void Mesh::setMaterial(const Material& material) { m_material = material; }
