#include "colorDepthFramebuffer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utility>

namespace
{
bool hasGlContext()
{
    return glfwGetCurrentContext() != nullptr;
}
}

ColorDepthFramebuffer::~ColorDepthFramebuffer()
{
    clear();
}

ColorDepthFramebuffer::ColorDepthFramebuffer(ColorDepthFramebuffer&& other) noexcept
    : framebuffer_(std::exchange(other.framebuffer_, 0)),
      colorTexture_(std::exchange(other.colorTexture_, 0)),
      depthRenderbuffer_(std::exchange(other.depthRenderbuffer_, 0)),
      size_(std::exchange(other.size_, glm::ivec2(0)))
{
}

ColorDepthFramebuffer& ColorDepthFramebuffer::operator=(ColorDepthFramebuffer&& other) noexcept
{
    if (this == &other)
        return *this;
    clear();
    framebuffer_ = std::exchange(other.framebuffer_, 0);
    colorTexture_ = std::exchange(other.colorTexture_, 0);
    depthRenderbuffer_ = std::exchange(other.depthRenderbuffer_, 0);
    size_ = std::exchange(other.size_, glm::ivec2(0));
    return *this;
}

void ColorDepthFramebuffer::ensure(glm::ivec2 requestedSize, GLuint restoreFramebuffer)
{
    if (requestedSize.x <= 0 || requestedSize.y <= 0 || !hasGlContext())
        return;
    if (framebuffer_ != 0 && size_ == requestedSize)
        return;

    clear();

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    glGenTextures(1, &colorTexture_);
    glBindTexture(GL_TEXTURE_2D, colorTexture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        requestedSize.x,
        requestedSize.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        colorTexture_,
        0);

    glGenRenderbuffers(1, &depthRenderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer_);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT24,
        requestedSize.x,
        requestedSize.y);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        depthRenderbuffer_);

    size_ = requestedSize;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, restoreFramebuffer);
}

void ColorDepthFramebuffer::clear()
{
    if (framebuffer_ == 0 && colorTexture_ == 0 && depthRenderbuffer_ == 0)
    {
        size_ = glm::ivec2(0);
        return;
    }

    if (hasGlContext())
    {
        if (depthRenderbuffer_ != 0)
            glDeleteRenderbuffers(1, &depthRenderbuffer_);
        if (colorTexture_ != 0)
            glDeleteTextures(1, &colorTexture_);
        if (framebuffer_ != 0)
            glDeleteFramebuffers(1, &framebuffer_);
    }
    framebuffer_ = 0;
    colorTexture_ = 0;
    depthRenderbuffer_ = 0;
    size_ = glm::ivec2(0);
}

bool ColorDepthFramebuffer::ready() const
{
    return framebuffer_ != 0 && size_.x > 0 && size_.y > 0;
}
