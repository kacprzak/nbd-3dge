#include "Shader.h"

#include "Util.h"
//#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace boost::algorithm;

Shader::Shader(GLenum type, const std::string& filename)
{
    //path p(file);
    m_type = type;

    string source;
    ifstream f(filename.c_str());

    if (f.is_open() == true) {
        source.assign((istreambuf_iterator<char>(f) ),
                      (istreambuf_iterator<char>()  ));
        f.close();
    } else {
        throw std::runtime_error{"File not found: " + filename};
    }

    m_shaderId = glCreateShader(m_type);

    std::cout << "Compiling: " << extractFilename(filename) << "\tid: " << m_shaderId << std::endl;

    const GLchar *shaderSource[1];
    shaderSource[0] = source.c_str();

    glShaderSource(m_shaderId, 1, shaderSource, 0);
    glCompileShader(m_shaderId);

    GLint status;
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(m_shaderId, infoLogLength, NULL, strInfoLog);

        const char *strShaderType = NULL;
        switch(type)
        {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        std::cerr << "Compile failure in " << strShaderType << " shader:\n"
                  << strInfoLog << "\n";
        delete[] strInfoLog;
    }
}

Shader::~Shader()
{
    glDeleteShader(m_shaderId);
}
