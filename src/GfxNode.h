#ifndef GFXNODE_H
#define GFXNODE_H

#include "Mesh.h"
#include "Texture.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>

class Camera;

class GfxNode
{
 public:
    GfxNode(const std::string& name);
    virtual ~GfxNode() {};

    const std::string& name() { return m_name; }

    void setTextures(std::vector<std::shared_ptr<Texture>> textures);
    void addTexture(std::shared_ptr<Texture> texture);
    
    void setMesh(std::shared_ptr<Mesh> mesh);
    void setShaderProgram(std::shared_ptr<ShaderProgram> shaderProgram);

    void move(float x, float y = 0.0f, float z = 0.0f);
    void move(const glm::vec3& pos);
    void moveTo(float x, float y = 0.0f, float z = 0.0f);
    void moveTo(const glm::vec3& pos);
    void moveForward(float distance);
    void moveRight(float distance);
    void moveLeft(float distance);

    glm::vec3 position() const { return m_position; }

    virtual void draw(const Camera* camera) const;
    virtual void draw(ShaderProgram* shaderProgram, const Camera* camera) const;
    virtual void update(float delta);

    void setOrientation(float x, float y, float z);
    void rotate(float x, float y, float z);

    glm::vec3 orientation() const { return m_orientation; }

    void setScale(float s);

    bool isIdle() const { return m_state == Idle; }
    bool isActive() const { return m_state == Active; }
    bool isDestroyed() const { return m_state == Destroyed; }

 protected:

    enum State {
        Idle,
        Active,
        Destroyed
    };

    const glm::mat4& modelMatrix() const { return m_modelMatrix; }

    void setState(State state) { m_state = state; }

    virtual void refresh();

    bool m_dirty = true;

 private:
    void rebuildModelMatrix();

    const std::string m_name;

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<ShaderProgram> m_shaderProgram;
    std::vector<std::shared_ptr<Texture>> m_textures;
    
    State m_state;

    glm::vec3 m_position;
    glm::vec3 m_orientation;
    glm::vec3 m_scale;

    glm::mat4 m_modelMatrix;
};

#endif // GFXNODE_H
