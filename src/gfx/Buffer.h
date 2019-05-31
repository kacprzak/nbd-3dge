#ifndef GFX_BUFFER_H
#define GFX_BUFFER_H

#include "Macros.h"

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <vector>

namespace gfx {

class Buffer final
{
    OSTREAM_FRIEND(Buffer);

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

OSTREAM_IMPL_1(gfx::Buffer, m_bufferId)

//------------------------------------------------------------------------------

struct Accessor final
{
    enum Attribute { Position, Normal, Tangent, TexCoord_0, Color_0, Joints_0, Weights_0, Size };

    // clang-format off
    template <typename T> GLenum glTypeToEnum() const { return GL_INVALID_ENUM; }
    // clang-format on

    template <typename T>
    std::vector<T> getElements(std::size_t from, std::size_t to) const
    {
        if (from == to) return {};

        if (from > to) {
            throw std::invalid_argument{"<to> must be larger or equal than <from>"};
        }

        if (to > count) {
            throw std::out_of_range{"<to> is larger than <count>"};
        }

        if (glTypeToEnum<T>() != type) {
            throw std::runtime_error{"Wrong type while loading data from OpenGL Buffer"};
        }
        std::vector<T> ans(to - from, T{});
        buffer->getData(ans.data(), ans.size() * sizeof(T), byteOffset + from * sizeof(T));
        return ans;
    }

    template <typename T>
    std::vector<T> getData() const
    {
        return getElements<T>(0, count);
    }

    std::size_t typeSize() const
    {
        switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: return 1;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT: return 2;
        default: return 4;
        }
    }

    std::shared_ptr<Buffer> buffer;
    std::ptrdiff_t byteOffset = 0;
    unsigned count            = 0; //< Number of elements (not bytes!)
    unsigned size             = 4; //< Number of components per element
    GLenum type;                   //< Component type
    bool normalized = false;
    std::array<float, 16> min{};
    std::array<float, 16> max{};
};

OSTREAM_IMPL(gfx::Accessor)

// clang-format off
template <> inline GLenum Accessor::glTypeToEnum<GLbyte>() const { return GL_BYTE; }
template <> inline GLenum Accessor::glTypeToEnum<GLubyte>() const { return GL_UNSIGNED_BYTE; }
template <> inline GLenum Accessor::glTypeToEnum<GLshort>() const { return GL_SHORT; }
template <> inline GLenum Accessor::glTypeToEnum<GLushort>() const { return GL_UNSIGNED_SHORT; }
template <> inline GLenum Accessor::glTypeToEnum<GLint>() const { return GL_INT; }
template <> inline GLenum Accessor::glTypeToEnum<GLuint>() const { return GL_UNSIGNED_INT; }
template <> inline GLenum Accessor::glTypeToEnum<GLfloat>() const { return GL_FLOAT; }
template <> inline GLenum Accessor::glTypeToEnum<glm::vec2>() const { return GL_FLOAT; }
template <> inline GLenum Accessor::glTypeToEnum<glm::vec3>() const { return GL_FLOAT; }
template <> inline GLenum Accessor::glTypeToEnum<glm::vec4>() const { return GL_FLOAT; }
template <> inline GLenum Accessor::glTypeToEnum<glm::quat>() const { return GL_FLOAT; }
// clang-format on

//------------------------------------------------------------------------------

Accessor calculateTangents(const std::array<Accessor, Accessor::Attribute::Size>& attributes,
                           const Accessor& indices);

} // namespace gfx

#endif // GFX_BUFFER_H
