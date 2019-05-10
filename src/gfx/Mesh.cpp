#include "Mesh.h"

#include "../Logger.h"

#include <array>
#include <cstring>
#include <limits>

namespace gfx {

Mesh::Mesh(std::array<Accessor, Accessor::Attribute::Size> attributes, Accessor indices,
           GLenum primitive)
    : m_attributes{attributes}
    , m_indices{indices}
    , m_primitive{primitive}
{
    if (m_attributes[Accessor::Attribute::Position].count == 0) {
        auto msg = "Mesh must have positions buffer";
        LOG_ERROR(msg);
        throw std::runtime_error{msg};
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if (m_indices.count > 0) {
        m_indices.buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
    }

    for (int i = 0; i < Accessor::Attribute::Size; ++i) {
        const Accessor& acc = m_attributes[i];
        if (acc.count > 0) {
            acc.buffer->bind(GL_ARRAY_BUFFER);
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, acc.size, acc.type, acc.normalized, acc.buffer->m_byteStride,
                                  (const void*)acc.byteOffset);
        }
    }

    LOG_CREATED;

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other)
{
    std::swap(m_vao, other.m_vao);
    std::swap(m_attributes, other.m_attributes);
    std::swap(m_indices, other.m_indices);
    std::swap(m_primitive, other.m_primitive);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);

    LOG_RELEASED;
}

void Mesh::draw(ShaderProgram* shaderProgram) const
{
    if (shaderProgram) {
        shaderProgram->use();
        m_material.applyTo(shaderProgram);
    }

    glBindVertexArray(m_vao);

    if (m_indices.count == 0) {
        glDrawArrays(m_primitive, 0, m_attributes[Accessor::Attribute::Position].count);
    } else {
        glDrawElements(m_primitive, m_indices.count, m_indices.type, 0);
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

Aabb Mesh::aabb() const
{
    auto minPos = m_attributes[Accessor::Attribute::Position].min;
    auto maxPos = m_attributes[Accessor::Attribute::Position].max;

    return {{minPos[0], minPos[1], minPos[2]}, {maxPos[0], maxPos[1], maxPos[2]}};
}

void Mesh::setMaterial(const Material& material) { m_material = material; }

} // namespace gfx
