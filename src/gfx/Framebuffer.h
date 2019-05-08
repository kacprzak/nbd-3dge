#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>
#include <glm/fwd.hpp>

#include "Texture.h"

namespace gfx {

class Framebuffer final
{
    OSTREAM_FRIEND(Framebuffer);

  public:
    Framebuffer(glm::ivec3 size);
    ~Framebuffer();

    void bindForWriting(unsigned cascadeIndex);
    void bindDepthComponent(int textureUnit);

  private:
    GLuint m_framebufferId = 0;
    Texture m_shadowMap;
};

OSTREAM_IMPL_1(gfx::Framebuffer, m_framebufferId)

} // namespace gfx

#endif /* FRAMEBUFFER_H */
