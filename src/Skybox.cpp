#include "Skybox.h"

#include "Camera.h"
#include "Mesh.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(std::shared_ptr<Texture> front,
               std::shared_ptr<Texture> right,
               std::shared_ptr<Texture> back,
               std::shared_ptr<Texture> left,
               std::shared_ptr<Texture> top,
               std::shared_ptr<Texture> bottom)
{
    m_textures.push_back(front);
    m_textures.push_back(back);
    m_textures.push_back(right);
    m_textures.push_back(left);
    m_textures.push_back(top);
    m_textures.push_back(bottom);

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

    std::vector<float> normals;
    std::vector<unsigned short> empty2;

    Mesh *mesh = Mesh::create("skybox.obj",
                              vertices,
                              normals,
                              texcoords,
                              empty2,
                              GL_QUADS);
    
    m_mesh = std::shared_ptr<Mesh>{mesh};
}

void Skybox::draw(const Camera *camera) const
{
    if (m_shaderProgram) {
        m_shaderProgram->use();

        // Move skybox with the camera
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), camera->position());

        GLint pMtxLoc = glGetUniformLocation(m_shaderProgram->id(), "projectionMatrix");
        GLint vMtxLoc = glGetUniformLocation(m_shaderProgram->id(), "viewMatrix");
        GLint mMtxLoc = glGetUniformLocation(m_shaderProgram->id(), "modelMatrix");

        glUniformMatrix4fv(pMtxLoc, 1, GL_FALSE, glm::value_ptr(camera->projectionMatrix()));
        glUniformMatrix4fv(vMtxLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix()));
        glUniformMatrix4fv(mMtxLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    } else {
        m_shaderProgram->use(false);
    }

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    int start = 0;

    for (const std::shared_ptr<Texture>& tex : m_textures) {
        if (tex) {
            tex->bind();
            m_mesh->draw(start, 4);
            start += 4;
        }
    }

    glPopAttrib();
}
