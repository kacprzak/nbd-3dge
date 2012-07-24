#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "shader.h"
#include <vector>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void addShared(Shader *shader);
    void link();

private:
    GLuint m_shaderProgramId;
    std::vector<Shader *> m_shaders;
    bool m_linked;
};

#endif // SHADERPROGRAM_H
