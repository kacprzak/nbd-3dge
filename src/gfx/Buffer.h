#ifndef BUFFER_H
#define BUFFER_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <vector>

namespace gfx {

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
    void getData(void* data, std::size_t size, std::ptrdiff_t byteOffset = 0) const;

    unsigned m_byteStride = 0;
    std::size_t m_size    = 0;

  private:
    GLuint m_bufferId = 0;
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

    // clang-format off
    template <typename T> GLenum glTypeToEnum() const { return GL_INVALID_ENUM; }
    template <> GLenum glTypeToEnum<GLushort>() const { return GL_UNSIGNED_SHORT; }
	template <> GLenum glTypeToEnum<GLuint>() const { return GL_UNSIGNED_INT; }
	template <> GLenum glTypeToEnum<GLfloat>() const { return GL_FLOAT; }
	template <> GLenum glTypeToEnum<glm::vec2>() const { return GL_FLOAT; }
	template <> GLenum glTypeToEnum<glm::vec3>() const { return GL_FLOAT; }
	template <> GLenum glTypeToEnum<glm::vec4>() const { return GL_FLOAT; }
    // clang-format on

    template <typename T>
    std::vector<T> getData() const
    {
        if (glTypeToEnum<T>() != type) {
            throw std::runtime_error("Wrong type while loading data from OpenGL Buffer");
        }
        std::vector<T> ans(count * size, T{});
        buffer->getData(ans.data(), ans.size() * sizeof(T), byteOffset);
        return ans;
    }

    std::shared_ptr<Buffer> buffer;
    unsigned byteOffset = 0;
    unsigned count      = 0; //< Number of elements (not bytes!)
    unsigned size       = 4; //< Number of components per element
    GLenum type;             //< Component type
    bool normalized = false;
    std::array<float, 4> min{};
    std::array<float, 4> max{};
};

} // namespace gfx

#endif // BUFFER_H
