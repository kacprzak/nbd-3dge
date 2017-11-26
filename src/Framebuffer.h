#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>
#include <glm/fwd.hpp>

#include "Texture.h"

class Framebuffer final
{
  public:
    Framebuffer(glm::ivec2 size);
    ~Framebuffer();

    void bindForWriting();
    void bindDepthComponent(int textureUnit);

  private:
    GLuint m_framebufferId = 0;
    Texture m_shadowMap;
};

#endif /* FRAMEBUFFER_H */
