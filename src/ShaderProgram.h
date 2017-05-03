#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "Shader.h"

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class ShaderProgram final
{
 public:
    ShaderProgram();
    ~ShaderProgram();

    void addShared(Shader *shader);
    void link();
    void use();

    void setUniform(const char* name, const glm::mat4& matrix);
    void setUniform(const char* name, float value);
    void setUniform(const char* name, int value);
    
 private:
    GLuint m_shaderProgramId;
    std::vector<Shader *> m_shaders;
    bool m_linked;
};

#endif // SHADERPROGRAM_H
