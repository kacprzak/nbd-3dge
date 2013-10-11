#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "Shader.h"
#include <vector>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void addShared(Shader *shader);
    void link();
    void use(bool activate = true);
    GLuint id() { return m_shaderProgramId; }

private:
    GLuint m_shaderProgramId;
    std::vector<Shader *> m_shaders;
    bool m_linked;
};

#endif // SHADERPROGRAM_H
