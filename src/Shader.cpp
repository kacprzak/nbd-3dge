#include "Shader.h"

#include "Logger.h"
#include "Util.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace boost::algorithm;

Shader::Shader(GLenum type, const std::string& source)
    : m_type{type}
{
    m_shaderId = glCreateShader(m_type);

    LOG_INFO << "Compiling Shader: " << m_shaderId;

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
        // clang-format off
        switch(type)
        {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }
        // clang-format on

        LOG_ERROR << "Compile failure in " << strShaderType << " shader:\n" << strInfoLog;
        delete[] strInfoLog;
    }
}

Shader::~Shader() { glDeleteShader(m_shaderId); }
