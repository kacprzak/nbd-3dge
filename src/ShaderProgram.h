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
    ~ShaderProgram();

    void addShared(Shader* shader);
    void link();
    void use();

    void setUniform(const char* name, const glm::mat4& matrix);
    void setUniform(const char* name, float value);
    void setUniform(const char* name, int value);

  private:
    GLint getUniformLocation(const char* name);

    GLuint m_shaderProgramId;
    std::vector<Shader*> m_shaders;
    boost::container::flat_map<std::string, GLint> m_uniformLocs;
    bool m_linked;
};

#endif // SHADERPROGRAM_H
