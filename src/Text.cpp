#include "Text.h"

#include <glm/gtc/matrix_transform.hpp>

Text::Text(std::shared_ptr<Font> font)
    : m_font{font}
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);
}

Text::~Text()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_buffer);
}

void Text::setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void Text::setText(const std::string& text)
{
    if (text == m_text)
        return;

    struct Vertex
    {
        float x, y, s, t;
    };

    m_text = text;

    std::vector<Vertex> verts;
    // Six vertices per glyph
    verts.reserve(text.size() * 6);

    float coursorX = 0.0f;
    float coursorY = 0.0f;

    const float texW = m_font->getScaleW();
    const float texH = m_font->getScaleH();

    char prev = '\0';
    for (std::size_t i = 0; i < text.size(); ++i) {
        char curr = text[i];
        if (curr == '\n') {
            prev     = '\0';
            coursorX = 0.0f;
            coursorY -= m_font->getLineHeight();
        }

        const auto& c = m_font->getChar(curr);
        int kerning   = m_font->getKerning(prev, curr);

        if (c.width != 0 && c.height != 0) {
            Vertex v[6];

            auto& topLeft = v[0];
            topLeft.x     = coursorX + c.xoffset + kerning;
            topLeft.y     = coursorY - c.yoffset;
            topLeft.s     = c.x / texW;
            topLeft.t     = 1.0f - c.y / texH;

            auto& bottomLeft = v[1];
            bottomLeft.x     = topLeft.x;
            bottomLeft.y     = coursorY - (c.height + c.yoffset);
            bottomLeft.s     = topLeft.s;
            bottomLeft.t     = 1.0f - (c.y + c.height) / texH;

            auto& bottomRight = v[2];
            bottomRight.x     = coursorX + c.xoffset + c.width + kerning;
            bottomRight.y     = bottomLeft.y;
            bottomRight.s     = (c.x + c.width) / texW;
            bottomRight.t     = bottomLeft.t;

            v[3] = bottomRight;

            auto& topRight = v[4];
            topRight.x     = bottomRight.x;
            topRight.y     = topLeft.y;
            topRight.s     = bottomRight.s;
            topRight.t     = topLeft.t;

            v[5] = topLeft;

            for (size_t j = 0; j < 6; ++j)
                verts.emplace_back(v[j]);
        }

        coursorX += c.xadvance + kerning;
        prev = curr;
    }

    m_vertsCount = verts.size();
    m_bufferSize = verts.size() * sizeof(Vertex);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    if (m_bufferSize > m_bufferReservedSize) {
        glBufferData(GL_ARRAY_BUFFER, m_bufferSize, 0, GL_DYNAMIC_DRAW);
        m_bufferReservedSize = m_bufferSize;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, m_bufferSize, &verts[0]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex) / 2));

    glBindVertexArray(0);

    m_modelMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(-1.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(0.001f));
}

void Text::draw()
{
    if (m_text.empty())
        return;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    if (m_shaderProgram) {
        m_shaderProgram->use();
        m_shaderProgram->setUniform("modelMatrix", m_modelMatrix);
    }

    m_font->getTexture(0)->bind(0);

    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLES, 0, m_vertsCount);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
