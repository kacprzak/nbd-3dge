#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <boost/container/flat_map.hpp>

#include <string>
#include <vector>

namespace gfx {

class ShaderProgram final
{
  public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other);
    ~ShaderProgram();

    void link(const std::vector<Shader*>& shaders);
    void use();

    void setUniform(const char* name, const glm::mat4& matrix);
    void setUniform(const char* name, const glm::vec4& vector);
    void setUniform(const char* name, const glm::vec3& vector);
    void setUniform(const char* name, float value);
    void setUniform(const char* name, int value);

    std::string name;

  private:
    GLint getUniformLocation(const char* name);

    GLuint m_shaderProgramId = 0;
    boost::container::flat_map<std::string, GLint> m_uniformLocs;
    bool m_linked = false;
};

} // namespace gfx

#endif // SHADERPROGRAM_H