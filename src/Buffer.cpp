#include "Buffer.h"

#include "Logger.h"

Buffer::Buffer(GLenum target)
    : m_target{target}
{
    glGenBuffers(1, &m_bufferId);
    LOG_INFO("Created Buffer: {}", m_bufferId);
}

Buffer::Buffer(Buffer&& other)
{
    std::swap(m_byteStride, other.m_byteStride);
    std::swap(m_target, other.m_target);
    std::swap(m_bufferId, other.m_bufferId);
    std::swap(m_size, other.m_size);
}

Buffer::~Buffer()
{
    if (m_bufferId) {
        glDeleteBuffers(1, &m_bufferId);
        LOG_INFO("Released Buffer: {}", m_bufferId);
    }
}

void Buffer::bind() { glBindBuffer(m_target, m_bufferId); }

void Buffer::loadData(const void* data, size_t size, GLenum usage)
{
    bind();
    glBufferData(m_target, size, data, usage);
    m_size = size;
}

void Buffer::getData(void* data, size_t size) const
{
    glBindBuffer(m_target, m_bufferId);
    void* buff = glMapBuffer(m_target, GL_READ_ONLY);

    std::memcpy(data, buff, std::min(size, m_size));

    glUnmapBuffer(m_target);
}
