#ifndef HEIGHTFIELD_H
#define HEIGHTFIELD_H

#include <vector>

struct Heightfield
{
    std::vector<float> heights;
    int w;
    int h;
    float amplitude;
};

#endif // HEIGHTFIELD_H
