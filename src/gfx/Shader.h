#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

namespace gfx {

class Shader final
{
  public:
    Shader(GLenum type, const std::string& source);
    Shader(const Shader& other) = delete;
    Shader(Shader&& other);
    ~Shader();

    GLenum type() { return m_type; }
    GLuint id() { return m_shaderId; }

  private:
    GLuint m_shaderId = 0;
    GLenum m_type;
};

} // namespace gfx

#endif // SHADER_H
