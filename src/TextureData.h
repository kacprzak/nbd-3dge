#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include <string>
#include <vector>

struct TextureData
{
    std::vector<std::string> filenames;
    bool linearColor = true;
    bool clamp       = false;
    std::string name;
};

#endif /* TEXTUREDATA_H */
