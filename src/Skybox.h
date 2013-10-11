/* -*- c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#ifndef SKYBOX_H
#define SKYBOX_H

#include "Texture.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>

class Skybox
{
public:
    Skybox(TexturePtr front, TexturePtr right, TexturePtr back,
           TexturePtr left, TexturePtr top, TexturePtr bottom = TexturePtr());

    void draw() const;
    void draw(ShaderProgram *program) const;

    void moveTo(float x, float y = 0.0f, float z = 0.0f);
    void moveTo(const glm::vec3& pos);

private:
    static std::vector<float> quadsToTriangles3(const std::vector<float>& vertices);
    static std::vector<float> quadsToTriangles2(const std::vector<float>& vertices);

    std::vector<TexturePtr> m_textures;

    glm::mat4 m_modelMatrix;

    MeshPtr m_mesh;
};

#endif // SKYBOX_H
