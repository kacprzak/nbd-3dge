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
    glGetBufferSubData(GL_COPY_READ_BUFFER, byteOffset, size, data);
}

//------------------------------------------------------------------------------

namespace {

template <typename T>
Accessor _calculateTangents(const std::array<Accessor, Accessor::Attribute::Size>& attributes,
                            const Accessor& indicesAcc)
{
    using namespace glm;

    auto texcoords = attributes[Accessor::Attribute::TexCoord_0].getData<vec2>();

    if (texcoords.empty()) return {};

    auto indices   = indicesAcc.getData<T>();
    auto positions = attributes[Accessor::Attribute::Position].getData<vec3>();
    auto normals   = attributes[Accessor::Attribute::Normal].getData<vec3>();

    std::vector<vec4> tangents;

    tangents.resize(normals.size());

    auto inc = 3; // Only GL_TRIANGLES supported

    for (std::ptrdiff_t i = 2; i < indices.size(); i += inc) {
        size_t index[3] = {indices[i - 2], indices[i - 1], indices[i - 0]};
        // Shortcuts for positions
        vec3 v0 = positions[index[0]];
        vec3 v1 = positions[index[1]];
        vec3 v2 = positions[index[2]];

        // Shortcuts for UVs
        vec2 st0 = texcoords[index[0]];
        vec2 st1 = texcoords[index[1]];
        vec2 st2 = texcoords[index[2]];

        // Edges of the triangle : postion delta
        vec3 dPos1 = v1 - v0;
        vec3 dPos2 = v2 - v0;

        // ST delta
        vec2 dST1 = st1 - st0;
        vec2 dST2 = st2 - st0;

        float r        = 1.0f / (dST1.x * dST2.y - dST1.y * dST2.x);
        vec3 tangent   = (dPos1 * dST2.y - dPos2 * dST1.y) * r;
        vec3 bitangent = (dPos2 * dST1.x - dPos1 * dST2.x) * r;

        for (int j = 0; j < inc; ++j) {
            auto t = tangent;
            auto n = normals[index[j]];

            // Gram-Schmidt orthogonalize
            tangents[index[j]] = vec4(normalize(t - n * dot(n, t)), 0.0f);
            // Calculate headedness
            tangents[index[j]].w = dot(cross(n, t), bitangent) < 0.0f ? -1.0f : 1.0f;
        }
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

} // namespace

Accessor calculateTangents(const std::array<Accessor, Accessor::Attribute::Size>& attributes,
                           const Accessor& indicesAcc)
{
    switch (indicesAcc.type) {
    case GL_UNSIGNED_BYTE: return _calculateTangents<uint8_t>(attributes, indicesAcc);
    case GL_UNSIGNED_SHORT: return _calculateTangents<uint16_t>(attributes, indicesAcc);
    case GL_UNSIGNED_INT: return _calculateTangents<uint32_t>(attributes, indicesAcc);
    default: throw std::invalid_argument{"Unknown indices type"};
    }
}

} // namespace gfx
