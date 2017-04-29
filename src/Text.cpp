#include "Text.h"

#include <glm/gtc/matrix_transform.hpp>

Text::Text(std::shared_ptr<Font> font)
    : m_font{font}
{
}

void Text::setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void Text::setText(const std::string& text)
{
    struct Vertex { float x, y, s, t; };

    m_text = text;

    std::vector<Vertex> verts;
    // Six vertices per glyph
    std::size_t size = text.size() * 6;
    verts.resize(size);

    float coursorX = 0.0f;
    float coursorY = 0.0f;

    char prev = '\0';
    for (std::size_t i = 0 ; i < text.size(); ++i) {
        char curr = text[i];
        if (curr == '\n') {
            prev = '\0';
            coursorX = 0.0f;
            coursorY -= m_font->getLineHeight();
        }
        
        const auto& c = m_font->getChar(curr);
        const auto& tex = m_font->getTexture(c);

        const float texW = (float)tex->getWidth();
        const float texH = (float)tex->getHeight();

        //if (coursorX == 0.0f)
        //    coursorX += c.xoffset;

        int kerning = m_font->getKerning(prev, curr);
        //int kerning = 0;
        
        auto& topLeft = verts[i*6];
        topLeft.x = coursorX + c.xoffset + kerning;
        topLeft.y = coursorY - c.yoffset;
        topLeft.s = c.x / texW;
        topLeft.t = 1.0f - c.y / texH;

        auto& bottomLeft = verts[i*6+1];
        bottomLeft.x = topLeft.x;
        bottomLeft.y = coursorY - (c.height + c.yoffset);
        bottomLeft.s = topLeft.s;
        bottomLeft.t = 1.0f - (c.y + c.height) / texH;

        auto& bottomRight = verts[i*6+2];
        bottomRight.x = coursorX + c.xoffset + c.width + kerning;
        bottomRight.y = bottomLeft.y;
        bottomRight.s = (c.x + c.width) / texW;
        bottomRight.t = bottomLeft.t;

        verts[i*6+3] = bottomRight;
        
        auto& topRight = verts[i*6+4];
        topRight.x = bottomRight.x;
        topRight.y = topLeft.y;
        topRight.s = bottomRight.s;
        topRight.t = topLeft.t;

        verts[i*6+5] = topLeft;
        
        coursorX += c.xadvance;

        prev = curr;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex)/2));

    glBindVertexArray(0);
    
    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(-1.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(0.005f));

}

void Text::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    if (m_shaderProgram) {
        m_shaderProgram->use();
        m_shaderProgram->setUniform("modelMatrix", m_modelMatrix);
    }

    m_font->getTexture(0)->bind(0);
    
    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, m_text.size() * 6);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
