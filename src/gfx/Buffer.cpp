#include "Buffer.h"

namespace gfx {

Buffer::Buffer()
{
    glGenBuffers(1, &m_bufferId);
    LOG_CREATED;
}

Buffer::Buffer(Buffer&& other)
{
    std::swap(m_byteStride, other.m_byteStride);
    std::swap(m_bufferId, other.m_bufferId);
    std::swap(m_size, other.m_size);
}

Buffer::~Buffer()
{
    if (m_bufferId) {
        glDeleteBuffers(1, &m_bufferId);
        LOG_RELEASED;
    }
}

void Buffer::bind(GLenum target) { glBindBuffer(target, m_bufferId); }

void Buffer::loadData(const void* data, size_t size, GLenum usage)
{
    bind(GL_COPY_WRITE_BUFFER);
    glBufferData(GL_COPY_WRITE_BUFFER, size, data, usage);
    m_size = size;
}

void Buffer::getData(void* data, size_t size, std::ptrdiff_t byteOffset) const
{
    glBindBuffer(GL_COPY_READ_BUFFER, m_bufferId);
    void* buff = glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);

    std::memcpy(data, (uint8_t*)buff + byteOffset, std::min(size, m_size - byteOffset));

    glUnmapBuffer(GL_COPY_READ_BUFFER);
}

//------------------------------------------------------------------------------

Accessor calculateTangents(const std::array<Accessor, Accessor::Attribute::Size>& attributes,
                           const Accessor& indicesAcc)
{
    auto indices   = indicesAcc.getData<uint16_t>();
    auto positions = attributes[Accessor::Attribute::Normal].getData<glm::vec3>();
    auto normals   = attributes[Accessor::Attribute::Normal].getData<glm::vec3>();
    auto texcoords = attributes[Accessor::Attribute::TexCoord_0].getData<glm::vec2>();

    std::vector<glm::vec3> tangents;

    if (texcoords.empty()) return {};

    tangents.resize(normals.size());

    auto inc = 3; // Only GL_TRIANGLES supported

    for (size_t i = 2; i < indices.size(); i += inc) {
        auto index0 = indices[i - 2];
        auto index1 = indices[i - 1];
        auto index2 = indices[i - 0];
        // Shortcuts for positions
        glm::vec3 v0 = positions[index0];
        glm::vec3 v1 = positions[index1];
        glm::vec3 v2 = positions[index2];

        // Shortcuts for UVs
        glm::vec2 st0 = texcoords[index0];
        glm::vec2 st1 = texcoords[index1];
        glm::vec2 st2 = texcoords[index2];

        // Edges of the triangle : postion delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // ST delta
        glm::vec2 deltaST1 = st1 - st0;
        glm::vec2 deltaST2 = st2 - st0;

        float r           = 1.0f / (deltaST1.x * deltaST2.y - deltaST1.y * deltaST2.x);
        glm::vec3 tangent = (deltaPos1 * deltaST2.y - deltaPos2 * deltaST1.y) * r;
        // glm::vec3 bitangent = (deltaPos2 * deltaST1.x - deltaPos1 * deltaST2.x) * r;

        tangent = glm::normalize(tangent);

        tangents[index0] = tangent;
        tangents[index1] = tangent;
        tangents[index2] = tangent;
    }

    auto tangentsBuffer = std::make_shared<Buffer>();
    tangentsBuffer->loadData(tangents.data(), tangents.size() * sizeof(tangents[0]));

    Accessor tangentsAccesor;
    tangentsAccesor.buffer = tangentsBuffer;
    tangentsAccesor.count  = tangents.size();
    tangentsAccesor.type   = GL_FLOAT;
    tangentsAccesor.size   = sizeof(tangents[0]) / sizeof(tangents[0][0]);

    return tangentsAccesor;
}

} // namespace gfx
