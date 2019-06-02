#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "Macros.h"
#include "Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <boost/container/flat_map.hpp>

#include <string>
#include <vector>

namespace gfx {

class ShaderProgram final
{
    OSTREAM_FRIEND(ShaderProgram);

  public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other);
    ~ShaderProgram();

    void link(const std::vector<Shader*>& shaders);
    void use();

    void setUniform(const std::string& name, const glm::mat4& matrix);
    void setUniform(const std::string& name, const glm::mat3& matrix);
    void setUniform(const std::string& name, const glm::vec4& vector);
    void setUniform(const std::string& name, const glm::vec3& vector);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, int value);

    std::string name;

  private:
    GLint getUniformLocation(const std::string& name);

    GLuint m_shaderProgramId = 0;
    boost::container::flat_map<std::string, GLint> m_uniformLocs;
    bool m_linked = false;
};

OSTREAM_IMPL_1(gfx::ShaderProgram, m_shaderProgramId)

} // namespace gfx

#endif // SHADERPROGRAM_H
