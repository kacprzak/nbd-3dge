#include "Text.h"

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

    for (std::size_t i = 0 ; i < text.length(); ++i) {
        const auto& c = m_font->getChar(text[i]);
        const auto& tex = m_font->getTexture(c);

        // upper left
        verts[i*6].x = (float) coursorX + c.xoffset;
        verts[i*6].y = (float) c.yoffset;
        verts[i*6].s = (float) c.x / (float) tex->getWidth();
        verts[i*6].t = 1.0f - (float) c.y / (float) tex->getHeight();

        // upper right
        verts[i*6+1].x = (float) c.width + coursorX + c.xoffset;
        verts[i*6+1].y = (float) c.yoffset;
        verts[i*6+1].s = (float) (c.x + c.width) / (float) tex->getWidth();
        verts[i*6+1].t = 1.0f - (float) c.y / (float) tex->getHeight();

        // lower right
        verts[i*6+2].x = (float) c.width + coursorX + c.xoffset;
        verts[i*6+2].y = (float) c.height + c.yoffset;
        verts[i*6+2].s = (float) (c.x + c.width) / (float) tex->getWidth();
        verts[i*6+2].t = 1.0f - (float) (c.y + c.height) / (float) tex->getHeight();

        verts[i*6+3] = verts[i*6];
        verts[i*6+4] = verts[i*6+2];
        
        // lower left
        verts[i*6+5].x = (float) coursorX + c.xoffset;
        verts[i*6+5].y = (float) c.height + c.yoffset;
        verts[i*6+5].s = (float) c.x / (float) tex->getWidth();
        verts[i*6+5].t = 1.0f - (float) (c.y + c.height) / (float) tex->getHeight();

        coursorX += c.xadvance;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Text::draw()
{
    if (m_shaderProgram) {
        m_shaderProgram->use();
    }

    m_font->getTexture(0)->bind(0);
    
    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, m_text.size() * 6);
}
