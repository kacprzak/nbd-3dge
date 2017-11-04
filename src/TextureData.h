#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include <string>

struct TextureData
{
    std::string filename;
    bool linearColor = true;
    bool clamp       = false;
};

#endif /* TEXTUREDATA_H */
