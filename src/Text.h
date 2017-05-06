#ifndef TEXT_H
#define TEXT_H

#include "Font.h"
#include "ShaderProgram.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Text final
{
  public:
    Text(std::shared_ptr<Font> font);
    ~Text();

    void setText(const std::string& text);

    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram);

    void draw();

  private:
    std::shared_ptr<Font> m_font;
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::string m_text;

    GLuint m_vao    = 0;
    GLuint m_buffer = 0;

    glm::mat4 m_modelMatrix;
    std::size_t m_vertsCount         = 0;
    std::size_t m_bufferSize         = 0;
    std::size_t m_bufferReservedSize = 0;
};

#endif // TEXT_H
