#include "skybox.h"

#include "mesh.h"
#include <vector>

SkyBox::SkyBox()
{
    float x = 80.0f;

    /*
     *    7----8     y
     *   /|   /|     |
     *  3----4 |     o--x
     *  | 6--|-5    /
     *  |/   |/    z
     *  2----1
     *
     */

    std::vector<float> vertices = {// front
                                   x, -x,  x, // 1
                                   -x, -x,  x, // 2
                                   -x,  x,  x, // 3
                                   x,  x,  x, // 4
                                   // back
                                   x, -x, -x, // 5
                                   x,  x, -x, // 8
                                   -x,  x, -x, // 7
                                   -x, -x, -x, // 6
                                   // left
                                   -x, -x,  x, // 2
                                   -x, -x, -x, // 6
                                   -x,  x, -x, // 7
                                   -x,  x,  x, // 3
                                   // right
                                   x, -x, -x, // 5
                                   x, -x,  x, // 1
                                   x,  x,  x, // 4
                                   x,  x, -x, // 8
                                   // top
                                   x,  x,  x, // 4
                                   -x,  x,  x, // 3
                                   -x,  x, -x, // 7
                                   x,  x, -x, // 8
                                   // bottom
                                   x, -x,  x, // 1
                                   x, -x, -x, // 5
                                   -x, -x, -x, // 6
                                   -x, -x,  x, // 2
                                  };

    std::vector<float> normals = { // front
                                   0, 0, -1, // 1
                                   0, 0, -1, // 2
                                   0, 0, -1, // 3
                                   0, 0, -1, // 4
                                   // back
                                   0, 0, 1, // 5
                                   0, 0, 1, // 8
                                   0, 0, 1, // 7
                                   0, 0, 1, // 6
                                   // left
                                   1, 0, 0, // 2
                                   1, 0, 0, // 6
                                   1, 0, 0, // 7
                                   1, 0, 0, // 3
                                   // right
                                   -1, 0, 0, // 5
                                   -1, 0, 0, // 1
                                   -1, 0, 0, // 4
                                   -1, 0, 0, // 8
                                   // top
                                   0, -1, 0, // 4
                                   0, -1, 0, // 3
                                   0, -1, 0, // 7
                                   0, -1, 0, // 8
                                   // bottom
                                   0, 1, 0, // 1
                                   0, 1, 0, // 5
                                   0, 1, 0, // 6
                                   0, 1, 0};// 2

    std::vector<float> texcoords = { // front
                                     0,     1.0/3, // 1
                                     0.25,  1.0/3, // 2
                                     0.25,  2.0/3, // 3
                                     0.0,   2.0/3, // 4
                                     // back
                                     0.75,  1.0/3, // 5
                                     0.75,  2.0/3, // 8
                                     0.5,   2.0/3, // 7
                                     0.5,   1.0/3, // 6
                                     // left
                                     0.25,  1.0/3, // 2
                                     0.5,   1.0/3, // 6
                                     0.5,   2.0/3, // 7
                                     0.25,  2.0/3, // 3
                                     // right
                                     0.75,  1.0/3, // 5
                                     1,     1.0/3, // 1
                                     1,     2.0/3, // 4
                                     0.75,  2.0/3, // 8
                                     // top
                                     0.25,  1,    // 4
                                     0.25,  2.0/3, // 3
                                     0.5,   2.0/3, // 7
                                     0.5,   1,    // 8
                                     // bottom
                                     0.25,  0,    // 1
                                     0.5,   0,    // 5
                                     0.5,   1.0/3, // 6
                                     0.25,  1.0/3};// 2
    vertices = quadsToTriangles3(vertices);
    normals = quadsToTriangles3(normals);
    texcoords = quadsToTriangles2(texcoords);

    std::vector<unsigned short> empty2;

    Mesh *mesh = Mesh::create("skybox.obj",
                              vertices,
                              normals,
                              texcoords,
                              empty2,
                              GL_FLAT);
    setMesh(boost::shared_ptr<Mesh>(mesh));
}

void SkyBox::draw()
{
    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    //glDisable(GL_BLEND);

    super::draw();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopAttrib();
}

void SkyBox::update(float /*delta*/)
{
}

std::vector<float> SkyBox::quadsToTriangles3(const std::vector<float>& v)
{
    std::vector<float> nv;
    for (unsigned int i = 0; i < v.size();) {
        float v1[3] = { v[i++], v[i++], v[i++] };
        float v2[3] = { v[i++], v[i++], v[i++] };
        float v3[3] = { v[i++], v[i++], v[i++] };
        float v4[3] = { v[i++], v[i++], v[i++] };

        nv.push_back(v1[0]);
        nv.push_back(v1[1]);
        nv.push_back(v1[2]);
        nv.push_back(v2[0]);
        nv.push_back(v2[1]);
        nv.push_back(v2[2]);
        nv.push_back(v3[0]);
        nv.push_back(v3[1]);
        nv.push_back(v3[2]);

        nv.push_back(v1[0]);
        nv.push_back(v1[1]);
        nv.push_back(v1[2]);
        nv.push_back(v3[0]);
        nv.push_back(v3[1]);
        nv.push_back(v3[2]);
        nv.push_back(v4[0]);
        nv.push_back(v4[1]);
        nv.push_back(v4[2]);
    }
    return nv;
}

std::vector<float> SkyBox::quadsToTriangles2(const std::vector<float>& v)
{
    std::vector<float> nv;
    for (unsigned int i = 0; i < v.size();) {
        float v1[2] = { v[i++], v[i++] };
        float v2[2] = { v[i++], v[i++] };
        float v3[2] = { v[i++], v[i++] };
        float v4[2] = { v[i++], v[i++] };

        nv.push_back(v1[0]);
        nv.push_back(v1[1]);
        nv.push_back(v2[0]);
        nv.push_back(v2[1]);
        nv.push_back(v3[0]);
        nv.push_back(v3[1]);

        nv.push_back(v1[0]);
        nv.push_back(v1[1]);
        nv.push_back(v3[0]);
        nv.push_back(v3[1]);
        nv.push_back(v4[0]);
        nv.push_back(v4[1]);
    }
    return nv;
}
