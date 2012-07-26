#include "skybox.h"

#include "mesh.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(TexturePtr tex)
{
    m_texture = tex;
    m_modelMatrix = glm::mat4(1.0f);

    float x = 50.0f;

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

    float twoBy3 = 2.0 / 3.0;
    float oneBy3 = 1.0 / 3.0;

    std::vector<float> texcoords = { // front
                                     0,     oneBy3, // 1
                                     0.25,  oneBy3, // 2
                                     0.25,  twoBy3, // 3
                                     0.0,   twoBy3, // 4
                                     // back
                                     0.75,  oneBy3, // 5
                                     0.75,  twoBy3, // 8
                                     0.5,   twoBy3, // 7
                                     0.5,   oneBy3, // 6
                                     // left
                                     0.25,  oneBy3, // 2
                                     0.5,   oneBy3, // 6
                                     0.5,   twoBy3, // 7
                                     0.25,  twoBy3, // 3
                                     // right
                                     0.75,  oneBy3, // 5
                                     1,     oneBy3, // 1
                                     1,     twoBy3, // 4
                                     0.75,  twoBy3, // 8
                                     // top
                                     0.25,  1,    // 4
                                     0.25,  twoBy3, // 3
                                     0.5,   twoBy3, // 7
                                     0.5,   1,    // 8
                                     // bottom
                                     0.25,  0,    // 1
                                     0.5,   0,    // 5
                                     0.5,   oneBy3, // 6
                                     0.25,  oneBy3};// 2
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
    m_mesh = MeshPtr(mesh);
}

void Skybox::draw() const
{
    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    //glDisable(GL_BLEND);

    m_texture->bind();
    m_mesh->draw();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopAttrib();
}

void Skybox::draw(ShaderProgram *program) const
{
    if (program) {
        GLint modelMatrixUnif = glGetUniformLocation(program->id(), "modelMatrix");
        glUniformMatrix4fv(modelMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    }

    draw();
}

void Skybox::moveTo(float x, float y, float z)
{
    m_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Skybox::moveTo(const glm::vec3& pos)
{
    m_modelMatrix = glm::translate(glm::mat4(1.0f), pos);
}

std::vector<float> Skybox::quadsToTriangles3(const std::vector<float>& v)
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

std::vector<float> Skybox::quadsToTriangles2(const std::vector<float>& v)
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
