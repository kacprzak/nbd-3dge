#ifndef SKYBOX_H
#define SKYBOX_H

#include "actor.h"

class SkyBox : public Actor
{
    typedef Actor super;

public:
    SkyBox();

    void draw();
    void update(float delta);

private:
    std::vector<float> quadsToTriangles3(const std::vector<float>& vertices);
    std::vector<float> quadsToTriangles2(const std::vector<float>& vertices);
};

#endif // SKYBOX_H
