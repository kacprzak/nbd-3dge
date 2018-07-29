#include "Framebuffer.h"

#include <glm/glm.hpp>

#include "Logger.h"
#include "Texture.h"

Framebuffer::Framebuffer(glm::ivec3 size)
    : m_shadowMap{Texture::createShadowMap(size)}
{
    glGenFramebuffers(1, &m_framebufferId);
    LOG_TRACE("Created Framebuffer: {}", m_framebufferId);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap.m_textureId, 0, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOG_FATAL("Framebuffer error, status: {}", status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_framebufferId);
    LOG_TRACE("Deleted Framebuffer: {}", m_framebufferId);
}

void Framebuffer::bindForWriting(unsigned cascadeIndex)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebufferId);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap.m_textureId, 0,
                              cascadeIndex);
}

void Framebuffer::bindDepthComponent(int textureUnit) { m_shadowMap.bind(textureUnit); }
