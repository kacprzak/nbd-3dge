#include "Skybox.h"

#include "Camera.h"
#include "Mesh.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(std::shared_ptr<Material> material)
    : m_material{material}
{
    const float x = 1.0f;

    /*
     *    6----7     y
     *   /|   /|     |
     *  2----3 |     o--x
     *  | 5--|-4    /
     *  |/   |/    z
     *  1----0
     *
     */

    MeshData md;
    md.primitive = GL_TRIANGLES;

    // clang-format off
    md.positions = {
        { x, -x,  x},
        {-x, -x,  x},
        {-x,  x,  x},
        { x,  x,  x},
        { x, -x, -x},
        {-x, -x, -x},
        {-x,  x, -x},
        { x,  x, -x}
    };

    md.indices = {
        // front
        0, 1, 3,
        1, 2, 3,
        // back
        4, 7, 5,
        5, 7, 6,
        // left
        1, 5, 2,
        2, 5, 6,
        // right
        0, 3, 4,
        4, 3, 7,
        // top
        6, 7, 3,
        3, 2, 6,
        // bottom
        0, 4, 5,
        5, 1, 0
    };
    // clang-format on

    m_mesh = std::make_shared<Mesh>(md);
}

void Skybox::draw(const Camera* camera) const
{
    if (m_shaderProgram) {
        m_shaderProgram->use();

        // Move skybox with the camera
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), camera->transformation()->position);

        m_shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        m_shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        m_shaderProgram->setUniform("modelMatrix", modelMatrix);
    } else {
        glUseProgram(0);
    }

    glDepthMask(GL_FALSE);

    if (m_material) {
        m_material->textures.at(0)->bind(0);
        m_mesh->draw();
    }

    glDepthMask(GL_TRUE);
}
