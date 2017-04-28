#ifndef TEXT_H
#define TEXT_H

#include "Font.h"
#include "ShaderProgram.h"

#include <GL/glew.h>
#include <string>

class Text
{
 public:
    Text(std::shared_ptr<Font> font);

    void setText(const std::string& text);

    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram);

    void draw();

 private:
    std::shared_ptr<Font> m_font;
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::string m_text;

    GLuint m_vao;
    GLuint m_buffer;
    std::size_t m_bufferSize = 0;
};

#endif // TEXT_H
