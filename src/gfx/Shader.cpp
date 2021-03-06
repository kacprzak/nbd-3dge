#include "Shader.h"

#include "../Logger.h"

namespace gfx {

Shader::Shader(GLenum type, const std::string& source)
    : m_type{type}
{
    m_shaderId = glCreateShader(m_type);

    LOG_TRACE("Compiling Shader: {}", m_shaderId);

    const GLchar* shaderSource[1];
    shaderSource[0] = source.c_str();

    glShaderSource(m_shaderId, 1, shaderSource, 0);
    glCompileShader(m_shaderId);

    GLint status;
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(m_shaderId, infoLogLength, NULL, strInfoLog);

        const char* strShaderType = NULL;

        switch (type) {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        LOG_ERROR("Compile failure in {} shader:\n{}", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }
}

Shader::Shader(Shader&& other)
{
    std::swap(m_shaderId, other.m_shaderId);
    std::swap(m_type, other.m_type);
}

Shader::~Shader() { glDeleteShader(m_shaderId); }

} // namespace gfx