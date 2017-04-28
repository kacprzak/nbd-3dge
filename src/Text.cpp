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

    for (std::size_t i = 0 ; i < text.size(); ++i) {
        const auto& c = m_font->getChar(text[i]);
        const auto& tex = m_font->getTexture(c);

        const float texW = (float)tex->getWidth();
        const float texH = (float)tex->getHeight();
        
        // upper left
        verts[i*6].x = coursorX + c.xoffset;
        verts[i*6].y = -c.yoffset;
        verts[i*6].s = c.x / texW;
        verts[i*6].t = 1.0f - c.y / texH;

        // lower left
        verts[i*6+1].x = coursorX + c.xoffset;
        verts[i*6+1].y = -(c.height + c.yoffset);
        verts[i*6+1].s = c.x / texW;
        verts[i*6+1].t = 1.0f - (c.y + c.height) / texH;

        // lower right
        verts[i*6+2].x = c.width + coursorX + c.xoffset;
        verts[i*6+2].y = -(c.height + c.yoffset);
        verts[i*6+2].s = (c.x + c.width) / texW;
        verts[i*6+2].t = 1.0f - (c.y + c.height) / texH;

        verts[i*6+3] = verts[i*6+2];
        
        // upper right
        verts[i*6+4].x = c.width + coursorX + c.xoffset;
        verts[i*6+4].y = -c.yoffset;
        verts[i*6+4].s = (c.x + c.width) / texW;
        verts[i*6+4].t = 1.0f - c.y / texH;

        verts[i*6+5] = verts[i*6];
        
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
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex)/2));

    glBindVertexArray(0);
}

void Text::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    if (m_shaderProgram) {
        m_shaderProgram->use();
    }

    m_font->getTexture(0)->bind(0);
    
    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, m_text.size() * 6);
    glDisable(GL_BLEND);
}
