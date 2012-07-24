#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

class Shader
{
public:
    Shader(GLenum type, const std::string& filename);
    ~Shader();

    GLenum type() { return m_type; }
    GLuint id() { return m_shaderId; }

private:
    GLuint m_shaderId;
    GLenum m_type;
};

#endif // SHADER_H
