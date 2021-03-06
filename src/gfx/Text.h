#ifndef TEXT_H
#define TEXT_H

#include "Font.h"
#include "ShaderProgram.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

namespace gfx {

class Text final
{
  public:
    Text(const std::shared_ptr<Font>& font);
    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;
    ~Text();

    void setText(const std::string& text);

    //! Position of leftTop corner of text relavtive to leftTop screen coord
    void setPosition(glm::vec3 pos);

    void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);

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

} // namespace gfx

#endif // TEXT_H
