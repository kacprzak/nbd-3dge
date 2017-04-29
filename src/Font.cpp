#include "Font.h"
#include "FontLoader.h"

Font::Font()
{
}

std::vector<std::string> Font::getTexturesFilenames() const
{
    return m_pages;
}

void Font::setTextures(const std::vector<std::shared_ptr<Texture>>& textures)
{
    m_textures = textures;
}

std::shared_ptr<Texture> Font::getTexture(const Char& c) const
{
    return m_textures.at(c.page);
}

std::shared_ptr<Texture> Font::getTexture(int page) const
{
    return m_textures.at(page);
}

Font::Char Font::getChar(char c) const
{
    return m_chars.at(c);
}

unsigned Font::getLineHeight() const
{
    return m_common.lineHeight;
}

int Font::getKerning(char prev, char next) const
{
    const std::pair<uint32_t, uint32_t> key{prev, next};
    const auto& it = m_kerning.find(key);
    if (it != std::end(m_kerning))
        return it->second;
    else
        return 0;
}
