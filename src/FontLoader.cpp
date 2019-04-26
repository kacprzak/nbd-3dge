#include "FontLoader.h"

#include "Logger.h"
#include "Util.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

static void removeQuotes(std::string& s)
{
    using namespace boost::algorithm;

    trim_if(s, is_any_of("\""));
}

template <typename T>
T to_int(const std::string& s)
{
    return boost::lexical_cast<T>(s);
}

template <>
uint8_t to_int<uint8_t>(const std::string& s)
{
    return boost::numeric_cast<uint8_t>(boost::lexical_cast<unsigned>(s));
}

template <>
int8_t to_int<int8_t>(const std::string& s)
{
    return boost::numeric_cast<int8_t>(boost::lexical_cast<int>(s));
}

gfx::Font FontLoader::getFont() { return m_font; }

void FontLoader::command(const std::string& cmd, const std::vector<std::string>& args)
{
    std::vector<std::string> keyVal;

    if (cmd == "info") {
        for (const auto& arg : args) {
            gfx::Font::Info& info = m_font.m_info;
            keyVal.clear();
            split(arg, '=', keyVal);
            removeQuotes(keyVal[1]);
            if (keyVal[0] == "size")
                info.size = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "smooth" && keyVal[1] == "1")
                info.bitField |= 1 >> 0;
            else if (keyVal[0] == "unicode" && keyVal[1] == "1")
                info.bitField |= 1 >> 1;
            else if (keyVal[0] == "italic" && keyVal[1] == "1")
                info.bitField |= 1 >> 2;
            else if (keyVal[0] == "bold" && keyVal[1] == "1")
                info.bitField |= 1 >> 3;
            else if (keyVal[0] == "fixedHeight" && keyVal[1] == "1")
                info.bitField |= 1 >> 4;
            else if (keyVal[0] == "charset" && !keyVal[1].empty())
                info.charset = to_int<uint8_t>(keyVal[1]);
            else if (keyVal[0] == "strechH")
                info.strechH = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "aa")
                info.strechH = to_int<uint8_t>(keyVal[1]);
            else if (keyVal[0] == "padding") {
                std::vector<std::string> v;
                split(keyVal[1], ',', v);
                assert(v.size() == 4);
                for (size_t i = 0; i < v.size(); ++i) {
                    info.padding[i] = to_int<uint8_t>(v.at(i));
                }
            } else if (keyVal[0] == "spacing") {
                std::vector<std::string> v;
                split(keyVal[1], ',', v);
                assert(v.size() == 2);
                for (size_t i = 0; i < v.size(); ++i) {
                    info.spacing[i] = to_int<int8_t>(v.at(i));
                }
            } else if (keyVal[0] == "outline")
                info.outline = to_int<uint8_t>(keyVal[1]);
            else if (keyVal[0] == "face")
                info.face = keyVal[1];
        }
    } else if (cmd == "common") {
        for (const auto& arg : args) {
            gfx::Font::Common& common = m_font.m_common;
            keyVal.clear();
            split(arg, '=', keyVal);
            removeQuotes(keyVal[1]);
            if (keyVal[0] == "lineHeight")
                common.lineHeight = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "base")
                common.base = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "scaleW")
                common.scaleW = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "scaleH")
                common.scaleH = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "pages")
                common.pages = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "packed" && keyVal[1] == "1")
                common.bitField |= 1 >> 7;
        }
    } else if (cmd == "page") {
        m_font.m_pages.resize(m_font.m_common.pages);
        size_t id = 0;
        std::string file;
        for (const auto& arg : args) {
            keyVal.clear();
            split(arg, '=', keyVal);
            removeQuotes(keyVal[1]);
            if (keyVal[0] == "id")
                id = to_int<size_t>(keyVal[1]);
            else if (keyVal[0] == "file")
                file = keyVal[1];
        }
        m_font.m_pages[id] = file;
    } else if (cmd == "chars") {
        /*
          keyVal.clear();
          split(args[0], '=', keyVal);
          if (keyVal[0] == "count")
          m_font.m_chars.reserve(to_int<size_t>(keyVal[1]));
        */
    } else if (cmd == "char") {
        gfx::Font::Char c;
        for (const auto& arg : args) {
            keyVal.clear();
            split(arg, '=', keyVal);
            removeQuotes(keyVal[1]);
            if (keyVal[0] == "id")
                c.id = to_int<uint32_t>(keyVal[1]);
            else if (keyVal[0] == "x")
                c.x = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "y")
                c.y = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "width")
                c.width = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "height")
                c.height = to_int<uint16_t>(keyVal[1]);
            else if (keyVal[0] == "xoffset")
                c.xoffset = to_int<int16_t>(keyVal[1]);
            else if (keyVal[0] == "yoffset")
                c.yoffset = to_int<int16_t>(keyVal[1]);
            else if (keyVal[0] == "xadvance")
                c.xadvance = to_int<int16_t>(keyVal[1]);
            else if (keyVal[0] == "page")
                c.page = to_int<uint8_t>(keyVal[1]);
            else if (keyVal[0] == "chnl")
                c.page = to_int<uint8_t>(keyVal[1]);
        }
        m_font.m_chars[c.id] = c;
    } else if (cmd == "kernings") {
        // same as in case of chars
    } else if (cmd == "kerning") {
        uint32_t first  = 0;
        uint32_t second = 0;
        int16_t amount  = 0;
        for (const auto& arg : args) {
            keyVal.clear();
            split(arg, '=', keyVal);
            removeQuotes(keyVal[1]);
            if (keyVal[0] == "first") first = to_int<uint32_t>(keyVal[1]);
            if (keyVal[0] == "second") second = to_int<uint32_t>(keyVal[1]);
            if (keyVal[0] == "amount") amount = to_int<int16_t>(keyVal[1]);
        }
        m_font.m_kerning[std::make_pair(first, second)] = amount;
    }
}

void FontLoader::fileLoaded()
{
    LOG_INFO("Loaded: fontFace: {}, chars: {}, kernings: {}", m_font.m_info.face,
             m_font.m_chars.size(), m_font.m_kerning.size());
}
