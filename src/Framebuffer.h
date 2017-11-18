#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture.h"

#include <GL/glew.h>

class Framebuffer final
{
  public:
    Framebuffer();
    ~Framebuffer();

    void bindForWriting();
    void bindDepthComponent(int textureUnit);

  private:
    GLuint m_framebufferId = 0;
    Texture m_shadowMap;
};

#endif /* FRAMEBUFFER_H */
