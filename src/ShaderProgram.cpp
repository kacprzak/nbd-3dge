#include "ShaderProgram.h"

#include "Logger.h"

#include <glm/gtc/type_ptr.hpp>
#include <iterator>
#include <sstream>

ShaderProgram::ShaderProgram()
{
    m_shaderProgramId = glCreateProgram();
    LOG_TRACE << "Created Program: " << m_shaderProgramId;
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
{
    std::swap(m_shaderProgramId, other.m_shaderProgramId);
    std::swap(m_linked, other.m_linked);
    std::swap(m_uniformLocs, other.m_uniformLocs);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgramId);
    LOG_TRACE << "Deleted Program: " << m_shaderProgramId;
}

void ShaderProgram::link(const std::vector<Shader*>& shaders)
{
    m_linked = false;
    m_uniformLocs.clear();

    for (Shader* s : shaders) {
        glAttachShader(m_shaderProgramId, s->id());
    }

    glLinkProgram(m_shaderProgramId);

    std::ostringstream ss;
    ss << "Linking Program: " << m_shaderProgramId << " ( ";
    for (const auto& s : shaders)
        ss << s->id() << " ";
    LOG_TRACE << ss.str() << ")";

    GLint status;
    glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(m_shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(m_shaderProgramId, infoLogLength, 0, strInfoLog);
        LOG_WARNING << "Linker failure: " << strInfoLog;
        delete[] strInfoLog;
    } else {
        m_linked = true;
    }

    for (Shader* s : shaders) {
        glDetachShader(m_shaderProgramId, s->id());
    }
}

void ShaderProgram::use() { glUseProgram(m_shaderProgramId); }

void ShaderProgram::setUniform(const char* name, const glm::mat4& matrix)
{
    GLint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(const char* name, const glm::vec4& vector)
{
    GLint loc = getUniformLocation(name);
    glUniform4fv(loc, 1, glm::value_ptr(vector));
}

void ShaderProgram::setUniform(const char* name, const glm::vec3& vector)
{
    GLint loc = getUniformLocation(name);
    glUniform3fv(loc, 1, glm::value_ptr(vector));
}

void ShaderProgram::setUniform(const char* name, float value)
{
    GLint loc = getUniformLocation(name);
    glUniform1f(loc, value);
}

void ShaderProgram::setUniform(const char* name, int value)
{
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, value);
}

GLint ShaderProgram::getUniformLocation(const char* name)
{
    const auto it = m_uniformLocs.find(name);
    if (it != m_uniformLocs.cend()) {
        return it->second;
    } else {
        GLint loc           = glGetUniformLocation(m_shaderProgramId, name);
        m_uniformLocs[name] = loc;
        return loc;
    }
}
