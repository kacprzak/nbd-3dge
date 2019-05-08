#include "Skybox.h"

#include "Camera.h"
#include "Mesh.h"
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gfx {

Skybox::Skybox()
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

    // clang-format off
    std::vector<glm::vec3> positions = {
        { x, -x,  x},
        {-x, -x,  x},
        {-x,  x,  x},
        { x,  x,  x},
        { x, -x, -x},
        {-x, -x, -x},
        {-x,  x, -x},
        { x,  x, -x}
    };

    std::vector<uint8_t> indices = {
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

    auto indicesBuffer = std::make_shared<Buffer>();
    indicesBuffer->loadData(indices.data(), sizeof(indices[0]) * indices.size());

    Accessor indicesAcc;
    indicesAcc.buffer = indicesBuffer;
    indicesAcc.type   = GL_UNSIGNED_BYTE;
    indicesAcc.count  = indices.size();
    indicesAcc.size   = 1;

    auto positionsBuffer = std::make_shared<Buffer>();
    positionsBuffer->loadData(positions.data(), sizeof(positions[0]) * positions.size());

    Accessor posAcc;
    posAcc.buffer = positionsBuffer;
    posAcc.type   = GL_FLOAT;
    posAcc.count  = positions.size();
    posAcc.size   = 3;

    std::array<Accessor, Accessor::Attribute::Size> attributes{};
    attributes[Accessor::Attribute::Position] = posAcc;

    m_mesh       = std::make_shared<Mesh>(attributes, indicesAcc, GL_TRIANGLES);
    m_mesh->name = "SKYBOX";
}

void Skybox::draw(const Camera* camera) const
{
    if (m_shaderProgram) {
        m_shaderProgram->use();

        // Move skybox with the camera
        glm::mat4 modelMatrix =
            glm::translate(glm::mat4(1.0f), glm::vec3{camera->worldTranslation()});

        m_shaderProgram->setUniform("projectionMatrix", camera->projectionMatrix());
        m_shaderProgram->setUniform("viewMatrix", camera->viewMatrix());
        m_shaderProgram->setUniform("modelMatrix", modelMatrix);
    } else {
        glUseProgram(0);
    }

    glDepthMask(GL_FALSE);

    if (m_textures[TextureUnit::BaseColor]) {
        m_textures[TextureUnit::BaseColor]->bind(TextureUnit::BaseColor);
        m_mesh->draw(m_shaderProgram.get());
    }

    glDepthMask(GL_TRUE);
}

} // namespace gfx