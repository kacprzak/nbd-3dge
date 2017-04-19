#ifndef FONT_H
#define FONT_H

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <map>

class FontLoader;

class Font
{
    friend class FontLoader;

    struct Char
    {
        int id;
        int x;
        int y;
        int width;
        int height;
        int xoffset;
        int yoffset;
        int xadvance;
        int page;
        int chnl;
    };
    
 public:
    Font(const std::string& filename);

 private:
    struct Info {
        std::string face;
        std::string charset;
        unsigned size;
        bool bold;
        bool italic;
        bool unicode;
        bool smooth;
        bool aa;
        int strechH;
        glm::ivec4 padding;
        glm::ivec4 spacing;
    } m_info;

    struct Common {
        int lineHeight;
        int base;
        int scaleW;
        int scaleH;
        int pages;
        bool packed;
    } m_common;

    struct Page {
        int id;
        std::string file;
    };

    std::vector<Page> m_pages;    
    std::map<int, Char> m_chars;
    std::map<std::pair<int, int>, char> m_kerning;
};

#endif // FONT_H
