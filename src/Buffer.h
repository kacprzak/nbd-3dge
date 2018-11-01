#ifndef BUFFER_H
#define BUFFER_H

#include <GL/glew.h>
#include <array>
#include <memory>

class Buffer final
{
  public:
    Buffer();
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other);
    ~Buffer();

    void bind(GLenum target);

    void loadData(const void* data, std::size_t size, GLenum usage = GL_STATIC_DRAW);
    void getData(void* data, std::size_t size) const;

    unsigned m_byteStride = 0;

  private:
    GLuint m_bufferId  = 0;
    std::size_t m_size = 0;
};

struct Accessor final
{
    enum Attribute {
        Position,
        Normal,
        Tangent,
        TexCoord_0,
        TexCoord_1,
        TexCoord_2,
        TexCoord_3,
        Size
    };

    std::shared_ptr<Buffer> buffer;
    unsigned byteOffset = 0;
    unsigned count      = 0;
    unsigned size       = 4;
    GLenum type;
    bool normalized = false;
    std::array<float, 4> min{};
    std::array<float, 4> max{};
};

#endif // BUFFER_H
