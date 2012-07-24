#ifndef SKYBOX_H
#define SKYBOX_H

#include "texture.h"
#include "mesh.h"
#include <boost/shared_ptr.hpp>

class Skybox
{
public:
    Skybox(boost::shared_ptr<Texture> tex);

    void draw();

private:
    std::vector<float> quadsToTriangles3(const std::vector<float>& vertices);
    std::vector<float> quadsToTriangles2(const std::vector<float>& vertices);

    boost::shared_ptr<Texture> m_texture;
    boost::shared_ptr<Mesh> m_mesh;
};

#endif // SKYBOX_H
