#pragma once

#include "safeInclude/includeGL.h"

#include <glm/glm.hpp>

class ColorDepthFramebuffer
{
public:
    ColorDepthFramebuffer() = default;
    ~ColorDepthFramebuffer();

    ColorDepthFramebuffer(const ColorDepthFramebuffer&) = delete;
    ColorDepthFramebuffer& operator=(const ColorDepthFramebuffer&) = delete;
    ColorDepthFramebuffer(ColorDepthFramebuffer&& other) noexcept;
    ColorDepthFramebuffer& operator=(ColorDepthFramebuffer&& other) noexcept;

    void ensure(glm::ivec2 requestedSize, GLuint restoreFramebuffer);
    void clear();

    GLuint framebuffer() const { return framebuffer_; }
    GLuint colorTexture() const { return colorTexture_; }
    glm::ivec2 size() const { return size_; }
    bool ready() const;

private:
    GLuint framebuffer_{};
    GLuint colorTexture_{};
    GLuint depthRenderbuffer_{};
    glm::ivec2 size_{};
};
