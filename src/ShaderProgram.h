#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "Shader.h"

#include <GL/glew.h>
#include <boost/container/flat_map.hpp>
#include <glm/glm.hpp>
#include <vector>

class ShaderProgram final
{
  public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& other) = delete;
    ~ShaderProgram();

    void link(const std::vector<Shader*>& shaders);
    void use();

    void setUniform(const char* name, const glm::mat4& matrix);
    void setUniform(const char* name, float value);
    void setUniform(const char* name, int value);

  private:
    GLint getUniformLocation(const char* name);

    GLuint m_shaderProgramId;
    boost::container::flat_map<std::string, GLint> m_uniformLocs;
    bool m_linked;
};

#endif // SHADERPROGRAM_H
