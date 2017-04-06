/* -*- c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include "Skybox.h"

#include "Mesh.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(TexturePtr front, TexturePtr right, TexturePtr back,
               TexturePtr left, TexturePtr top, TexturePtr bottom)
{
    m_textures.push_back(front);
    m_textures.push_back(back);
    m_textures.push_back(right);
    m_textures.push_back(left);
    m_textures.push_back(top);
    m_textures.push_back(bottom);

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
                                   -x, -x, -x, // 6
                                    x, -x, -x, // 5
                                    x,  x, -x, // 8
                                   -x,  x, -x, // 7
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
                                    x, -x, -x, // 5
                                   -x, -x, -x, // 6
                                   -x, -x,  x, // 2
                                    x, -x,  x, // 1
                                  };

    std::vector<float> texcoords = { // front
                                     0,  0, // 1
                                     1,  0, // 2
                                     1,  1, // 3
                                     0,  1, // 4
                                     // back
                                     0,  0, // 6
                                     1,  0, // 5
                                     1,  1, // 8
                                     0,  1, // 7
                                     // left
                                     0,  0, // 2
                                     1,  0, // 6
                                     1,  1, // 7
                                     0,  1, // 3
                                     // right
                                     0,  0, // 5
                                     1,  0, // 1
                                     1,  1, // 4
                                     0,  1, // 8
                                     // top
                                     0,  0, // 4
                                     1,  0, // 3
                                     1,  1, // 7
                                     0,  1, // 8
                                     // bottom
                                     0,  0, // 5
                                     1,  0, // 6
                                     1,  1, // 2
                                     0,  1};// 1

    vertices = quadsToTriangles3(vertices);
    texcoords = quadsToTriangles2(texcoords);

    std::vector<float> normals;
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

    glPushMatrix();
    glMultMatrixf(glm::value_ptr(m_modelMatrix));

    int start = 0;

    for (TexturePtr tex : m_textures) {
        if (tex.get() != nullptr) {
            tex->bind();
            m_mesh->draw(start, 6);
            start += 6;
        }
    }

    glPopMatrix();

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

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    //glDisable(GL_BLEND);

    int start = 0;

    for (TexturePtr tex : m_textures) {
        if (tex.get() != nullptr) {
            tex->bind();
            m_mesh->draw(start, 6);
            start += 6;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopAttrib();
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
    for (size_t i = 0; i < v.size() - 12;) {
        float va[4][3];

        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 3; ++k) {
                va[j][k] = v[i++];
            }
        }

        nv.push_back(va[0][0]);
        nv.push_back(va[0][1]);
        nv.push_back(va[0][2]);
        nv.push_back(va[1][0]);
        nv.push_back(va[1][1]);
        nv.push_back(va[1][2]);
        nv.push_back(va[2][0]);
        nv.push_back(va[2][1]);
        nv.push_back(va[2][2]);

        nv.push_back(va[0][0]);
        nv.push_back(va[0][1]);
        nv.push_back(va[0][2]);
        nv.push_back(va[2][0]);
        nv.push_back(va[2][1]);
        nv.push_back(va[2][2]);
        nv.push_back(va[3][0]);
        nv.push_back(va[3][1]);
        nv.push_back(va[3][2]);
    }
    return nv;
}

std::vector<float> Skybox::quadsToTriangles2(const std::vector<float>& v)
{
    std::vector<float> nv;
    for (size_t i = 0; i < v.size() - 8;) {
        float va[4][2];
        
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 2; ++k) {
                va[j][k] = v[i++];
            }
        }

        nv.push_back(va[0][0]);
        nv.push_back(va[0][1]);
        nv.push_back(va[1][0]);
        nv.push_back(va[1][1]);
        nv.push_back(va[2][0]);
        nv.push_back(va[2][1]);

        nv.push_back(va[0][0]);
        nv.push_back(va[0][1]);
        nv.push_back(va[2][0]);
        nv.push_back(va[2][1]);
        nv.push_back(va[3][0]);
        nv.push_back(va[3][1]);
    }
    return nv;
}