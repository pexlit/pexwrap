#pragma once

#include "model/staticModelGeometry.h"
#include "safeInclude/includeGL.h"

#include <filesystem>

enum class TextureFileColorSpace
{
    Linear,
    Srgb,
};

enum class TextureWrap
{
    ClampToEdge,
    Repeat,
};

GLuint uploadRgbaTexture(
    const std::filesystem::path& path,
    TextureFileColorSpace colorSpace,
    TextureWrap wrap = TextureWrap::ClampToEdge);
GLuint uploadSrgbTexture(const std::filesystem::path& path);
GLuint uploadSrgbAlphaTexture(
    const std::filesystem::path& path,
    TextureWrap wrap = TextureWrap::ClampToEdge);
GLuint uploadLinearTexture(
    const std::filesystem::path& path,
    TextureWrap wrap = TextureWrap::ClampToEdge);
void destroyTexture(GLuint& texture);
GLuint uploadStaticModelTexture(
    const StaticModelGeometry& geometry,
    int textureIndex,
    bool srgb = true);
