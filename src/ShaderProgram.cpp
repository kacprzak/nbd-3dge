#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iterator>

ShaderProgram::ShaderProgram()
    : m_linked(false)
{
    m_shaderProgramId = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgramId);
}

void ShaderProgram::addShared(Shader *shader)
{
    m_shaders.push_back(shader);
}

void ShaderProgram::link()
{    
    for (Shader *s : m_shaders) {
        glAttachShader(m_shaderProgramId, s->id());
    }

    glLinkProgram(m_shaderProgramId);

    std::cout << "Linking: ";
    for (const auto& s : m_shaders)
        std::cout << s->id() << " ";
    std::cout << "\tid: " << m_shaderProgramId << std::endl;
    
    GLint status;
    glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(m_shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(m_shaderProgramId, infoLogLength, 0, strInfoLog);
        std::cout << "Linker failure: " << strInfoLog << std::endl;
        delete [] strInfoLog;
    }

    for (Shader *s : m_shaders) {
        glDetachShader(m_shaderProgramId, s->id());
    }

    m_linked = true;
    m_shaders.clear();
}

void ShaderProgram::use()
{
    glUseProgram(m_shaderProgramId);
}

void ShaderProgram::setUniform(const char* name, const glm::mat4& matrix)
{
    GLint loc = glGetUniformLocation(m_shaderProgramId, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}
