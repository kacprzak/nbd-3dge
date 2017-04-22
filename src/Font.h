#ifndef FONT_H
#define FONT_H

#include <vector>
#include <string>
#include <map>

class FontLoader;

class Font final
{
    friend class FontLoader;
    
 public:
    Font();

 private:
    struct Info {
        uint16_t size;
        uint8_t bitField = 0; //< bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4: fixedHeigth, bits 5-7: reserved
        uint8_t charset;
        uint16_t strechH;
        uint8_t aa;
        uint8_t padding[4]; //< up, right, down, left
        int8_t spacing[2]; //< horiz, vert (in AngelFont doc it's uint8_t but Hiero can output ints )
        uint8_t outline;
        std::string face;
    } m_info;

    struct Common {
        uint16_t lineHeight;
        uint16_t base;
        uint16_t scaleW;
        uint16_t scaleH;
        uint16_t pages;
        uint8_t bitField = 0; //< bits 0-6: reserved, bit 7: packed
    } m_common;

    std::vector<std::string> m_pages; //< texture files for each page

    struct Char
    {
        uint32_t id;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        int16_t xoffset;
        int16_t yoffset;
        int16_t xadvance;
        uint8_t page;
        uint8_t chnl;
    };
    
    std::map<uint32_t, Char> m_chars; //< char.id -> char
    std::map<std::pair<uint32_t, uint32_t>, int16_t> m_kerning; //< (first, second) -> amount
};

#endif // FONT_H
