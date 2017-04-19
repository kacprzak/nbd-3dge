#include "Skybox.h"

#include "Camera.h"
#include "Mesh.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(std::shared_ptr<Texture> texture)
    : m_texture{texture}
{
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

    Mesh *mesh = Mesh::create("skybox.obj",
                              vertices, {}, {}, {},
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

    if (m_texture) {
        m_texture->bind(GL_TEXTURE0);
        m_mesh->draw();
    }

    glPopAttrib();
}
